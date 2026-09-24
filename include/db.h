/* Database access for the core and for modules: PostgreSQL, asynchronously.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/db.h), where the same API sits in front of a
 * driver module.  In Services the PostgreSQL driver is part of the core
 * (src/postgres/) and is registered by the core at start-up, so every
 * module can use it without linking against libpq or knowing that it
 * exists: these functions live in the executable, which exports its
 * symbols to the modules.
 *
 * Two things are true of this API, and both are the point of it:
 *
 *   - No driver type escapes.  A caller never sees a PGconn, a PGresult,
 *     an Oid or a libpq error.  It sends a DbQuery and receives a DbResult,
 *     whose rows are JSON (json_t, from jansson) and whose errors are the
 *     closed DbError enum.
 *
 *   - Nothing waits.  A query is a network round trip and the core is
 *     single-threaded, so db_query() hands the query to a pooled connection
 *     running in a worker thread (see worker.h) and returns at once.  The
 *     answer arrives later, in the main thread, as a call to the DbResultFn
 *     the caller gave -- a different pass through the main loop, and up to
 *     the configured timeout later (DB_TIMEOUT_MAX_MS at the very most).
 *     The user who asked may well have quit by then: never keep a User *
 *     across a query; keep the nick and look it up again.
 *
 * Statements are always prepared and values always bound: there is no
 * entry point here that takes an assembled statement, so a value can never
 * be read as SQL however it is spelled.
 *
 * Who owns what: the DbQuery, its DbParam list and every string they point
 * at belong to the caller and are copied before db_query() returns (they
 * may be local variables).  The DbResult handed to the callback belongs to
 * the driver and is released as soon as the callback returns; to keep the
 * rows, call json_incref() on DbResult.data.
 *
 * A call is tracked against the module that made it (pass THIS_MODULE;
 * the core passes NULL).  Unloading the module drops its pending callbacks
 * rather than calling into code that is no longer mapped, so a module does
 * not have to drain the database before it can be unloaded.
 *
 * The persistent Services data (nicknames, channels, memos, ...) does not
 * go through this API: it is kept in memory as always and written to
 * PostgreSQL by the table store (src/postgres/pg_store.c, see databases.h).
 * This API is for everything else a module wants to ask a database.
 *
 * See docs/readme.database.
 */

#ifndef DB_H
#define DB_H

#include <sys/types.h>

struct Module_;

/* jansson's json_t.  Declared, never defined here: a caller that means to
 * read DbResult.data includes <jansson.h>, which defines the same struct.
 * A caller that only wants a column or two uses db_row_str()/db_row_int()
 * below instead. */
struct json_t;

/* Longest translated error message, without the NUL. */
#define DB_ERRMSG_LEN 255

/* Hard ceiling on the timeout of a query, in milliseconds.  A query that
 * has not answered in five seconds has failed as far as Services are
 * concerned; waiting longer only holds a pooled connection hostage. */
#define DB_TIMEOUT_MAX_MS 5000

/* Query timeout when the database block does not set one. */
#define DB_TIMEOUT_DEFAULT_MS 5000

/* Longest a migration may run (an hour), and its default (a minute).
 * Migrations are DDL an operator is waiting for, not queries. */
#define DB_MIGRATION_TIMEOUT_MAX     3600000
#define DB_MIGRATION_TIMEOUT_DEFAULT 60000

/* Longest a save of the Services tables may run, and its default.  A save
 * rewrites every table in one transaction, in a worker thread; it does not
 * hold up the main loop however long it takes. */
#define DB_SAVE_TIMEOUT_MAX     600000
#define DB_SAVE_TIMEOUT_DEFAULT 60000

/* Timeout of a synchronous query (db_query_sync()), by default.  Short: the
 * main loop waits for it. */
#define DB_SYNC_TIMEOUT_DEFAULT_MS 1000

/* Most parameters one query may carry. */
#define DB_MAX_PARAMS 64

/* Largest pool accepted for one role, and the default. */
#define DB_MAX_POOL     64
#define DB_DEFAULT_POOL 4

/* Schema the tables live in when the database block does not say. */
#define DB_DEFAULT_SCHEMA "services"

/* Which side of the configuration a query is routed to.  A deployment
 * with a read replica points `read' at it and `write' at the primary; one
 * without simply sets `dsn', and both roles resolve to it. */
enum DbRole {
    DB_ROLE_READ,  /* Read-only work: db_query() */
    DB_ROLE_WRITE, /* Anything that changes data: db_exec() */
    DB_ROLE_LAST
};

/* Type of a bound parameter.  Abstract types, mapped by the driver to its
 * server's own; DB_TYPE_UNKNOWN leaves the type to the server to infer from
 * the statement, which is usually what a hand-written WHERE clause wants. */
enum DbType {
    DB_TYPE_UNKNOWN = 0, /* Let the server infer it */
    DB_TYPE_NULL,        /* SQL NULL; the value is ignored */
    DB_TYPE_BOOL,
    DB_TYPE_SMALLINT, /* 16-bit signed integer */
    DB_TYPE_INT,      /* 32-bit signed integer */
    DB_TYPE_BIGINT,   /* 64-bit signed integer */
    DB_TYPE_FLOAT,    /* Double-precision float */
    DB_TYPE_NUMERIC,  /* Exact decimal, carried as text */
    DB_TYPE_TEXT,
    DB_TYPE_BYTEA,
    DB_TYPE_JSON, /* JSON document, carried as text */
    DB_TYPE_UUID,
    DB_TYPE_DATE,
    DB_TYPE_TIME,
    DB_TYPE_TIMESTAMP,   /* Without time zone */
    DB_TYPE_TIMESTAMPTZ, /* With time zone */
    DB_TYPE_INET,
    DB_TYPE_LAST
};

/* How a parameter's bytes are encoded.  DB_FORMAT_TEXT is the one to use.
 * DB_FORMAT_BINARY is only accepted for fixed-width types (bool, the
 * integers, float, uuid, the date/time types), because DbParam carries no
 * length; send anything else as text, which for bytea means the \x form. */
enum DbFormat {
    DB_FORMAT_TEXT = 0,  /* value is a NUL-terminated string */
    DB_FORMAT_BINARY = 1 /* value is the type's wire encoding */
};

/* What went wrong, in terms that do not name a database product. */
enum DbError {
    DB_OK = 0,          /* No error */
    DB_ERR_UNAVAILABLE, /* No driver, or its pool is not running */
    DB_ERR_CONFIG,      /* The database block is missing or unusable */
    DB_ERR_CONNECT,     /* Could not reach the database */
    DB_ERR_TIMEOUT,     /* Took longer than the configured timeout */
    DB_ERR_BUSY,        /* Pool queue is full; retry later */
    DB_ERR_PARAM,       /* The query or its parameters are malformed */
    DB_ERR_SYNTAX,      /* The database rejected the statement */
    DB_ERR_UNDEFINED,   /* No such table, column or function */
    DB_ERR_PERMISSION,  /* The database refused on privilege grounds */
    DB_ERR_UNIQUE,      /* Unique constraint violated */
    DB_ERR_FOREIGN_KEY, /* Foreign key constraint violated */
    DB_ERR_NOT_NULL,    /* NOT NULL constraint violated */
    DB_ERR_CONSTRAINT,  /* Some other constraint violated */
    DB_ERR_DATA,        /* Bad value: wrong type, out of range, ... */
    DB_ERR_READONLY,    /* A write was sent to a read-only connection */
    DB_ERR_RETRY,       /* Deadlock or serialization failure; retryable */
    DB_ERR_RESOURCE,    /* Out of memory, connections or disk */
    DB_ERR_INTERNAL,    /* Anything the driver could not classify */
    DB_ERR_LAST
};

/* An error, translated.  dberr_code is DB_OK exactly when the query
 * succeeded.  dberr_message is the driver's own wording, never the
 * database's (which names tables, columns and fragments of the statement),
 * so it is safe to show to a user; the database's own account goes to the
 * log. */
struct DbErrDetails {
    enum DbError dberr_code;
    char dberr_message[DB_ERRMSG_LEN + 1];
};

/* One bound parameter.  A NULL value means SQL NULL. */
struct DbParam {
    enum DbType type;
    const char* value;
    enum DbFormat format;
};

/* A statement, with $1, $2 placeholders, and the values to bind to it:
 *
 *     struct DbParam  nick   = { DB_TYPE_TEXT, u->nick, DB_FORMAT_TEXT };
 *     struct DbParam* args[] = { &nick, NULL };
 *     struct DbQuery  q      = { "select score from scores where nick = $1",
 *                                args };
 *
 *     if (db_query(THIS_MODULE, &q, score_loaded, sstrdup(u->nick)) != DB_OK)
 *         ...refused here and now; score_loaded() will not be called...
 */
struct DbQuery {
    const char* sql;
    struct DbParam** params; /* NULL-terminated; NULL for none */
};

/* The answer to one query.  On success `data' is a JSON array with one
 * object per row, mapping column name to value -- empty for a statement
 * that returns nothing -- and `rows' is the number of rows returned or
 * affected.  On failure `data' is NULL and `err' says what happened.
 *
 * Column types map to JSON as: integers -> integer; float4/float8/numeric
 * -> number; bool -> true/false; json/jsonb -> the document itself; NULL ->
 * null; everything else (timestamps, uuid, inet, arrays, bytea) -> string. */
struct DbResult {
    struct json_t* data;
    struct DbErrDetails err;
    unsigned int rows;
};

/* Receives the answer to a query, in the main thread.  Called exactly once
 * for every call db_query()/db_exec() accepted, unless the module that made
 * the call is unloaded first.  `res' is valid only until this returns. */
typedef void (*DbResultFn)(const struct DbResult* res, void* user);

/*************************************************************************/

/* Asking for work.  This is the whole consumer API. */

/* Run a read-only query, on the `read' side of the configuration.
 * `mod' is THIS_MODULE (NULL from the core).  `query' is copied before this
 * returns.  `cb' (may be NULL) is called later with the result.  Returns
 * DB_OK when the query was accepted and `cb' will run; anything else is a
 * refusal here and now, and `cb' will never run. */
extern enum DbError db_query(struct Module_* mod, const struct DbQuery* query,
                             DbResultFn cb, void* user);

/* Run a statement that changes data, on the `write' side. */
extern enum DbError db_exec(struct Module_* mod, const struct DbQuery* query,
                            DbResultFn cb, void* user);

/* Run a read or a write and wait for the answer, on the main thread's own
 * connection, for at most the sync timeout (`sync_timeout', at most
 * DB_TIMEOUT_MAX_MS).  For a command that cannot reply before it knows the
 * answer; everything that can wait should use db_query()/db_exec(), and
 * data read often belongs in the cache (cache.h).  `res' is filled in as a
 * callback would see it, whatever the outcome; release it with
 * db_result_free().  Returns res->err.dberr_code. */
extern enum DbError db_query_sync(struct Module_* mod,
                                  const struct DbQuery* query,
                                  struct DbResult* res);

/* Release what db_query_sync() or db_migrate_sync() put in `res'. */
extern void db_result_free(struct DbResult* res);

/* Nonzero when the driver is up and the configuration is usable. */
extern int db_available(void);

/* Name of the driver ("postgres"), or NULL. */
extern const char* db_driver_name(void);

/* A short description of `code'; never NULL. */
extern const char* db_strerror(enum DbError code);

/*************************************************************************/

/* Reading a result without including jansson.  Every one of these is safe
 * on a NULL result and on a row or column that is not there. */

/* Number of rows in `data'. */
extern unsigned int db_rows(struct json_t* data);

/* One column of one row, as text ("" when missing or SQL NULL).  Numbers
 * and booleans are rendered; a JSON object or array comes back compact.
 * Valid until the next call. */
extern const char* db_row_str(struct json_t* data, unsigned int row,
                              const char* column);

/* One column of one row, as an integer (0 when missing, NULL or not a
 * number). */
extern long long db_row_int(struct json_t* data, unsigned int row,
                            const char* column);

/*************************************************************************/

/* Migrations: the one thing in this API that runs SQL the caller supplied
 * as a whole script, because DDL cannot be prepared.  That SQL must come
 * from the module's own source -- never from a user, never from a
 * parameter.  The script and the row in the `migrations' table that says
 * it ran are one transaction: a script that fails leaves neither. */

struct DbMigration {
    const char* dbm_module;   /* Module it belongs to, or "core" */
    unsigned int dbm_version; /* Version being applied or reverted */
    const char* dbm_name;     /* Migration name, for the record */
    const char* dbm_sql;      /* The whole script */
    const char* dbm_exec_by;  /* Who asked (an oper's nick), for the record */
    int dbm_revert;           /* Nonzero to revert instead of apply */
};

/* Run one migration and record it, atomically, on a connection of its own,
 * with the migration timeout.  On success the result's data holds the
 * recorded row.  Returns DB_OK when the migration was accepted. */
extern enum DbError db_migrate(struct Module_* mod,
                               const struct DbMigration* migration,
                               DbResultFn cb, void* user);

/* The same, synchronously, on the main thread's own connection, with the
 * migration timeout.  Used at start-up (see migration.h).  Release `res'
 * with db_result_free(). */
extern enum DbError db_migrate_sync(const struct DbMigration* migration,
                                    struct DbResult* res);

/*************************************************************************/

/* Instrumentation. */
extern unsigned int db_calls_pending(void);
extern unsigned int db_calls_total(void);
extern unsigned int db_calls_failed(void);

/* Write a line per connection pool to the log. */
extern void db_report(void);

/*************************************************************************/

/* The `database' block.  The strings belong to the configuration and stay
 * valid until the next REHASH; the driver takes copies of what it keeps,
 * and watches dbconf_generation to notice a change. */

struct DatabaseConf {
    char* dbconf_dsn;                    /* Connection string (required) */
    char* dbconf_role_dsn[DB_ROLE_LAST]; /* Per-role override, or NULL */
    int dbconf_role_pool[DB_ROLE_LAST];  /* Connections per role */
    int dbconf_timeout_ms;               /* Query timeout, clamped */
    int dbconf_migration_ms;             /* Migration timeout, clamped */
    int dbconf_save_ms;                  /* Table save timeout, clamped */
    int dbconf_sync_ms;                  /* Sync query timeout, clamped */
    char* dbconf_schema;                 /* Schema for every table */
    unsigned int dbconf_generation;      /* Bumped on every change */
};

/* The configuration, or NULL if there is none. */
extern const struct DatabaseConf* db_conf(void);
/* Connection string for `role': its own, else the common one. */
extern const char* db_conf_dsn(enum DbRole role);
extern int db_conf_pool(enum DbRole role);
extern int db_conf_timeout(void);
extern int db_conf_migration_timeout(void);
extern int db_conf_save_timeout(void);
extern int db_conf_sync_timeout(void);
extern const char* db_conf_schema(void);
extern unsigned int db_conf_generation(void);

/*************************************************************************/

/* The driver side.  The core's PostgreSQL driver implements these; nothing
 * else calls them. */

struct DbDriver {
    const char* dbdrv_name;
    /* Start one query: copy what is needed, queue it, and answer later
     * with db_complete(id, ...).  A refusal (anything but DB_OK) means
     * db_complete() must not be called for `id'. */
    enum DbError (*dbdrv_submit)(unsigned long id, const struct DbQuery* query,
                                 enum DbRole role);
    /* Release a json_t the driver produced (possibly NULL). */
    void (*dbdrv_release)(struct json_t* data);
    /* Start one migration (optional). */
    enum DbError (*dbdrv_migrate)(unsigned long id,
                                  const struct DbMigration* migration);
    /* Readers for db_rows() and friends (optional). */
    unsigned int (*dbdrv_rows)(struct json_t* data);
    const char* (*dbdrv_row_str)(struct json_t* data, unsigned int row,
                                 const char* column);
    long long (*dbdrv_row_int)(struct json_t* data, unsigned int row,
                               const char* column);
    /* Synchronous variants (optional): fill `*data' and `*rows' as
     * db_complete() would receive them, and return the outcome. */
    enum DbError (*dbdrv_query_sync)(const struct DbQuery* query,
                                     struct json_t** data,
                                     unsigned int* rows);
    enum DbError (*dbdrv_migrate_sync)(const struct DbMigration* migration,
                                       struct json_t** data);
    /* Notice a changed configuration (optional); write a status report
     * (optional); stop everything (optional). */
    void (*dbdrv_reconfigure)(void);
    void (*dbdrv_report)(void);
    void (*dbdrv_shutdown)(void);
};

/* Register `driver'; there is one at a time.  Returns nonzero on success. */
extern int db_register_driver(const struct DbDriver* driver);

/* Withdraw the driver, failing every query still in flight. */
extern void db_unregister_driver(void);

/* Deliver the answer to query `id'.  Main thread only; exactly once for
 * every submit accepted.  `data' is released through dbdrv_release whether
 * or not the callback runs.  `message' may be NULL for db_strerror(). */
extern void db_complete(unsigned long id, struct json_t* data,
                        unsigned int rows, enum DbError code,
                        const char* message);

/*************************************************************************/

/* Core-side interface.  Not for modules. */

/* Publish what the `database' block read (its directive table, which
 * init.c binds with the other core blocks, is db_directives[] in db.c).
 * Called after CONFIGURE_SET. */
extern void db_config_apply(void);
/* Drop a module's pending calls; the module loader calls this. */
extern void db_drop_module(struct Module_* mod);
/* Release everything, at exit. */
extern void db_shutdown(void);

/*************************************************************************/

#endif /* DB_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
