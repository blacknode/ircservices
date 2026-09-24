/* The database facade: configuration, dispatch, and nothing else.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/db.c).
 *
 * There is no database code here.  This file holds the `database' block,
 * the driver (the core's own PostgreSQL driver, registered at start-up),
 * and the table of queries that are waiting for an answer -- and it exists
 * mostly so that the answer can be thrown away safely when the module that
 * asked for it is unloaded before it arrives.
 *
 * A query in flight is a DbCall with an identifier, and the driver is
 * handed the identifier rather than the callback.  That is the whole trick:
 * when a module goes away its calls are forgotten, and the driver's
 * db_complete() a second later finds nothing to call and drops the result
 * instead of jumping into an unmapped page.
 *
 * See include/db.h and docs/readme.database.
 */

#include "db.h"
#include "conffile.h"
#include "modules.h"
#include "services.h"

/*************************************************************************/

/* One query that has been accepted and not yet answered. */
typedef struct DbCall_ DbCall;
struct DbCall_ {
    DbCall* next;
    unsigned long id;      /* Identifier the driver holds */
    DbResultFn fn;         /* Callback, or NULL */
    void* user;            /* Caller's opaque pointer */
    struct Module_* owner; /* Module that asked, or NULL */
};

/* The registered driver, or NULL. */
static const struct DbDriver* db_driver;

/* Queries accepted and not yet answered, and the next identifier (never
 * reused, never zero). */
static DbCall* db_calls;
static unsigned long db_next_id = 1;

/* The published configuration, or NULL. */
static struct DatabaseConf* db_config;

/* Statistics. */
static unsigned int db_stat_pending, db_stat_total, db_stat_failed;

/* Text for each DbError, indexed by the enum. */
static const char* const db_error_text[DB_ERR_LAST] = {
    "no error",
    "the database is not available",
    "the database block is missing or unusable",
    "could not connect to the database",
    "the database did not answer in time",
    "the database connection pool is busy",
    "the query or its parameters are malformed",
    "the database rejected the statement",
    "no such table, column or function",
    "the database refused the operation",
    "a unique constraint was violated",
    "a foreign key constraint was violated",
    "a value was null where none is allowed",
    "a constraint was violated",
    "a value was not acceptable to the database",
    "the connection is read-only",
    "the transaction was rolled back; retry it",
    "the database is out of resources",
    "the database driver failed",
};

const char* db_strerror(enum DbError code)
{
    unsigned int index = (unsigned int)code;

    if (index >= DB_ERR_LAST)
        return "unknown database error";
    return db_error_text[index];
}

/*************************************************************************/
/*************************** The database block **************************/
/*************************************************************************/

/* Values as the directive table reads them. */
static char *cf_dsn, *cf_read_dsn, *cf_write_dsn, *cf_schema;
static int32 cf_pool, cf_read_pool, cf_write_pool;
static int32 cf_timeout, cf_migration_timeout, cf_save_timeout;
static int32 cf_sync_timeout;

ConfigDirective db_directives[] = {
    {"dsn", {{CD_STRING, CF_DIRREQ, &cf_dsn}}},
    {"migration_timeout", {{CD_TIMEMSEC, 0, &cf_migration_timeout}}},
    {"pool", {{CD_POSINT, 0, &cf_pool}}},
    {"read", {{CD_STRING, 0, &cf_read_dsn}}},
    {"read_pool", {{CD_POSINT, 0, &cf_read_pool}}},
    {"save_timeout", {{CD_TIMEMSEC, 0, &cf_save_timeout}}},
    {"schema", {{CD_STRING, 0, &cf_schema}}},
    {"sync_timeout", {{CD_TIMEMSEC, 0, &cf_sync_timeout}}},
    {"timeout", {{CD_TIMEMSEC, 0, &cf_timeout}}},
    {"write", {{CD_STRING, 0, &cf_write_dsn}}},
    {"write_pool", {{CD_POSINT, 0, &cf_write_pool}}},
    {NULL}};

/* Release every string in `conf' and zero it. */
static void db_conf_release(struct DatabaseConf* conf)
{
    int role;

    free(conf->dbconf_dsn);
    for (role = 0; role < DB_ROLE_LAST; role++)
        free(conf->dbconf_role_dsn[role]);
    free(conf->dbconf_schema);
    memset(conf, 0, sizeof(*conf));
}

/* Compare two strings that may be NULL. */
static int db_str_differ(const char* a, const char* b)
{
    if (!a || !b)
        return a != b;
    return strcmp(a, b) != 0;
}

/* Nonzero if `a' and `b' would build different pools or tables. */
static int db_conf_differ(const struct DatabaseConf* a,
                          const struct DatabaseConf* b)
{
    int role;

    if (db_str_differ(a->dbconf_dsn, b->dbconf_dsn) ||
        db_str_differ(a->dbconf_schema, b->dbconf_schema) ||
        a->dbconf_timeout_ms != b->dbconf_timeout_ms ||
        a->dbconf_migration_ms != b->dbconf_migration_ms ||
        a->dbconf_save_ms != b->dbconf_save_ms ||
        a->dbconf_sync_ms != b->dbconf_sync_ms)
        return 1;
    for (role = 0; role < DB_ROLE_LAST; role++) {
        if (db_str_differ(a->dbconf_role_dsn[role],
                          b->dbconf_role_dsn[role]) ||
            a->dbconf_role_pool[role] != b->dbconf_role_pool[role])
            return 1;
    }
    return 0;
}

/* Clamp a timeout read from the configuration to [1, max], defaulting to
 * `def' when unset.  Asking for more than the maximum is corrected rather
 * than refused, so that Services do not fail to start over it, but it is
 * said out loud. */
static int db_clamp_timeout(const char* name, int32 ms, int def, int max)
{
    if (ms <= 0)
        return def;
    if (ms > max) {
        config_error(conf_filename(), 0,
                     "database: %s of %dms exceeds the %dms maximum;"
                     " using %dms",
                     name, (int)ms, max, max);
        return max;
    }
    return ms;
}

void db_config_apply(void)
{
    static unsigned int generation;
    struct DatabaseConf conf;
    int role;

    memset(&conf, 0, sizeof(conf));
    conf.dbconf_dsn = cf_dsn ? sstrdup(cf_dsn) : NULL;
    conf.dbconf_role_dsn[DB_ROLE_READ] =
        cf_read_dsn && *cf_read_dsn ? sstrdup(cf_read_dsn) : NULL;
    conf.dbconf_role_dsn[DB_ROLE_WRITE] =
        cf_write_dsn && *cf_write_dsn ? sstrdup(cf_write_dsn) : NULL;
    conf.dbconf_schema =
        sstrdup(cf_schema && *cf_schema ? cf_schema : DB_DEFAULT_SCHEMA);
    conf.dbconf_role_pool[DB_ROLE_READ] = cf_read_pool ? cf_read_pool
                                          : cf_pool    ? cf_pool
                                                       : DB_DEFAULT_POOL;
    conf.dbconf_role_pool[DB_ROLE_WRITE] = cf_write_pool ? cf_write_pool
                                           : cf_pool     ? cf_pool
                                                         : DB_DEFAULT_POOL;
    for (role = 0; role < DB_ROLE_LAST; role++) {
        if (conf.dbconf_role_pool[role] > DB_MAX_POOL) {
            config_error(conf_filename(), 0,
                         "database: pool of %d exceeds the maximum of %d;"
                         " using %d",
                         conf.dbconf_role_pool[role], DB_MAX_POOL,
                         DB_MAX_POOL);
            conf.dbconf_role_pool[role] = DB_MAX_POOL;
        }
    }
    conf.dbconf_timeout_ms = db_clamp_timeout(
        "timeout", cf_timeout, DB_TIMEOUT_DEFAULT_MS, DB_TIMEOUT_MAX_MS);
    conf.dbconf_migration_ms = db_clamp_timeout(
        "migration_timeout", cf_migration_timeout,
        DB_MIGRATION_TIMEOUT_DEFAULT, DB_MIGRATION_TIMEOUT_MAX);
    conf.dbconf_sync_ms =
        db_clamp_timeout("sync_timeout", cf_sync_timeout,
                         DB_SYNC_TIMEOUT_DEFAULT_MS, DB_TIMEOUT_MAX_MS);
    conf.dbconf_save_ms =
        db_clamp_timeout("save_timeout", cf_save_timeout,
                         DB_SAVE_TIMEOUT_DEFAULT, DB_SAVE_TIMEOUT_MAX);

    /* An unchanged block changes nothing: the generation only moves when
     * something the driver cares about did, so a REHASH that did not touch
     * the block does not tear down the connection pools. */
    if (db_config && !db_conf_differ(db_config, &conf)) {
        db_conf_release(&conf);
        return;
    }

    if (!db_config)
        db_config = scalloc(1, sizeof(*db_config));
    else
        db_conf_release(db_config);
    *db_config = conf;
    db_config->dbconf_generation = ++generation;

    if (db_driver && db_driver->dbdrv_reconfigure)
        (*db_driver->dbdrv_reconfigure)();
}

const struct DatabaseConf* db_conf(void)
{
    return db_config;
}

const char* db_conf_dsn(enum DbRole role)
{
    if (!db_config || role < 0 || role >= DB_ROLE_LAST)
        return NULL;
    if (db_config->dbconf_role_dsn[role])
        return db_config->dbconf_role_dsn[role];
    return db_config->dbconf_dsn;
}

int db_conf_pool(enum DbRole role)
{
    if (!db_config || role < 0 || role >= DB_ROLE_LAST)
        return DB_DEFAULT_POOL;
    return db_config->dbconf_role_pool[role];
}

int db_conf_timeout(void)
{
    int ms = db_config ? db_config->dbconf_timeout_ms : DB_TIMEOUT_DEFAULT_MS;

    /* Clamped on apply already; clamped again here because this is the
     * value a driver arms its deadline with. */
    return (ms > 0 && ms <= DB_TIMEOUT_MAX_MS) ? ms : DB_TIMEOUT_MAX_MS;
}

int db_conf_migration_timeout(void)
{
    int ms = db_config ? db_config->dbconf_migration_ms
                       : DB_MIGRATION_TIMEOUT_DEFAULT;

    return (ms > 0 && ms <= DB_MIGRATION_TIMEOUT_MAX)
               ? ms
               : DB_MIGRATION_TIMEOUT_MAX;
}

int db_conf_save_timeout(void)
{
    int ms = db_config ? db_config->dbconf_save_ms : DB_SAVE_TIMEOUT_DEFAULT;

    return (ms > 0 && ms <= DB_SAVE_TIMEOUT_MAX) ? ms : DB_SAVE_TIMEOUT_MAX;
}

int db_conf_sync_timeout(void)
{
    int ms = db_config ? db_config->dbconf_sync_ms : DB_SYNC_TIMEOUT_DEFAULT_MS;

    return (ms > 0 && ms <= DB_TIMEOUT_MAX_MS) ? ms : DB_TIMEOUT_MAX_MS;
}

const char* db_conf_schema(void)
{
    return db_config && db_config->dbconf_schema ? db_config->dbconf_schema
                                                 : DB_DEFAULT_SCHEMA;
}

unsigned int db_conf_generation(void)
{
    return db_config ? db_config->dbconf_generation : 0;
}

/*************************************************************************/
/******************************** Drivers ********************************/
/*************************************************************************/

int db_register_driver(const struct DbDriver* driver)
{
    if (!driver || !driver->dbdrv_name || !driver->dbdrv_submit ||
        !driver->dbdrv_release) {
        log("database: refusing an incomplete driver");
        return 0;
    }
    if (db_driver) {
        log("database: driver %s is already registered; refusing %s",
            db_driver->dbdrv_name, driver->dbdrv_name);
        return 0;
    }
    db_driver = driver;
    log_debug(1, "database: driver %s registered", driver->dbdrv_name);
    return 1;
}

/* Take one call off db_calls; returns it detached (still allocated), or
 * NULL. */
static DbCall* db_call_take(unsigned long id)
{
    DbCall **call_p, *call;

    for (call_p = &db_calls; (call = *call_p) != NULL; call_p = &call->next) {
        if (call->id == id) {
            *call_p = call->next;
            db_stat_pending--;
            return call;
        }
    }
    return NULL;
}

/* Fail every call still waiting, with `code'.  The callbacks run here, in
 * the main thread, which is where they would have run anyway. */
static void db_fail_all(enum DbError code)
{
    struct DbResult res;
    DbCall* call;

    while ((call = db_calls) != NULL) {
        db_calls = call->next;
        db_stat_pending--;
        db_stat_failed++;
        if (call->fn) {
            memset(&res, 0, sizeof(res));
            res.err.dberr_code = code;
            strscpy(res.err.dberr_message, db_strerror(code),
                    sizeof(res.err.dberr_message));
            (*call->fn)(&res, call->user);
        }
        free(call);
    }
}

void db_unregister_driver(void)
{
    const struct DbDriver* driver = db_driver;

    if (!driver)
        return;
    /* The driver stops first, answering what it had queued; then whatever
     * is left is failed here, since nothing will ever answer it now. */
    if (driver->dbdrv_shutdown)
        (*driver->dbdrv_shutdown)();
    db_driver = NULL;
    db_fail_all(DB_ERR_UNAVAILABLE);
}

void db_drop_module(struct Module_* mod)
{
    DbCall **call_p, *call;

    if (!mod)
        return;
    /* The module is going away, so its callbacks are dropped in silence
     * rather than called.  The driver may still complete them later;
     * db_complete() will find nothing and release the result. */
    for (call_p = &db_calls; (call = *call_p) != NULL;) {
        if (call->owner == mod) {
            *call_p = call->next;
            db_stat_pending--;
            free(call);
        }
        else {
            call_p = &call->next;
        }
    }
}

void db_shutdown(void)
{
    db_unregister_driver();
    if (db_config) {
        db_conf_release(db_config);
        free(db_config);
        db_config = NULL;
    }
}

void db_report(void)
{
    log("database: %u quer%s pending, %u accepted, %u failed", db_stat_pending,
        db_stat_pending == 1 ? "y" : "ies", db_stat_total, db_stat_failed);
    if (db_driver && db_driver->dbdrv_report)
        (*db_driver->dbdrv_report)();
}

/*************************************************************************/
/******************************** Dispatch *******************************/
/*************************************************************************/

/* Reject `query' before it reaches the driver. */
static enum DbError db_check_query(const struct DbQuery* query)
{
    unsigned int n;

    if (!query || !query->sql || !*query->sql)
        return DB_ERR_PARAM;
    if (query->params) {
        for (n = 0; query->params[n]; n++) {
            if (n >= DB_MAX_PARAMS)
                return DB_ERR_PARAM;
        }
    }
    return DB_OK;
}

/* Record a new call; returns it, already on db_calls. */
static DbCall* db_call_new(struct Module_* mod, DbResultFn cb, void* user)
{
    DbCall* call = scalloc(1, sizeof(*call));

    call->id = db_next_id++;
    call->fn = cb;
    call->user = user;
    call->owner = mod;
    /* On the list before the driver is asked: a driver that manages to
     * complete the call from inside its own submit must find it there. */
    call->next = db_calls;
    db_calls = call;
    db_stat_pending++;
    return call;
}

/* The driver refused `id': withdraw the call, unless the driver already
 * completed it on the way out. */
static void db_call_withdraw(unsigned long id)
{
    DbCall* call = db_call_take(id);

    free(call);
}

static enum DbError db_submit(struct Module_* mod, const struct DbQuery* query,
                              DbResultFn cb, void* user, enum DbRole role)
{
    enum DbError err;
    unsigned long id;

    if ((err = db_check_query(query)) != DB_OK)
        return err;
    if (!db_driver)
        return DB_ERR_UNAVAILABLE;
    if (!db_config)
        return DB_ERR_CONFIG;

    id = db_call_new(mod, cb, user)->id;
    err = (*db_driver->dbdrv_submit)(id, query, role);
    if (err != DB_OK) {
        db_call_withdraw(id);
        return err;
    }
    db_stat_total++;
    return DB_OK;
}

enum DbError db_query(struct Module_* mod, const struct DbQuery* query,
                      DbResultFn cb, void* user)
{
    return db_submit(mod, query, cb, user, DB_ROLE_READ);
}

enum DbError db_exec(struct Module_* mod, const struct DbQuery* query,
                     DbResultFn cb, void* user)
{
    return db_submit(mod, query, cb, user, DB_ROLE_WRITE);
}

enum DbError db_migrate(struct Module_* mod,
                        const struct DbMigration* migration, DbResultFn cb,
                        void* user)
{
    enum DbError err;
    unsigned long id;

    if (!migration || !migration->dbm_sql || !*migration->dbm_sql ||
        !migration->dbm_module || !*migration->dbm_module ||
        !migration->dbm_name || !*migration->dbm_name)
        return DB_ERR_PARAM;
    if (!db_driver || !db_driver->dbdrv_migrate)
        return DB_ERR_UNAVAILABLE;
    if (!db_config)
        return DB_ERR_CONFIG;

    id = db_call_new(mod, cb, user)->id;
    err = (*db_driver->dbdrv_migrate)(id, migration);
    if (err != DB_OK) {
        db_call_withdraw(id);
        return err;
    }
    db_stat_total++;
    return DB_OK;
}

void db_complete(unsigned long id, struct json_t* data, unsigned int rows,
                 enum DbError code, const char* message)
{
    struct DbResult res;
    DbCall* call = db_call_take(id);

    if (code != DB_OK)
        db_stat_failed++;
    if (call && call->fn) {
        memset(&res, 0, sizeof(res));
        res.data = data;
        res.rows = rows;
        res.err.dberr_code = code;
        strscpy(res.err.dberr_message,
                message && *message ? message : db_strerror(code),
                sizeof(res.err.dberr_message));
        (*call->fn)(&res, call->user);
    }
    free(call);
    /* Released even when nobody was left to look at it: the reference
     * count is the driver's. */
    if (data && db_driver)
        (*db_driver->dbdrv_release)(data);
}

/* Fill `res' for a synchronous call that ended with `code'. */
static enum DbError db_sync_result(struct DbResult* res, struct json_t* data,
                                   unsigned int rows, enum DbError code)
{
    memset(res, 0, sizeof(*res));
    if (code != DB_OK) {
        db_stat_failed++;
        if (data && db_driver)
            (*db_driver->dbdrv_release)(data);
        data = NULL;
        rows = 0;
    }
    res->data = data;
    res->rows = rows;
    res->err.dberr_code = code;
    strscpy(res->err.dberr_message, db_strerror(code),
            sizeof(res->err.dberr_message));
    return code;
}

enum DbError db_query_sync(struct Module_* mod, const struct DbQuery* query,
                           struct DbResult* res)
{
    struct json_t* data = NULL;
    unsigned int rows = 0;
    enum DbError err;

    if ((err = db_check_query(query)) != DB_OK)
        return db_sync_result(res, NULL, 0, err);
    if (!db_driver || !db_driver->dbdrv_query_sync)
        return db_sync_result(res, NULL, 0, DB_ERR_UNAVAILABLE);
    if (!db_config)
        return db_sync_result(res, NULL, 0, DB_ERR_CONFIG);
    db_stat_total++;
    err = (*db_driver->dbdrv_query_sync)(query, &data, &rows);
    return db_sync_result(res, data, rows, err);
}

enum DbError db_migrate_sync(const struct DbMigration* migration,
                             struct DbResult* res)
{
    struct json_t* data = NULL;
    enum DbError err;

    if (!migration || !migration->dbm_sql || !migration->dbm_module ||
        !migration->dbm_name)
        return db_sync_result(res, NULL, 0, DB_ERR_PARAM);
    if (!db_driver || !db_driver->dbdrv_migrate_sync)
        return db_sync_result(res, NULL, 0, DB_ERR_UNAVAILABLE);
    if (!db_config)
        return db_sync_result(res, NULL, 0, DB_ERR_CONFIG);
    db_stat_total++;
    err = (*db_driver->dbdrv_migrate_sync)(migration, &data);
    return db_sync_result(res, data, data ? db_rows(data) : 0, err);
}

void db_result_free(struct DbResult* res)
{
    if (res && res->data && db_driver)
        (*db_driver->dbdrv_release)(res->data);
    if (res)
        res->data = NULL;
}

/*************************************************************************/
/***************************** Reading results ***************************/
/*************************************************************************/

unsigned int db_rows(struct json_t* data)
{
    if (!data || !db_driver || !db_driver->dbdrv_rows)
        return 0;
    return (*db_driver->dbdrv_rows)(data);
}

const char* db_row_str(struct json_t* data, unsigned int row,
                       const char* column)
{
    const char* value;

    if (!data || !column || !db_driver || !db_driver->dbdrv_row_str)
        return "";
    value = (*db_driver->dbdrv_row_str)(data, row, column);
    return value ? value : "";
}

long long db_row_int(struct json_t* data, unsigned int row, const char* column)
{
    if (!data || !column || !db_driver || !db_driver->dbdrv_row_int)
        return 0;
    return (*db_driver->dbdrv_row_int)(data, row, column);
}

/*************************************************************************/

int db_available(void)
{
    return db_driver && db_config;
}

const char* db_driver_name(void)
{
    return db_driver ? db_driver->dbdrv_name : NULL;
}

unsigned int db_calls_pending(void)
{
    return db_stat_pending;
}

unsigned int db_calls_total(void)
{
    return db_stat_total;
}

unsigned int db_calls_failed(void)
{
    return db_stat_failed;
}

/*************************************************************************/

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
