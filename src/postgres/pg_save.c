/* The table store's save job, in a worker thread.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * A save rewrites every table that changed since the last one, in one
 * transaction, on a connection of its own:
 *
 *     BEGIN
 *     SELECT token FROM instance WHERE id = 1 FOR UPDATE
 *     -- for each table:
 *     [CREATE TABLE IF NOT EXISTS ... / ALTER TABLE ... ADD COLUMN ...]
 *     TRUNCATE TABLE <table>
 *     COPY <table> (<columns>) FROM STDIN   -- the rows, as prepared
 *     COMMIT
 *
 * Either every table is written or none is: a save that fails half way
 * leaves the previous save in place, whole.  Readers of the tables (a web
 * panel, a report) see the old contents until the COMMIT and the new ones
 * after it, never a mixture.
 *
 * The instance check is what keeps two copies of Services from taking
 * turns overwriting each other's data: the instance row names the copy
 * that owns the schema (see pg_store.c), and a save from any other copy
 * is rolled back and reported.
 *
 * The rows were snapshotted in the main thread, so the data being written
 * is consistent across tables however long the save takes, and the main
 * loop does not wait for any of it.
 *
 * Everything here obeys worker.h: no services.h, the system allocator,
 * worker_log() for logging.
 */

#include "pg_save.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* How much of the COPY data is handed to libpq at a time. */
#define PG_COPY_CHUNK 65536

/*************************************************************************/
/**************************** Growable buffers ***************************/
/*************************************************************************/

void pgbuf_init(struct PgBuf* buf)
{
    memset(buf, 0, sizeof(*buf));
}

void pgbuf_free(struct PgBuf* buf)
{
    free(buf->pgb_data);
    pgbuf_init(buf);
}

/* Make room for `more' bytes plus a NUL. */
static int pgbuf_reserve(struct PgBuf* buf, size_t more)
{
    size_t want;
    char* data;

    if (buf->pgb_failed)
        return 0;
    if (buf->pgb_len + more + 1 <= buf->pgb_size)
        return 1;
    want = buf->pgb_size ? buf->pgb_size : 4096;
    while (want < buf->pgb_len + more + 1)
        want *= 2;
    if (!(data = realloc(buf->pgb_data, want))) {
        buf->pgb_failed = 1;
        return 0;
    }
    buf->pgb_data = data;
    buf->pgb_size = want;
    return 1;
}

void pgbuf_append(struct PgBuf* buf, const void* data, size_t len)
{
    if (!pgbuf_reserve(buf, len))
        return;
    memcpy(buf->pgb_data + buf->pgb_len, data, len);
    buf->pgb_len += len;
    buf->pgb_data[buf->pgb_len] = '\0';
}

void pgbuf_puts(struct PgBuf* buf, const char* str)
{
    pgbuf_append(buf, str, strlen(str));
}

void pgbuf_printf(struct PgBuf* buf, const char* fmt, ...)
{
    va_list args;
    int n;

    va_start(args, fmt);
    n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (n < 0 || !pgbuf_reserve(buf, (size_t)n))
        return;
    va_start(args, fmt);
    vsnprintf(buf->pgb_data + buf->pgb_len, (size_t)n + 1, fmt, args);
    va_end(args);
    buf->pgb_len += (size_t)n;
}

char* pgbuf_steal(struct PgBuf* buf)
{
    char* data;

    if (buf->pgb_failed || !pgbuf_reserve(buf, 0)) {
        pgbuf_free(buf);
        return NULL;
    }
    data = buf->pgb_data;
    pgbuf_init(buf);
    return data;
}

char* pg_save_strdup(const char* str)
{
    return str ? strdup(str) : NULL;
}

void pg_save_free(void* ptr)
{
    free(ptr);
}

/*************************************************************************/
/********************************** Jobs *********************************/
/*************************************************************************/

struct PgSaveJob* pg_save_job_new(int ntables)
{
    struct PgSaveJob* job = calloc(1, sizeof(*job));
    int i;

    if (!job)
        return NULL;
    if (ntables > 0 && !(job->psj_tables = calloc((size_t)ntables,
                                                  sizeof(*job->psj_tables)))) {
        free(job);
        return NULL;
    }
    job->psj_ntables = ntables;
    for (i = 0; i < ntables; i++)
        pgbuf_init(&job->psj_tables[i].pst_data);
    return job;
}

void pg_save_job_free(struct PgSaveJob* job)
{
    int i;

    if (!job)
        return;
    for (i = 0; i < job->psj_ntables; i++) {
        struct PgSaveTable* t = &job->psj_tables[i];
        free(t->pst_name);
        free(t->pst_ddl);
        free(t->pst_truncate);
        free(t->pst_copy);
        pgbuf_free(&t->pst_data);
    }
    free(job->psj_tables);
    free(job->psj_dsn);
    free(job->psj_search_path);
    free(job->psj_check);
    free(job->psj_token);
    free(job);
}

/*************************************************************************/
/******************************** The save *******************************/
/*************************************************************************/

/* Push `len' bytes of COPY data, waiting for the socket as needed.
 * Returns nonzero on success. */
static int pg_copy_put(PGconn* pg, const char* data, int len,
                       const struct PgDeadline* deadline, enum DbError* code)
{
    int res;

    for (;;) {
        res = len ? PQputCopyData(pg, data, len) : PQputCopyEnd(pg, NULL);
        if (res == 1)
            return 1;
        if (res < 0) {
            pg_error_log("writing a table", PQerrorMessage(pg));
            *code = DB_ERR_CONNECT;
            return 0;
        }
        /* The output buffer is full: drain it and try again. */
        while ((res = PQflush(pg)) > 0) {
            int ready = pg_socket_wait(PQsocket(pg), 1, -1, deadline);
            if (ready <= 0) {
                *code = ready == 0 ? DB_ERR_TIMEOUT : DB_ERR_CONNECT;
                return 0;
            }
        }
        if (res < 0) {
            pg_error_log("writing a table", PQerrorMessage(pg));
            *code = DB_ERR_CONNECT;
            return 0;
        }
    }
}

/* Write one table: DDL if needed, TRUNCATE, COPY. */
static int pg_save_table(PGconn* pg, const struct PgSaveTable* t,
                         const struct PgDeadline* deadline, enum DbError* code)
{
    const char* data = t->pst_data.pgb_data;
    size_t left = t->pst_data.pgb_len;
    PGresult* res;
    int collected;

    if (t->pst_ddl &&
        !pg_run(pg, deadline, t->pst_ddl, 1, "creating a table", NULL, code))
        return 0;
    if (!pg_run(pg, deadline, t->pst_truncate, 1, "emptying a table", NULL,
                code))
        return 0;
    if (!pg_run(pg, deadline, t->pst_copy, 1, "writing a table", &res, code))
        return 0;
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        PQclear(res);
        *code = DB_ERR_INTERNAL;
        return 0;
    }
    PQclear(res);

    while (left > 0) {
        int n = left > PG_COPY_CHUNK ? PG_COPY_CHUNK : (int)left;
        if (!pg_copy_put(pg, data, n, deadline, code))
            return 0;
        data += n;
        left -= (size_t)n;
    }
    if (!pg_copy_put(pg, NULL, 0, deadline, code))
        return 0;

    collected = pg_collect(pg, -1, deadline, &res);
    if (collected != 0 || !res) {
        PQclear(res);
        *code = collected == -1 ? DB_ERR_TIMEOUT : DB_ERR_CONNECT;
        return 0;
    }
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        *code =
            pg_error_from_sqlstate(PQresultErrorField(res, PG_DIAG_SQLSTATE));
        pg_error_log("writing a table", PQresultErrorMessage(res));
        PQclear(res);
        return 0;
    }
    PQclear(res);
    return 1;
}

/* Check that this copy of Services still owns the schema.  Takes the
 * instance row's lock for the rest of the transaction, so that a copy
 * taking over waits for this save to end rather than interleaving. */
static int pg_save_check(PGconn* pg, struct PgSaveJob* job,
                         const struct PgDeadline* deadline, enum DbError* code)
{
    PGresult* res;
    int ok;

    if (!job->psj_token)
        return 1;
    if (!pg_run(pg, deadline, job->psj_check, 0, "checking the instance", &res,
                code))
        return 0;
    ok = PQntuples(res) == 1 && !PQgetisnull(res, 0, 0) &&
         strcmp(PQgetvalue(res, 0, 0), job->psj_token) == 0;
    PQclear(res);
    if (!ok) {
        job->psj_stolen = 1;
        *code = DB_ERR_PERMISSION;
    }
    return ok;
}

void pg_save_run(struct PgSaveJob* job)
{
    struct PgDeadline deadline;
    enum DbError code = DB_ERR_INTERNAL, ignored;
    long start = pg_monotonic_ms();
    PGconn* pg;
    int i;

    pg_deadline_set(&deadline, job->psj_timeout_ms);
    if (!(pg = pg_open(job->psj_dsn, &deadline, &code))) {
        job->psj_code = code;
        job->psj_elapsed_ms = pg_monotonic_ms() - start;
        return;
    }
    pg_session_setup(pg, &deadline, job->psj_timeout_ms, job->psj_search_path);

    do {
        if (!pg_run(pg, &deadline, "begin", 1, "beginning a save", NULL,
                    &code))
            break;
        if (!pg_save_check(pg, job, &deadline, &code))
            break;
        for (i = 0; i < job->psj_ntables; i++) {
            if (!pg_save_table(pg, &job->psj_tables[i], &deadline, &code)) {
                snprintf(job->psj_failed, sizeof(job->psj_failed), "%s",
                         job->psj_tables[i].pst_name);
                break;
            }
        }
        if (i < job->psj_ntables)
            break;
        if (!pg_run(pg, &deadline, "commit", 1, "committing a save", NULL,
                    &code))
            break;
        code = DB_OK;
    } while (0);

    if (code != DB_OK) {
        /* Best effort: closing the connection rolls back just as well. */
        if (PQtransactionStatus(pg) != PQTRANS_ACTIVE)
            pg_run(pg, &deadline, "rollback", 1, "rolling back a save", NULL,
                   &ignored);
    }
    PQfinish(pg);
    job->psj_code = code;
    job->psj_elapsed_ms = pg_monotonic_ms() - start;
}

/*************************************************************************/
/*************************** Through the pool ****************************/
/*************************************************************************/

/* Who to tell when the save is over. */
struct PgSaveNotify {
    PgSaveDoneFn fn;
    void* arg;
};

static void pg_save_work(struct WorkTask* task)
{
    pg_save_run(task->wt_in);
}

static void pg_save_done(struct WorkTask* task)
{
    struct PgSaveNotify* notify = task->wt_arg;

    if (notify && notify->fn)
        (*notify->fn)(task->wt_in, notify->arg);
}

static void pg_save_task_free(struct WorkTask* task)
{
    pg_save_job_free(task->wt_in);
    free(task->wt_arg);
    task->wt_in = NULL;
    task->wt_arg = NULL;
}

int pg_save_submit(struct PgSaveJob* job, PgSaveDoneFn done, void* arg)
{
    struct PgSaveNotify* notify;
    struct WorkTask* task;

    if (!(notify = calloc(1, sizeof(*notify))))
        return 0;
    if (!(task = worker_task_new(pg_save_work, pg_save_done))) {
        free(notify);
        return 0;
    }
    notify->fn = done;
    notify->arg = arg;
    task->wt_in = job;
    task->wt_arg = notify;
    task->wt_free = pg_save_task_free;
    if (!worker_submit(task)) {
        /* Still the caller's: detach it before the task goes. */
        task->wt_in = NULL;
        worker_task_free(task);
        return 0;
    }
    return 1;
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
