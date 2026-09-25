/* Migrations: running them, at start-up and for an operator.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/migration_run.c).
 *
 * Two ways in, for two different moments:
 *
 *   - At start-up (and when a module setting MODULE_APPLY_MIGRATIONS is
 *     loaded), migrations are applied synchronously, one after the other,
 *     on the main thread's own connection: nothing can run until they
 *     have, and nothing else is running yet.
 *
 *   - For an operator (OperServ MIGRATION), everything is asynchronous: a
 *     job is a list of steps, and one step at a time goes to db_migrate()
 *     and comes back through a callback.  A job copies everything it needs
 *     -- the module's name and the SQL of every step -- because the module
 *     that owns them can be unloaded while its migration runs, and
 *     remembers the operator by nick, because the operator may be gone by
 *     the time the answer comes back.
 *
 * Validation is migration.c.  See docs/readme.migrations.
 */

#include "services.h"
#include "db.h"
#include "migration.h"
#include "modules.h"

#include <stdarg.h>

/* The core's own migrations, embedded by the build (src/migrations/). */
extern const struct MigrationFile services_core_migrations[];

/* The core's set, validated at start-up and kept for OperServ MIGRATION
 * STATUS core. */
static struct MigrationSet* core_set;

/*************************************************************************/
/***************************** Applied versions **************************/
/*************************************************************************/

/* Nonzero if `version' is among the rows `applied'. */
static int migration_is_applied(struct json_t* applied, unsigned int version)
{
    unsigned int i, rows = db_rows(applied);

    for (i = 0; i < rows; i++) {
        if ((unsigned int)db_row_int(applied, i, "version") == version)
            return 1;
    }
    return 0;
}

/* Read the applied versions of `module', synchronously.  A missing
 * migrations table means nothing is applied yet -- the state the core's own
 * v1 exists to fix.  Returns nonzero on success. */
static int migration_applied_sync(const char* module, struct DbResult* res)
{
    struct DbParam name = {DB_TYPE_TEXT, module, DB_FORMAT_TEXT};
    struct DbParam* params[] = {&name, NULL};
    struct DbQuery query = {"select version from migrations"
                            " where module_name = $1 order by version",
                            params};
    struct DbQuery exists = {"select to_regclass('migrations') is not null"
                             " as present",
                             NULL};

    /* Asked first rather than inferred from an error, so that a fresh
     * database does not start with an error in the log. */
    if (db_query_sync(NULL, &exists, res) != DB_OK) {
        log("migration: cannot reach the database: %s",
            res->err.dberr_message);
        return 0;
    }
    if (!db_row_int(res->data, 0, "present"))
        return 1; /* nothing applied: an empty result */
    db_result_free(res);
    if (db_query_sync(NULL, &query, res) == DB_OK)
        return 1;
    if (res->err.dberr_code == DB_ERR_UNDEFINED)
        return 1;
    log("migration: cannot read the migrations table: %s",
        res->err.dberr_message);
    return 0;
}

/*************************************************************************/
/******************************* At start-up *****************************/
/*************************************************************************/

int migration_apply_now(const struct MigrationSet* set)
{
    struct DbResult applied, res;
    unsigned int i, done = 0;
    int ok = 1;

    if (!set || !migration_applied_sync(set->ms_module, &applied))
        return 0;
    for (i = 0; i < set->ms_count; i++) {
        const struct Migration* m = &set->ms_list[i];
        struct DbMigration migration;

        if (migration_is_applied(applied.data, m->mg_version))
            continue;
        memset(&migration, 0, sizeof(migration));
        migration.dbm_module = set->ms_module;
        migration.dbm_version = m->mg_version;
        migration.dbm_name = m->mg_name;
        migration.dbm_sql = m->mg_up;
        migration.dbm_exec_by = "Services";
        if (db_migrate_sync(&migration, &res) != DB_OK) {
            log("migration: %s v%u_%s failed: %s -- %u applied",
                set->ms_module, m->mg_version, m->mg_name,
                res.err.dberr_message, done);
            db_result_free(&res);
            ok = 0;
            break;
        }
        log("migration: %s v%u_%s applied in %s", set->ms_module,
            m->mg_version, m->mg_name,
            db_row_str(res.data, 0, "exec_duration"));
        db_result_free(&res);
        done++;
    }
    db_result_free(&applied);
    return ok;
}

int migration_core_start(void)
{
    const char* err = NULL;

    /* The core set goes through the same validator as any module's.  It
     * should never fail, which is exactly why it is worth checking. */
    if (!core_set &&
        !(core_set = migration_build(MIGRATION_CORE, services_core_migrations,
                                     &err))) {
        log("migration: Services' own migrations are broken: %s",
            err ? err : "none found");
        return 0;
    }
    return migration_apply_now(core_set);
}

void migration_check_pending(const struct MigrationSet* set)
{
    struct DbResult applied;
    unsigned int i, pending = 0;

    if (!set || !migration_applied_sync(set->ms_module, &applied))
        return;
    for (i = 0; i < set->ms_count; i++) {
        if (!migration_is_applied(applied.data, set->ms_list[i].mg_version))
            pending++;
    }
    db_result_free(&applied);
    if (pending)
        log("migration: module %s has %u pending migration%s; apply with"
            " OperServ MIGRATION APPLY %s",
            set->ms_module, pending, pending == 1 ? "" : "s", set->ms_module);
}

/*************************************************************************/
/***************************** Operator jobs *****************************/
/*************************************************************************/

/* Who to answer: the module whose reply function it is, the function,
 * and the operator's nick. */
typedef struct {
    struct Module_* owner;
    MigrationReplyFn reply;
    char nick[NICKMAX];
} MigrationAsker;

/* One migration a job is going to run. */
typedef struct {
    unsigned int version;
    char name[MIGRATION_NAME_LEN + 1];
    char* sql; /* A copy */
} MigrationStep;

/* A sequence of migrations, running one at a time. */
typedef struct MigrationJob_ MigrationJob;
struct MigrationJob_ {
    MigrationJob* next;
    MigrationAsker asker;
    char module[MIGRATION_MODULE_LEN + 1];
    int revert;
    unsigned int count, at, done;
    MigrationStep* steps;
};

/* A question asked of the database, and who asked it. */
typedef enum {
    MIGRATION_WANT_STATUS,
    MIGRATION_WANT_APPLY,
    MIGRATION_WANT_REVERT,
    MIGRATION_WANT_LIST,
} MigrationWant;

typedef struct MigrationQuery_ MigrationQuery;
struct MigrationQuery_ {
    MigrationQuery* next;
    MigrationAsker asker;
    char module[MIGRATION_MODULE_LEN + 1];
    MigrationWant want;
    unsigned int bound; /* APPLY's upto, REVERT's downto */
};

static MigrationJob* migration_jobs;
static MigrationQuery* migration_queries;

/* Tell the operator something, if their module is still loaded. */
static void migration_reply(const MigrationAsker* asker, const char* fmt,
                            ...) FORMAT(printf, 2, 3);
static void migration_reply(const MigrationAsker* asker, const char* fmt, ...)
{
    char text[512];
    va_list args;

    if (!asker->reply)
        return;
    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);
    (*asker->reply)(asker->nick, text);
}

/* Report the progress of a job: to its operator, to the log, and -- since
 * a schema change is not a private act -- to the other operators. */
static void migration_report(MigrationJob* job, const char* fmt, ...)
    FORMAT(printf, 2, 3);
static void migration_report(MigrationJob* job, const char* fmt, ...)
{
    char text[512];
    va_list args;

    va_start(args, fmt);
    vsnprintf(text, sizeof(text), fmt, args);
    va_end(args);
    log("migration: %s", text);
    migration_reply(&job->asker, "%s", text);
    wallops(NULL, "Migration (%s): %s", job->asker.nick, text);
}

static void migration_job_free(MigrationJob* job)
{
    MigrationJob** p;
    unsigned int i;

    for (p = &migration_jobs; *p; p = &(*p)->next) {
        if (*p == job) {
            *p = job->next;
            break;
        }
    }
    for (i = 0; i < job->count; i++)
        free(job->steps[i].sql);
    free(job->steps);
    free(job);
}

static void migration_query_free(MigrationQuery* query)
{
    MigrationQuery** p;

    for (p = &migration_queries; *p; p = &(*p)->next) {
        if (*p == query) {
            *p = query->next;
            break;
        }
    }
    free(query);
}

/* Is a job already running for `module'?  Two operators migrating one
 * module at once would interleave their steps. */
static int migration_job_running(const char* module)
{
    MigrationJob* job;

    for (job = migration_jobs; job; job = job->next) {
        if (strcmp(job->module, module) == 0)
            return 1;
    }
    return 0;
}

/* The declared set of a loaded module, or NULL. */
static const struct MigrationSet* migration_set_of(const char* module)
{
    Module* mod;

    if (migration_reserved_name(module))
        return core_set;
    mod = module_find(module);
    return mod ? module_migrations(mod) : NULL;
}

static void migration_run_step(MigrationJob* job);

/* One step finished.  Main thread. */
static void migration_step_done(const struct DbResult* res, void* user)
{
    MigrationJob* job = user;
    MigrationStep* step = &job->steps[job->at];

    if (res->err.dberr_code != DB_OK) {
        migration_report(job, "%s v%u_%s (%s) failed: %s -- %u of %u %s",
                         job->module, step->version, step->name,
                         job->revert ? "revert" : "apply",
                         res->err.dberr_message, job->done, job->count,
                         job->revert ? "reverted" : "applied");
        migration_job_free(job);
        return;
    }
    job->done++;
    migration_report(job, "%s v%u_%s %s in %s", job->module, step->version,
                     step->name, job->revert ? "reverted" : "applied",
                     res->rows ? db_row_str(res->data, 0, "exec_duration")
                               : "an unrecorded time");
    if (++job->at >= job->count) {
        migration_report(job, "%s: %u migration%s %s", job->module,
                         job->done, job->done == 1 ? "" : "s",
                         job->revert ? "reverted" : "applied");
        migration_job_free(job);
        return;
    }
    migration_run_step(job);
}

/* Send the current step to the database. */
static void migration_run_step(MigrationJob* job)
{
    MigrationStep* step = &job->steps[job->at];
    struct DbMigration migration;
    enum DbError err;

    memset(&migration, 0, sizeof(migration));
    migration.dbm_module = job->module;
    migration.dbm_version = step->version;
    migration.dbm_name = step->name;
    migration.dbm_sql = step->sql;
    migration.dbm_exec_by = job->asker.nick;
    migration.dbm_revert = job->revert;
    if ((err = db_migrate(NULL, &migration, migration_step_done, job)) !=
        DB_OK) {
        migration_report(job, "%s v%u_%s could not be started: %s",
                         job->module, step->version, step->name,
                         db_strerror(err));
        migration_job_free(job);
    }
}

/* Start a job of `count' steps. */
static MigrationJob* migration_job_new(const MigrationQuery* query,
                                       unsigned int count)
{
    MigrationJob* job = scalloc(1, sizeof(*job));

    job->asker = query->asker;
    strbcpy(job->module, query->module);
    job->revert = query->want == MIGRATION_WANT_REVERT;
    job->count = count;
    job->steps = scalloc(count ? count : 1, sizeof(*job->steps));
    job->next = migration_jobs;
    migration_jobs = job;
    return job;
}

static void migration_job_step(MigrationJob* job, unsigned int index,
                               const struct Migration* m)
{
    MigrationStep* step = &job->steps[index];

    step->version = m->mg_version;
    strbcpy(step->name, m->mg_name);
    step->sql = sstrdup(job->revert ? m->mg_down : m->mg_up);
}

/* The migrations table, for LIST. */
static void migration_list_done(MigrationQuery* query,
                                const struct DbResult* res)
{
    unsigned int i, rows = db_rows(res->data);

    migration_reply(&query->asker, "%-18s %-5s %-26s %-9s %-12s %s", "Module",
                    "Ver", "Migration", "Duration", "By", "Applied");
    for (i = 0; i < rows; i++) {
        migration_reply(&query->asker, "%-18s v%-4u %-26s %-9s %-12s %.19s",
                        db_row_str(res->data, i, "module_name"),
                        (unsigned int)db_row_int(res->data, i, "version"),
                        db_row_str(res->data, i, "module_migration_name"),
                        db_row_str(res->data, i, "exec_duration"),
                        db_row_str(res->data, i, "exec_by"),
                        db_row_str(res->data, i, "created_at"));
    }
    migration_reply(&query->asker, "End of migration list (%u row%s)", rows,
                    rows == 1 ? "" : "s");
}

/* The applied versions are known: do what was asked. */
static void migration_applied_known(const struct DbResult* res, void* user)
{
    MigrationQuery* query = user;
    const struct MigrationSet* set;
    MigrationJob* job;
    unsigned int count = 0, i;

    if (res->err.dberr_code != DB_OK) {
        migration_reply(&query->asker,
                        "Could not read the migrations table: %s",
                        res->err.dberr_message);
        migration_query_free(query);
        return;
    }
    if (query->want == MIGRATION_WANT_LIST) {
        migration_list_done(query, res);
        migration_query_free(query);
        return;
    }
    /* The module may have been unloaded while the question was in
     * flight; everything below needs its declared set. */
    if (!(set = migration_set_of(query->module))) {
        migration_reply(&query->asker, "Module %s is no longer loaded",
                        query->module);
        migration_query_free(query);
        return;
    }

    switch (query->want) {
    case MIGRATION_WANT_STATUS:
        migration_reply(&query->asker, "Migrations for %s:", query->module);
        for (i = 0; i < set->ms_count; i++) {
            const struct Migration* m = &set->ms_list[i];
            migration_reply(&query->asker, "  v%u_%s  %s", m->mg_version,
                            m->mg_name,
                            migration_is_applied(res->data, m->mg_version)
                                ? "applied"
                                : "pending");
        }
        migration_reply(&query->asker,
                        "End of migrations for %s (%u declared)",
                        query->module, set->ms_count);
        migration_query_free(query);
        return;

    case MIGRATION_WANT_APPLY:
        for (i = 0; i < set->ms_count; i++) {
            if (query->bound && set->ms_list[i].mg_version > query->bound)
                break;
            if (!migration_is_applied(res->data, set->ms_list[i].mg_version))
                count++;
        }
        if (!count) {
            migration_reply(&query->asker, "Nothing to apply for %s",
                            query->module);
            migration_query_free(query);
            return;
        }
        job = migration_job_new(query, count);
        count = 0;
        for (i = 0; i < set->ms_count; i++) {
            if (query->bound && set->ms_list[i].mg_version > query->bound)
                break;
            if (!migration_is_applied(res->data, set->ms_list[i].mg_version))
                migration_job_step(job, count++, &set->ms_list[i]);
        }
        break;

    case MIGRATION_WANT_REVERT:
    default: {
        unsigned int newest = 0, lowest;

        for (i = set->ms_count; i > 0; i--) {
            if (migration_is_applied(res->data,
                                     set->ms_list[i - 1].mg_version)) {
                newest = set->ms_list[i - 1].mg_version;
                break;
            }
        }
        if (!newest) {
            migration_reply(&query->asker, "Nothing to revert for %s",
                            query->module);
            migration_query_free(query);
            return;
        }
        /* Without a bound it stops where it starts: going down destroys
         * whatever the migration built, so one step is the default and
         * "all of them" has to be asked for. */
        lowest = query->bound ? query->bound : newest;
        for (i = newest; i >= lowest; i--) {
            if (migration_is_applied(res->data, i))
                count++;
        }
        if (!count) {
            migration_reply(&query->asker, "Nothing to revert for %s",
                            query->module);
            migration_query_free(query);
            return;
        }
        job = migration_job_new(query, count);
        count = 0;
        for (i = newest; i >= lowest; i--) {
            if (migration_is_applied(res->data, i))
                migration_job_step(job, count++, &set->ms_list[i - 1]);
        }
        break;
    }
    }

    migration_query_free(query);
    migration_report(job, "%s: %s %u migration%s", job->module,
                     job->revert ? "reverting" : "applying", job->count,
                     job->count == 1 ? "" : "s");
    migration_run_step(job);
}

/* Ask the database, then do `want'. */
static void migration_ask(struct Module_* owner, MigrationReplyFn reply,
                          const char* nick, const char* module,
                          MigrationWant want, unsigned int bound)
{
    MigrationQuery* query = scalloc(1, sizeof(*query));
    struct DbParam name = {DB_TYPE_TEXT, NULL, DB_FORMAT_TEXT};
    struct DbParam* params[] = {&name, NULL};
    struct DbQuery sql;
    enum DbError err;

    query->asker.owner = owner;
    query->asker.reply = reply;
    strbcpy(query->asker.nick, nick);
    strbcpy(query->module, module ? module : "");
    query->want = want;
    query->bound = bound;
    query->next = migration_queries;
    migration_queries = query;

    if (want == MIGRATION_WANT_LIST) {
        sql.sql = "select version, module_name, module_migration_name,"
                  " exec_duration, exec_by, created_at from migrations"
                  " order by module_name, version";
        sql.params = NULL;
    }
    else {
        name.value = query->module;
        sql.sql = "select version from migrations where module_name = $1"
                  " order by version";
        sql.params = params;
    }
    if ((err = db_query(NULL, &sql, migration_applied_known, query)) !=
        DB_OK) {
        migration_reply(&query->asker, "Cannot reach the database: %s",
                        db_strerror(err));
        migration_query_free(query);
    }
}

/*************************************************************************/
/********************************* Commands ******************************/
/*************************************************************************/

void migration_cmd_list(struct Module_* owner, MigrationReplyFn reply,
                        const char* nick)
{
    migration_ask(owner, reply, nick, NULL, MIGRATION_WANT_LIST, 0);
}

void migration_cmd_status(struct Module_* owner, MigrationReplyFn reply,
                          const char* nick, const char* module)
{
    MigrationAsker asker = {owner, reply, ""};

    strbcpy(asker.nick, nick);
    if (strcmp(module, MIGRATION_CORE) != 0 && !module_find(module)) {
        migration_reply(&asker, "Module %s is not loaded", module);
        return;
    }
    if (strcmp(module, MIGRATION_CORE) != 0 && !migration_set_of(module)) {
        migration_reply(&asker, "Module %s ships no migrations", module);
        return;
    }
    migration_ask(owner, reply, nick, module, MIGRATION_WANT_STATUS, 0);
}

/* Shared front half of APPLY and REVERT. */
static void migration_cmd_run(struct Module_* owner, MigrationReplyFn reply,
                              const char* nick, const char* module,
                              int revert, unsigned int bound)
{
    MigrationAsker asker = {owner, reply, ""};
    const struct MigrationSet* set;
    Module* mod;

    strbcpy(asker.nick, nick);
    /* The core's own schema is not an operator's to drive: it is applied
     * when Services start, and never reverted. */
    if (migration_reserved_name(module)) {
        migration_reply(&asker, "The core's migrations are applied at"
                                " start-up and cannot be %s by hand",
                        revert ? "reverted" : "applied");
        return;
    }
    if (!(mod = module_find(module))) {
        migration_reply(&asker, "Module %s is not loaded", module);
        return;
    }
    if (!(set = migration_set_of(module))) {
        migration_reply(&asker, "Module %s ships no migrations", module);
        return;
    }
    /* Nor are those of the modules that keep Services' own data: they are
     * applied when the module is loaded, and reverting one would drop the
     * tables from under the module while it runs. */
    if (revert &&
        (module_flags(mod) & MODULE_APPLY_MIGRATIONS)) {
        migration_reply(&asker, "The migrations of %s are applied when it is"
                                " loaded and cannot be reverted by hand",
                        module);
        return;
    }
    if (bound > set->ms_count) {
        migration_reply(&asker, "Module %s has no v%u; it declares v1 to v%u",
                        module, bound, set->ms_count);
        return;
    }
    if (migration_job_running(module)) {
        migration_reply(&asker,
                        "A migration of %s is already running; wait for it",
                        module);
        return;
    }
    migration_ask(owner, reply, nick, module,
                  revert ? MIGRATION_WANT_REVERT : MIGRATION_WANT_APPLY,
                  bound);
}

void migration_cmd_apply(struct Module_* owner, MigrationReplyFn reply,
                         const char* nick, const char* module,
                         unsigned int upto)
{
    migration_cmd_run(owner, reply, nick, module, 0, upto);
}

void migration_cmd_revert(struct Module_* owner, MigrationReplyFn reply,
                          const char* nick, const char* module,
                          unsigned int downto)
{
    migration_cmd_run(owner, reply, nick, module, 1, downto);
}

void migration_drop_module(struct Module_* mod)
{
    MigrationJob* job;
    MigrationQuery* query;

    /* The jobs carry on -- they are the core's -- but nobody is left to
     * answer through. */
    for (job = migration_jobs; job; job = job->next) {
        if (job->asker.owner == mod)
            job->asker.reply = NULL;
    }
    for (query = migration_queries; query; query = query->next) {
        if (query->asker.owner == mod)
            query->asker.reply = NULL;
    }
}

void migration_shutdown(void)
{
    while (migration_jobs)
        migration_job_free(migration_jobs);
    while (migration_queries)
        migration_query_free(migration_queries);
    migration_free(core_set);
    core_set = NULL;
}

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
