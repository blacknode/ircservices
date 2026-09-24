/* One connection, one thread, and a deadline it cannot exceed.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/pg_conn.c).
 *
 * Everything in this file after pg_conn_new() runs in a connection thread
 * (or, for pg_open() and pg_run(), in whichever thread owns the connection)
 * and obeys worker.h to the letter: the only memory it allocates comes from
 * worker_alloc() or from jansson, the only logging it does is worker_log(),
 * and the only thing it shares with the main thread is the pool's queue.
 *
 * PREPARED STATEMENTS, ALWAYS
 *
 * A statement is never assembled from a value.  It travels with $1, $2
 * placeholders, it is prepared once per connection and cached, and the
 * values travel beside it in their own array.  That is what makes injection
 * a non-question rather than a review item.  The one exception is SQL that
 * Services builds itself from names it chose (the table store's DDL and
 * COPY commands), which carries no value at all.
 *
 * THE DEADLINE
 *
 * db.h promises that no query outlives DB_TIMEOUT_MAX_MS, and a promise
 * that a stalled network can break is not one.  So the round trip is driven
 * through libpq's non-blocking entry points (PQsendPrepare and
 * PQsendQueryPrepared rather than PQprepare and PQexecPrepared) and every
 * wait is a poll() against the deadline.  When the deadline passes, the
 * query is cancelled, the connection is drained, and the caller is told
 * DB_ERR_TIMEOUT.  The server is told the same deadline at connect time,
 * as statement_timeout, so a query that survives a lost cancel packet still
 * ends on its own.
 */

#include "postgres.h"

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A statement this connection has prepared. */
struct PgStmt {
    struct PgStmt* pgs_hnext; /* Next in its hash bucket */
    unsigned int pgs_hash;    /* Hash of pgs_sql */
    unsigned int pgs_nparams; /* Parameters it was prepared with */
    Oid* pgs_types;           /* Their types, or NULL for none */
    char* pgs_sql;            /* The statement text */
    char pgs_name[32];        /* Name it is prepared under */
};

/* One pooled connection. */
struct PgConn {
    struct PgPool* pgc_pool; /* Pool this belongs to */
    PGconn* pgc_pg;          /* libpq connection, or NULL */
    time_t pgc_retry_at;     /* Monotonic second to retry connecting */
    unsigned long pgc_seq;   /* Counter behind PgStmt.pgs_name */
    unsigned int pgc_nstmts; /* Statements in the cache */
    struct PgStmt* pgc_buckets[PG_STMT_BUCKETS];
    char pgc_name[WORKER_NAMELEN + 1]; /* Name of its thread */
};

/* The longest deadline anything in this driver may arm: a migration. */
#define PG_DEADLINE_MAX_MS DB_MIGRATION_TIMEOUT_MAX

/*************************************************************************/
/******************************* Deadlines *******************************/
/*************************************************************************/

static void pg_now(struct timespec* now)
{
    if (clock_gettime(CLOCK_MONOTONIC, now) != 0) {
        now->tv_sec = 0;
        now->tv_nsec = 0;
    }
}

long pg_monotonic_ms(void)
{
    struct timespec now;

    pg_now(&now);
    return (long)now.tv_sec * 1000L + now.tv_nsec / 1000000L;
}

void pg_deadline_set(struct PgDeadline* deadline, int ms)
{
    if (ms < 0)
        ms = 0;
    if (ms > PG_DEADLINE_MAX_MS)
        ms = PG_DEADLINE_MAX_MS; /* the ceiling, one last time */

    pg_now(&deadline->pgd_at);
    deadline->pgd_at.tv_sec += ms / 1000;
    deadline->pgd_at.tv_nsec += (long)(ms % 1000) * 1000000L;
    if (deadline->pgd_at.tv_nsec >= 1000000000L) {
        deadline->pgd_at.tv_sec++;
        deadline->pgd_at.tv_nsec -= 1000000000L;
    }
}

int pg_deadline_left(const struct PgDeadline* deadline)
{
    struct timespec now;
    long ms;

    pg_now(&now);
    ms = (long)(deadline->pgd_at.tv_sec - now.tv_sec) * 1000L +
         (deadline->pgd_at.tv_nsec - now.tv_nsec) / 1000000L;
    if (ms < 0)
        return 0;
    if (ms > PG_DEADLINE_MAX_MS)
        return PG_DEADLINE_MAX_MS;
    return (int)ms;
}

int pg_socket_wait(int fd, int forwrite, int stopfd,
                   const struct PgDeadline* deadline)
{
    struct pollfd fds[2];
    nfds_t nfds = 1;
    int left, n;

    if (fd < 0)
        return -1;
    fds[0].fd = fd;
    fds[0].events = (short)(forwrite ? POLLOUT : POLLIN);
    fds[0].revents = 0;
    if (stopfd >= 0) {
        fds[1].fd = stopfd;
        fds[1].events = POLLIN;
        fds[1].revents = 0;
        nfds = 2;
    }

    for (;;) {
        if ((left = pg_deadline_left(deadline)) <= 0)
            return 0;
        n = poll(fds, nfds, left);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (n == 0)
            return 0;
        /* A stop outranks a ready socket: Services are waiting for this
         * thread to return, and the query is going to be failed anyway. */
        if (nfds == 2 && fds[1].revents)
            return -1;
        if (fds[0].revents)
            return 1;
    }
}

/*************************************************************************/
/********************** The prepared statement cache *********************/
/*************************************************************************/

static unsigned int pg_stmt_hash(const char* sql)
{
    unsigned int hash = 2166136261u; /* FNV-1a */

    while (*sql) {
        hash ^= (unsigned char)*sql++;
        hash *= 16777619u;
    }
    return hash;
}

static void pg_stmt_free(struct PgStmt* stmt)
{
    worker_free(stmt->pgs_sql);
    worker_free(stmt->pgs_types);
    worker_free(stmt);
}

/* Empty the cache, without telling the server.  Only correct when the
 * connection is going away with it, which is the only time it is called:
 * closing the connection discards every statement prepared on it. */
static void pg_stmt_clear(struct PgConn* conn)
{
    struct PgStmt *stmt, *next;
    unsigned int bucket;

    for (bucket = 0; bucket < PG_STMT_BUCKETS; bucket++) {
        for (stmt = conn->pgc_buckets[bucket]; stmt; stmt = next) {
            next = stmt->pgs_hnext;
            pg_stmt_free(stmt);
        }
        conn->pgc_buckets[bucket] = NULL;
    }
    conn->pgc_nstmts = 0;
}

/* Find the statement prepared for `req', if there is one.  The types are
 * part of the key, not just the text: a statement prepared with its
 * parameters declared as text is a different statement from the same text
 * prepared with them declared as integers. */
static struct PgStmt* pg_stmt_find(struct PgConn* conn,
                                   const struct PgRequest* req,
                                   unsigned int hash)
{
    struct PgStmt* stmt;
    unsigned int n;

    for (stmt = conn->pgc_buckets[hash % PG_STMT_BUCKETS]; stmt;
         stmt = stmt->pgs_hnext) {
        if (stmt->pgs_hash != hash || stmt->pgs_nparams != req->pgr_nparams)
            continue;
        if (strcmp(stmt->pgs_sql, req->pgr_sql) != 0)
            continue;
        for (n = 0; n < req->pgr_nparams; n++) {
            if (stmt->pgs_types[n] != req->pgr_params[n].pgb_type)
                break;
        }
        if (n == req->pgr_nparams)
            return stmt;
    }
    return NULL;
}

/*************************************************************************/
/************************** Talking to the server ************************/
/*************************************************************************/

int pg_collect(PGconn* pg, int stopfd, const struct PgDeadline* deadline,
               PGresult** out)
{
    PGresult *first = NULL, *res;
    int flushed;

    *out = NULL;

    /* Push the request out first; a full send buffer is a wait like any
     * other, and it is on the same deadline. */
    while ((flushed = PQflush(pg)) > 0) {
        int ready = pg_socket_wait(PQsocket(pg), 1, stopfd, deadline);
        if (ready <= 0)
            return ready == 0 ? -1 : -2;
    }
    if (flushed < 0)
        return -2;

    for (;;) {
        while (PQisBusy(pg)) {
            int ready = pg_socket_wait(PQsocket(pg), 0, stopfd, deadline);
            if (ready <= 0) {
                /* Whatever has arrived so far is of no use to anybody now,
                 * and the caller is about to cancel or close. */
                PQclear(first);
                return ready == 0 ? -1 : -2;
            }
            if (!PQconsumeInput(pg)) {
                pg_error_log("reading a result", PQerrorMessage(pg));
                PQclear(first);
                return -2;
            }
        }
        if (!(res = PQgetResult(pg)))
            break;
        /* A COPY in either direction hands back a result of its own in the
         * middle of the exchange; the caller deals with it. */
        if (PQresultStatus(res) == PGRES_COPY_IN ||
            PQresultStatus(res) == PGRES_COPY_OUT) {
            PQclear(first);
            *out = res;
            return 0;
        }
        /* The first result is the answer; with the simple protocol, a
         * later error outranks an earlier success, because it is what
         * made the script stop. */
        if (!first) {
            first = res;
        }
        else if (PQresultStatus(res) == PGRES_FATAL_ERROR &&
                 PQresultStatus(first) != PGRES_FATAL_ERROR) {
            PQclear(first);
            first = res;
        }
        else {
            PQclear(res);
        }
    }
    *out = first;
    return 0;
}

/* Cancel whatever is running and get the connection back to idle.
 * Returns nonzero when the connection is usable again. */
static int pg_cancel(PGconn* pg, int stopfd)
{
    struct PgDeadline grace;
    PGcancel* cancel;
    PGresult* res;
    char errbuf[256];

    if ((cancel = PQgetCancel(pg)) != NULL) {
        if (!PQcancel(cancel, errbuf, sizeof(errbuf)))
            pg_error_log("cancelling a query", errbuf);
        PQfreeCancel(cancel);
    }
    /* The cancel is a request, not a guarantee: the server still owes a
     * result.  Wait a moment for it, then give up on the connection rather
     * than on the pool. */
    pg_deadline_set(&grace, PG_CANCEL_GRACE_MS);
    if (pg_collect(pg, stopfd, &grace, &res) != 0)
        return 0;
    PQclear(res);
    return PQstatus(pg) == CONNECTION_OK;
}

/* Start connecting to `dsn'.  The connection string is passed as dbname
 * with expand_dbname set, so that it may be anything libpq accepts
 * ("host=... dbname=..." or a postgresql:// URI), and the keywords after
 * it override it: the client encoding is always UTF-8, whatever the
 * connection string says, because that is what the table store writes. */
static PGconn* pg_connect_start(const char* dsn)
{
    static const char* const keywords[] = {"dbname", "client_encoding",
                                           "fallback_application_name", NULL};
    const char* values[4];

    values[0] = dsn;
    values[1] = "UTF8";
    values[2] = "ircservices";
    values[3] = NULL;
    return PQconnectStartParams(keywords, values, 1);
}

/* Drive a connection started with pg_connect_start() to completion.
 * Returns DB_OK, or the reason it failed (the connection is then
 * finished). */
static enum DbError pg_connect_finish(PGconn* pg, int stopfd,
                                      const struct PgDeadline* deadline,
                                      const char* what)
{
    PostgresPollingStatusType polling = PGRES_POLLING_WRITING;

    if (PQstatus(pg) == CONNECTION_BAD) {
        pg_error_log(what, PQerrorMessage(pg));
        return DB_ERR_CONNECT;
    }
    for (;;) {
        int ready = pg_socket_wait(
            PQsocket(pg), polling == PGRES_POLLING_WRITING, stopfd, deadline);
        if (ready <= 0)
            return ready == 0 ? DB_ERR_TIMEOUT : DB_ERR_UNAVAILABLE;
        polling = PQconnectPoll(pg);
        if (polling == PGRES_POLLING_OK)
            break;
        if (polling == PGRES_POLLING_FAILED) {
            pg_error_log(what, PQerrorMessage(pg));
            return DB_ERR_CONNECT;
        }
    }
    if (PQsetnonblocking(pg, 1) != 0) {
        pg_error_log(what, PQerrorMessage(pg));
        return DB_ERR_RESOURCE;
    }
    return DB_OK;
}

PGconn* pg_open(const char* dsn, const struct PgDeadline* deadline,
                enum DbError* code)
{
    PGconn* pg;

    if (!(pg = pg_connect_start(dsn))) {
        *code = DB_ERR_RESOURCE;
        return NULL;
    }
    if ((*code = pg_connect_finish(pg, -1, deadline, "connecting")) != DB_OK) {
        PQfinish(pg);
        return NULL;
    }
    return pg;
}

/* Wait for the result of what was just sent and check it: the common tail
 * of pg_run() and pg_run_params(). */
static int pg_run_finish(PGconn* pg, const struct PgDeadline* deadline,
                         const char* what, PGresult** res_out,
                         enum DbError* code)
{
    PGresult* res;
    ExecStatusType status;
    int collected;

    collected = pg_collect(pg, -1, deadline, &res);
    if (collected == -1) {
        if (!pg_cancel(pg, -1))
            *code = DB_ERR_CONNECT;
        else
            *code = DB_ERR_TIMEOUT;
        return 0;
    }
    if (collected < 0 || !res) {
        *code = DB_ERR_CONNECT;
        return 0;
    }
    status = PQresultStatus(res);
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK &&
        status != PGRES_COPY_IN) {
        *code =
            pg_error_from_sqlstate(PQresultErrorField(res, PG_DIAG_SQLSTATE));
        pg_error_log(what, PQresultErrorMessage(res));
        PQclear(res);
        return 0;
    }
    if (res_out)
        *res_out = res;
    else
        PQclear(res);
    *code = DB_OK;
    return 1;
}

int pg_run(PGconn* pg, const struct PgDeadline* deadline, const char* sql,
           int simple, const char* what, PGresult** res_out,
           enum DbError* code)
{
    int sent;

    if (res_out)
        *res_out = NULL;
    if (simple)
        sent = PQsendQuery(pg, sql);
    else
        sent = PQsendQueryParams(pg, sql, 0, NULL, NULL, NULL, NULL, 0);
    if (!sent) {
        pg_error_log(what, PQerrorMessage(pg));
        *code = DB_ERR_CONNECT;
        return 0;
    }
    return pg_run_finish(pg, deadline, what, res_out, code);
}

int pg_run_params(PGconn* pg, const struct PgDeadline* deadline,
                  const char* sql, int nparams, const char* const* values,
                  const char* what, PGresult** res_out, enum DbError* code)
{
    if (res_out)
        *res_out = NULL;
    if (!PQsendQueryParams(pg, sql, nparams, NULL, values, NULL, NULL, 0)) {
        pg_error_log(what, PQerrorMessage(pg));
        *code = DB_ERR_CONNECT;
        return 0;
    }
    return pg_run_finish(pg, deadline, what, res_out, code);
}

void pg_session_setup(PGconn* pg, const struct PgDeadline* deadline,
                      int timeout_ms, const char* search_path)
{
    const char* values[2];
    char timeout[32];
    enum DbError code;

    /* set_config() rather than SET, because it is a SELECT and so takes
     * its values as bound parameters like everything else. */
    snprintf(timeout, sizeof(timeout), "%d", timeout_ms);
    values[0] = timeout;
    values[1] = search_path ? search_path : "public";
    /* A failure has been logged, and is survivable. */
    pg_run_params(pg, deadline,
                  "select set_config('statement_timeout', $1, false),"
                  " set_config('client_min_messages', 'warning', false),"
                  " set_config('search_path', $2, false)",
                  2, values, "setting up the session", NULL, &code);
}

/*************************************************************************/
/************************* A pooled connection ***************************/
/*************************************************************************/

/* Close the connection and forget everything prepared on it. */
static void pg_disconnect(struct PgConn* conn)
{
    if (conn->pgc_pg) {
        PQfinish(conn->pgc_pg);
        conn->pgc_pg = NULL;
    }
    pg_stmt_clear(conn);
}

/* Open the connection, if it is not open already.  Returns DB_OK when
 * there is a usable connection. */
static enum DbError pg_connect(struct PgConn* conn, int stopfd,
                               const struct PgDeadline* deadline)
{
    struct timespec now;
    enum DbError code;
    PGconn* pg;

    if (conn->pgc_pg && PQstatus(conn->pgc_pg) == CONNECTION_OK)
        return DB_OK;
    pg_disconnect(conn);

    /* A database that is down would otherwise be dialled once per queued
     * query, each attempt waiting out the whole deadline.  One attempt
     * every few seconds is enough to notice it coming back. */
    pg_now(&now);
    if (conn->pgc_retry_at && now.tv_sec < conn->pgc_retry_at)
        return DB_ERR_CONNECT;

    if (!(pg = pg_connect_start(pg_pool_dsn(conn->pgc_pool))))
        return DB_ERR_RESOURCE;
    if ((code = pg_connect_finish(pg, stopfd, deadline, "connecting")) !=
        DB_OK) {
        PQfinish(pg);
        conn->pgc_retry_at = now.tv_sec + PG_RECONNECT_DELAY;
        pg_pool_count_connect(conn->pgc_pool, 0);
        return code;
    }

    conn->pgc_pg = pg;
    conn->pgc_retry_at = 0;
    pg_pool_count_connect(conn->pgc_pool, 1);

    /* Arm the server side of the deadline, and point the search path at
     * the configured schema, so that a module's queries find the tables
     * its migrations created there. */
    pg_session_setup(pg, deadline, pg_pool_timeout(conn->pgc_pool),
                     pg_pool_search_path(conn->pgc_pool));

    worker_log("database: %s connected (%s pool)", conn->pgc_name,
               pg_pool_label(conn->pgc_pool));
    return DB_OK;
}

/* Prepare `req''s statement, or find it already prepared. */
static enum DbError pg_prepare(struct PgConn* conn, struct PgRequest* req,
                               int stopfd, const struct PgDeadline* deadline,
                               struct PgStmt** stmt_out)
{
    unsigned int hash = pg_stmt_hash(req->pgr_sql);
    Oid types[DB_MAX_PARAMS];
    struct PgStmt* stmt;
    PGresult* res;
    enum DbError code;
    unsigned int n;
    int collected;

    if ((*stmt_out = pg_stmt_find(conn, req, hash)) != NULL)
        return DB_OK;

    /* The cache is full.  Rather than send a DEALLOCATE -- the one
     * statement that would have to go outside the prepared path -- the
     * connection is closed, which discards every statement prepared on it
     * at once.  A module that gets here is generating statement text per
     * call, and should not be. */
    if (conn->pgc_nstmts >= PG_STMT_CACHE) {
        worker_log("database: %s recycling: %u prepared statements cached",
                   conn->pgc_name, conn->pgc_nstmts);
        pg_disconnect(conn);
        if ((code = pg_connect(conn, stopfd, deadline)) != DB_OK)
            return code;
    }

    for (n = 0; n < req->pgr_nparams; n++)
        types[n] = req->pgr_params[n].pgb_type;

    if (!(stmt = worker_alloc(sizeof(*stmt))))
        return DB_ERR_RESOURCE;
    stmt->pgs_hash = hash;
    stmt->pgs_nparams = req->pgr_nparams;
    snprintf(stmt->pgs_name, sizeof(stmt->pgs_name), "svc_%lu",
             ++conn->pgc_seq);
    if (!(stmt->pgs_sql = worker_alloc(strlen(req->pgr_sql) + 1))) {
        worker_free(stmt);
        return DB_ERR_RESOURCE;
    }
    strcpy(stmt->pgs_sql, req->pgr_sql);
    if (req->pgr_nparams) {
        stmt->pgs_types = worker_alloc(sizeof(Oid) * req->pgr_nparams);
        if (!stmt->pgs_types) {
            pg_stmt_free(stmt);
            return DB_ERR_RESOURCE;
        }
        memcpy(stmt->pgs_types, types, sizeof(Oid) * req->pgr_nparams);
    }

    if (!PQsendPrepare(conn->pgc_pg, stmt->pgs_name, req->pgr_sql,
                       (int)req->pgr_nparams,
                       req->pgr_nparams ? types : NULL)) {
        pg_error_log("preparing a statement", PQerrorMessage(conn->pgc_pg));
        pg_stmt_free(stmt);
        return DB_ERR_CONNECT;
    }

    collected = pg_collect(conn->pgc_pg, stopfd, deadline, &res);
    if (collected == -1) {
        pg_stmt_free(stmt);
        if (!pg_cancel(conn->pgc_pg, stopfd))
            pg_disconnect(conn);
        return DB_ERR_TIMEOUT;
    }
    if (collected < 0 || !res) {
        pg_stmt_free(stmt);
        pg_disconnect(conn);
        return DB_ERR_CONNECT;
    }
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        code =
            pg_error_from_sqlstate(PQresultErrorField(res, PG_DIAG_SQLSTATE));
        pg_error_log("preparing a statement", PQresultErrorMessage(res));
        PQclear(res);
        pg_stmt_free(stmt);
        return code;
    }
    PQclear(res);

    stmt->pgs_hnext = conn->pgc_buckets[hash % PG_STMT_BUCKETS];
    conn->pgc_buckets[hash % PG_STMT_BUCKETS] = stmt;
    conn->pgc_nstmts++;
    *stmt_out = stmt;
    return DB_OK;
}

/* Run one request on an open connection. */
static enum DbError pg_execute(struct PgConn* conn, struct PgRequest* req,
                               int stopfd, const struct PgDeadline* deadline,
                               json_t** rows, unsigned int* nrows)
{
    const char* values[DB_MAX_PARAMS];
    int lengths[DB_MAX_PARAMS];
    int formats[DB_MAX_PARAMS];
    struct PgStmt* stmt;
    PGresult* res;
    enum DbError code;
    unsigned int n;
    int collected;

    if ((code = pg_prepare(conn, req, stopfd, deadline, &stmt)) != DB_OK)
        return code;

    for (n = 0; n < req->pgr_nparams; n++) {
        values[n] = req->pgr_params[n].pgb_value;
        lengths[n] = req->pgr_params[n].pgb_length;
        formats[n] = req->pgr_params[n].pgb_format;
    }

    /* Result format 0: the server renders the values and pg_json.c decides
     * what each one is worth as JSON. */
    if (!PQsendQueryPrepared(conn->pgc_pg, stmt->pgs_name,
                             (int)req->pgr_nparams,
                             req->pgr_nparams ? values : NULL,
                             req->pgr_nparams ? lengths : NULL,
                             req->pgr_nparams ? formats : NULL, 0)) {
        pg_error_log("running a statement", PQerrorMessage(conn->pgc_pg));
        pg_disconnect(conn);
        return DB_ERR_CONNECT;
    }

    collected = pg_collect(conn->pgc_pg, stopfd, deadline, &res);
    if (collected == -1) {
        if (!pg_cancel(conn->pgc_pg, stopfd))
            pg_disconnect(conn);
        return DB_ERR_TIMEOUT;
    }
    if (collected < 0 || !res) {
        pg_disconnect(conn);
        return DB_ERR_CONNECT;
    }

    switch (PQresultStatus(res)) {
        case PGRES_TUPLES_OK:
            *rows = pg_json_rows(res);
            *nrows = (unsigned int)PQntuples(res);
            code = *rows ? DB_OK : DB_ERR_RESOURCE;
            break;
        case PGRES_COMMAND_OK: {
            /* A statement that returns nothing still has an answer: how many
             * rows it touched.  The rows themselves are an empty array, not
             * NULL, so a caller can iterate any result without a special
             * case. */
            const char* affected = PQcmdTuples(res);
            *rows = json_array();
            *nrows =
                (affected && *affected) ? (unsigned int)atoi(affected) : 0;
            code = *rows ? DB_OK : DB_ERR_RESOURCE;
            break;
        }
        default:
            code = pg_error_from_sqlstate(
                PQresultErrorField(res, PG_DIAG_SQLSTATE));
            pg_error_log("running a statement", PQresultErrorMessage(res));
            break;
    }
    PQclear(res);
    return code;
}

/*************************************************************************/
/******************************* The thread ******************************/
/*************************************************************************/

struct PgConn* pg_conn_new(struct PgPool* pool, unsigned int index)
{
    struct PgConn* conn = worker_alloc(sizeof(*conn));

    if (!conn)
        return NULL;
    conn->pgc_pool = pool;
    snprintf(conn->pgc_name, sizeof(conn->pgc_name), "pg-%s-%u",
             pg_pool_label(pool), index);
    return conn;
}

void pg_conn_free(struct PgConn* conn)
{
    if (!conn)
        return;
    pg_disconnect(conn);
    worker_free(conn);
}

const char* pg_conn_name(const struct PgConn* conn)
{
    return conn->pgc_name;
}

void pg_conn_main(struct Worker* worker, void* arg)
{
    struct PgConn* conn = arg;
    int stopfd = worker_stop_fd(worker);

    while (!worker_stopping(worker)) {
        struct PgRequest* req = pg_pool_take(conn->pgc_pool, worker);
        struct PgDeadline deadline;
        json_t* rows = NULL;
        unsigned int nrows = 0;
        enum DbError code;

        if (!req)
            continue;
        pg_deadline_set(&deadline, req->pgr_timeout_ms);
        if ((code = pg_connect(conn, stopfd, &deadline)) == DB_OK)
            code = pg_execute(conn, req, stopfd, &deadline, &rows, &nrows);
        if (code != DB_OK && rows) {
            json_decref(rows);
            rows = NULL;
            nrows = 0;
        }
        pg_pool_count(conn->pgc_pool, code);
        pg_pool_answer(worker, req->pgr_id, rows, nrows, code);
        pg_request_free(req);
    }

    /* The connection belongs to this thread and nothing else may close
     * it, so it goes now rather than in pg_conn_free(). */
    pg_disconnect(conn);
}

/*************************************************************************/
/********************************* Helpers *******************************/
/*************************************************************************/

char* pg_quote_ident(const char* name, char* buf, size_t size)
{
    size_t n = 0;

    if (size < 3)
        return NULL;
    buf[n++] = '"';
    for (; *name; name++) {
        if (*name == '"') {
            if (n + 2 >= size - 1)
                return NULL;
            buf[n++] = '"';
        }
        if (n + 1 >= size - 1)
            return NULL;
        buf[n++] = *name;
    }
    buf[n++] = '"';
    buf[n] = '\0';
    return buf;
}

char* pg_search_path(const char* schema, char* buf, size_t size)
{
    char quoted[256];

    if (!schema || !pg_quote_ident(schema, quoted, sizeof(quoted)))
        snprintf(buf, size, "public");
    else
        snprintf(buf, size, "%s, public", quoted);
    return buf;
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
