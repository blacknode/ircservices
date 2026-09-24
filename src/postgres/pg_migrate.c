/* Running a migration: its own connection, its own transaction.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/pg_migrate.c).
 *
 * A migration is not a query, and almost nothing about the rest of this
 * driver fits it.  Three differences decide the shape of this file.
 *
 * IT IS DDL, SO IT IS NOT PREPARED.  PREPARE accepts SELECT, INSERT,
 * UPDATE, DELETE, VALUES and MERGE; it does not accept CREATE TABLE, ALTER
 * TABLE or CREATE INDEX, which is what a migration is made of -- and a
 * migration is a script of several statements besides.  So the script goes
 * out through the simple protocol.  That is safe here for a reason that
 * does not generalise: the string is SQL the module author shipped in the
 * module itself.  It is not a query anybody assembled, it takes no
 * parameters, and nothing a user ever typed can reach it.  The row that
 * records the migration carries an operator's nick, and that one is bound
 * like everything else.
 *
 * IT IS ALL-OR-NOTHING.  The script and the row saying it ran are one
 * explicit transaction.  A migration that fails half way leaves neither
 * the change nor the record; a recorded migration is one that actually
 * happened.
 *
 * IT TAKES AS LONG AS IT TAKES.  A pooled connection has a five second
 * deadline and shares its thread with every query behind it; an ALTER
 * TABLE on a real table takes minutes.  So a migration gets a connection
 * of its own, opened for it and closed after it, on a pool thread rather
 * than a connection thread, with the migration timeout.
 *
 * Everything from pg_migrate_work() to the end of that section runs in a
 * worker thread and obeys worker.h.
 */

#include "postgres.h"

#include <stdio.h>
#include <string.h>

/* What the main thread hands the worker.  Worker memory, copied. */
struct PgMigrateJob {
    unsigned long pgm_id;     /* Handle for db_complete() */
    int pgm_timeout_ms;       /* Deadline for the whole thing */
    int pgm_revert;           /* Nonzero to revert */
    unsigned int pgm_version; /* Version being applied or reverted */
    char* pgm_dsn;            /* Where to connect */
    char* pgm_search_path;    /* The configured schema, then public */
    char* pgm_module;         /* Module the migration belongs to */
    char* pgm_name;           /* Migration name */
    char* pgm_sql;            /* The script */
    char* pgm_exec_by;        /* Who asked */
};

/* What the worker hands back. */
struct PgMigrateResult {
    json_t* pgm_rows;      /* The recorded row, or NULL */
    enum DbError pgm_code; /* How it ended */
};

/*************************************************************************/
/*************************** The worker thread ***************************/
/*************************************************************************/

/* Render `ms' as something an operator can read: "840ms", "3.2s",
 * "4m 11s". */
static void pg_migrate_duration(long ms, char* out, size_t size)
{
    if (ms < 1000)
        snprintf(out, size, "%ldms", ms);
    else if (ms < 60000)
        snprintf(out, size, "%ld.%lds", ms / 1000, (ms % 1000) / 100);
    else
        snprintf(out, size, "%ldm %lds", ms / 60000, (ms % 60000) / 1000);
}

/* Write or delete the row that records the migration, bound like any other
 * write; RETURNING hands back the row that was recorded. */
static int pg_migrate_record(PGconn* pg, const struct DbMigration* job,
                             const char* duration,
                             const struct PgDeadline* deadline, json_t** rows,
                             enum DbError* code)
{
    const char* values[5];
    char version[16];
    PGresult* res;
    int ok;

    snprintf(version, sizeof(version), "%u", job->dbm_version);
    if (job->dbm_revert) {
        /* Reverting removes the record: what is in the table is what is
         * applied, and a reverted migration is not. */
        values[0] = job->dbm_module;
        values[1] = version;
        ok = pg_run_params(pg, deadline,
                           "delete from migrations where module_name = $1"
                           " and version = $2::integer returning version,"
                           " module_name, module_migration_name,"
                           " exec_duration, exec_by, created_at",
                           2, values, "recording the migration", &res, code);
    }
    else {
        values[0] = version;
        values[1] = job->dbm_module;
        values[2] = job->dbm_name;
        values[3] = duration;
        values[4] = job->dbm_exec_by;
        ok = pg_run_params(pg, deadline,
                           "insert into migrations (version, module_name,"
                           " module_migration_name, exec_duration, exec_by)"
                           " values ($1::integer, $2, $3, $4, $5) returning"
                           " version, module_name, module_migration_name,"
                           " exec_duration, exec_by, created_at",
                           5, values, "recording the migration", &res, code);
    }
    if (!ok)
        return 0;
    *rows = pg_json_rows(res);
    PQclear(res);
    if (!*rows) {
        *code = DB_ERR_RESOURCE;
        return 0;
    }
    return 1;
}

enum DbError pg_migrate_exec(PGconn* pg, const struct PgDeadline* deadline,
                             const struct DbMigration* migration,
                             json_t** rows_ret)
{
    enum DbError code = DB_ERR_INTERNAL, ignored;
    json_t* rows = NULL;
    char duration[32];
    long elapsed;

    do {
        if (!pg_run(pg, deadline, "begin", 1, "beginning a migration", NULL,
                    &code))
            break;
        /* The clock the operator is told about covers the script and
         * nothing else. */
        elapsed = pg_monotonic_ms();
        if (!pg_run(pg, deadline, migration->dbm_sql, 1,
                    migration->dbm_revert ? "reverting a migration"
                                          : "applying a migration",
                    NULL, &code))
            break;
        elapsed = pg_monotonic_ms() - elapsed;
        pg_migrate_duration(elapsed, duration, sizeof(duration));
        if (!pg_migrate_record(pg, migration, duration, deadline, &rows,
                               &code))
            break;
        if (!pg_run(pg, deadline, "commit", 1, "committing a migration", NULL,
                    &code))
            break;
        code = DB_OK;
    } while (0);

    if (code != DB_OK) {
        /* Best effort: the connection may already be unusable, in which
         * case closing it rolls back just as well. */
        if (PQtransactionStatus(pg) != PQTRANS_IDLE)
            pg_run(pg, deadline, "rollback", 1, "rolling back a migration",
                   NULL, &ignored);
        if (rows) {
            json_decref(rows);
            rows = NULL;
        }
    }
    *rows_ret = rows;
    return code;
}

/* Run one migration, on a connection of its own.  Worker thread. */
static void pg_migrate_work(struct WorkTask* task)
{
    struct PgMigrateJob* job = task->wt_in;
    struct PgMigrateResult* out;
    struct PgDeadline deadline;
    struct DbMigration migration;
    enum DbError code = DB_ERR_INTERNAL;
    PGconn* pg;

    if (!(out = worker_alloc(sizeof(*out)))) {
        task->wt_status = -1;
        return;
    }
    task->wt_out = out;
    task->wt_out_len = sizeof(*out);

    pg_deadline_set(&deadline, job->pgm_timeout_ms);
    if (!(pg = pg_open(job->pgm_dsn, &deadline, &code))) {
        out->pgm_code = code;
        task->wt_status = -1;
        return;
    }
    /* The server gets the same deadline, so that a statement blocked on a
     * lock ends on its own even if the cancel never lands. */
    pg_session_setup(pg, &deadline, job->pgm_timeout_ms, job->pgm_search_path);

    memset(&migration, 0, sizeof(migration));
    migration.dbm_module = job->pgm_module;
    migration.dbm_version = job->pgm_version;
    migration.dbm_name = job->pgm_name;
    migration.dbm_sql = job->pgm_sql;
    migration.dbm_exec_by = job->pgm_exec_by;
    migration.dbm_revert = job->pgm_revert;
    out->pgm_code = pg_migrate_exec(pg, &deadline, &migration, &out->pgm_rows);
    PQfinish(pg);
    task->wt_status = out->pgm_code == DB_OK ? 0 : -1;
}

/*************************************************************************/
/************************** Back in the main thread **********************/
/*************************************************************************/

/* Release a migration task's payload.  Main thread; every path. */
static void pg_migrate_free(struct WorkTask* task)
{
    struct PgMigrateJob* job = task->wt_in;
    struct PgMigrateResult* out = task->wt_out;

    if (job) {
        worker_free(job->pgm_dsn);
        worker_free(job->pgm_search_path);
        worker_free(job->pgm_module);
        worker_free(job->pgm_name);
        worker_free(job->pgm_sql);
        worker_free(job->pgm_exec_by);
        worker_free(job);
        task->wt_in = NULL;
    }
    if (out) {
        /* Non-NULL only when the answer never reached db_complete(). */
        if (out->pgm_rows)
            json_decref(out->pgm_rows);
        worker_free(out);
        task->wt_out = NULL;
    }
}

/* Deliver the answer.  Main thread. */
static void pg_migrate_done(struct WorkTask* task)
{
    struct PgMigrateJob* job = task->wt_in;
    struct PgMigrateResult* out = task->wt_out;
    json_t* rows;

    if (!out) {
        db_complete(job->pgm_id, NULL, 0, DB_ERR_RESOURCE, NULL);
        return;
    }
    rows = out->pgm_rows;
    out->pgm_rows = NULL;
    db_complete(job->pgm_id, (struct json_t*)rows,
                rows ? pg_json_count(rows) : 0, out->pgm_code,
                pg_error_message(out->pgm_code));
}

/* Copy a string into worker memory ("" for NULL). */
static char* pg_migrate_dup(const char* text)
{
    size_t len;
    char* copy;

    if (!text)
        text = "";
    len = strlen(text);
    if ((copy = worker_alloc(len + 1)) != NULL)
        memcpy(copy, text, len + 1);
    return copy;
}

enum DbError pg_migrate_submit(unsigned long id,
                               const struct DbMigration* migration)
{
    struct PgMigrateJob* job;
    struct WorkTask* task;
    char search_path[300];
    const char* dsn;

    /* A migration writes, so it goes where writes go. */
    if (!(dsn = db_conf_dsn(DB_ROLE_WRITE)) || !*dsn)
        return DB_ERR_CONFIG;
    if (!(task = worker_task_new(pg_migrate_work, pg_migrate_done)))
        return DB_ERR_RESOURCE;
    task->wt_free = pg_migrate_free;
    if (!(job = worker_alloc(sizeof(*job)))) {
        worker_task_free(task);
        return DB_ERR_RESOURCE;
    }
    task->wt_in = job;
    task->wt_in_len = sizeof(*job);

    pg_search_path(db_conf_schema(), search_path, sizeof(search_path));
    job->pgm_id = id;
    job->pgm_timeout_ms = db_conf_migration_timeout();
    job->pgm_revert = migration->dbm_revert;
    job->pgm_version = migration->dbm_version;
    job->pgm_dsn = pg_migrate_dup(dsn);
    job->pgm_search_path = pg_migrate_dup(search_path);
    job->pgm_module = pg_migrate_dup(migration->dbm_module);
    job->pgm_name = pg_migrate_dup(migration->dbm_name);
    job->pgm_sql = pg_migrate_dup(migration->dbm_sql);
    job->pgm_exec_by = pg_migrate_dup(migration->dbm_exec_by);
    if (!job->pgm_dsn || !job->pgm_search_path || !job->pgm_module ||
        !job->pgm_name || !job->pgm_sql || !job->pgm_exec_by) {
        worker_task_free(task);
        return DB_ERR_RESOURCE;
    }

    /* The general worker pool, not a pool connection thread: a migration
     * can take minutes, and no query should queue behind it. */
    if (!worker_submit(task)) {
        worker_task_free(task);
        return DB_ERR_UNAVAILABLE;
    }
    return DB_OK;
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
