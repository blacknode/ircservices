/* The entity store: the work done in worker threads.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Two kinds of work, both off the main thread, both obeying worker.h: no
 * services.h, the system allocator, worker_log() for logging.
 *
 * THE WRITER is one dedicated worker with a PostgreSQL connection and a
 * Redis connection of its own.  It takes queued writes in batches and runs
 * each batch as one transaction:
 *
 *     BEGIN
 *     SELECT token FROM instance WHERE id = 1 FOR UPDATE   -- still ours?
 *     -- for each record: its statements (delete + insert per table)
 *     COMMIT
 *
 * and then copies the new bundles into Redis (MSET, pipelined).  One
 * writer, so writes to one record reach the database in the order they
 * were made; within a batch only the newest write of a record is run.  A
 * batch that fails for want of a database (no connection, a timeout) is
 * kept and tried again, every second, until it goes through: nothing is
 * dropped because PostgreSQL was down for a while.  A batch the database
 * refuses (a value it will not take) is split, so that one bad record is
 * reported and dropped and the others are written.
 *
 * THE FETCHES (store_prefetch()) run on the worker pool.  Each pool thread
 * keeps a PostgreSQL and a Redis connection of its own for as long as it
 * lives (thread-local, closed when the thread ends), because opening one
 * per batch would cost more than the batch.  A fetch asks Redis for every
 * key at once (MGET), asks PostgreSQL for all the misses in one query, and
 * copies what PostgreSQL said into Redis for the next time.
 */

#include "store_int.h"

#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************************************************************************/
/******************************** Helpers ********************************/
/*************************************************************************/

char* store_strdup(const char* s)
{
    return s ? strdup(s) : NULL;
}

void* store_calloc(size_t n, size_t size)
{
    void* p = calloc(n, size);

    if (!p) {
        /* As scalloc() does: nothing sensible can go on without it. */
        abort();
    }
    return p;
}

void store_free(void* p)
{
    free(p);
}

struct StoreConn* store_conn_copy(const struct StoreConn* conn)
{
    struct StoreConn* copy = calloc(1, sizeof(*copy));

    if (!copy)
        return NULL;
    *copy = *conn;
    copy->sc_dsn = store_strdup(conn->sc_dsn);
    copy->sc_search_path = store_strdup(conn->sc_search_path);
    copy->sc_token = store_strdup(conn->sc_token);
    copy->sc_check = store_strdup(conn->sc_check);
    return copy;
}

void store_conn_free(struct StoreConn* conn)
{
    if (!conn)
        return;
    free(conn->sc_dsn);
    free(conn->sc_search_path);
    free(conn->sc_token);
    free(conn->sc_check);
    free(conn);
}

void store_write_free(struct StoreWrite* w)
{
    int i;

    if (!w)
        return;
    for (i = 0; i < w->sw_nstmts; i++)
        free(w->sw_stmts[i]);
    free(w->sw_stmts);
    free(w->sw_param_bundle);
    free(w->sw_ident);
    free(w->sw_key);
    free(w->sw_bundle);
    free(w->sw_redis_key);
    free(w);
}

void store_fetch_free(struct StoreFetch* f)
{
    int i;

    if (!f)
        return;
    for (i = 0; i < f->sf_nkeys; i++) {
        free(f->sf_keys ? f->sf_keys[i] : NULL);
        free(f->sf_redis_keys ? f->sf_redis_keys[i] : NULL);
        free(f->sf_bundles ? f->sf_bundles[i] : NULL);
    }
    free(f->sf_keys);
    free(f->sf_redis_keys);
    free(f->sf_bundles);
    free(f->sf_found);
    free(f->sf_sql);
    store_conn_free(f->sf_conn);
    free(f);
}

/* Connections of one thread, rebuilt when the configuration changes. */
struct StoreLink {
    PGconn* sl_pg;
    struct RedisClient* sl_redis;
    unsigned int sl_generation;
};

static void link_close(struct StoreLink* link)
{
    if (link->sl_pg) {
        PQfinish(link->sl_pg);
        link->sl_pg = NULL;
    }
    if (link->sl_redis) {
        redis_client_close(link->sl_redis);
        link->sl_redis = NULL;
    }
}

/* PostgreSQL for `conn', opened if needed.  NULL with `*code' on failure. */
static PGconn* link_pg(struct StoreLink* link, const struct StoreConn* conn,
                       const struct PgDeadline* deadline, enum DbError* code)
{
    if (link->sl_generation != conn->sc_generation) {
        link_close(link);
        link->sl_generation = conn->sc_generation;
    }
    if (link->sl_pg && PQstatus(link->sl_pg) != CONNECTION_OK) {
        PQfinish(link->sl_pg);
        link->sl_pg = NULL;
    }
    if (!link->sl_pg) {
        if (!(link->sl_pg = pg_open(conn->sc_dsn, deadline, code)))
            return NULL;
        pg_session_setup(link->sl_pg, deadline, conn->sc_timeout_ms,
                         conn->sc_search_path);
    }
    return link->sl_pg;
}

static struct RedisClient* link_redis(struct StoreLink* link,
                                      const struct StoreConn* conn)
{
    if (link->sl_generation != conn->sc_generation) {
        link_close(link);
        link->sl_generation = conn->sc_generation;
    }
    if (!link->sl_redis)
        link->sl_redis = redis_client_new(&conn->sc_redis);
    return link->sl_redis;
}

/* Copy bundles into Redis: `bundles[i]' NULL means "no such record".  The
 * two kinds expire differently, so they go in two pipelines. */
static int redis_store(struct RedisClient* redis, const struct StoreConn* conn,
                       char** keys, char** bundles, int n)
{
    const char **k1, **v1, **k2, **v2;
    size_t *l1, *l2;
    int i, n1 = 0, n2 = 0, res = 0;

    if (n <= 0)
        return 0;
    k1 = malloc(sizeof(*k1) * n);
    v1 = malloc(sizeof(*v1) * n);
    l1 = malloc(sizeof(*l1) * n);
    k2 = malloc(sizeof(*k2) * n);
    v2 = malloc(sizeof(*v2) * n);
    l2 = malloc(sizeof(*l2) * n);
    if (!k1 || !v1 || !l1 || !k2 || !v2 || !l2) {
        res = -1;
        goto out;
    }
    for (i = 0; i < n; i++) {
        if (bundles[i]) {
            k1[n1] = keys[i];
            v1[n1] = bundles[i];
            l1[n1++] = strlen(bundles[i]);
        }
        else {
            k2[n2] = keys[i];
            v2[n2] = STORE_NEGATIVE;
            l2[n2++] = strlen(STORE_NEGATIVE);
        }
    }
    if (n1 && redis_client_mset(redis, k1, v1, l1, n1, conn->sc_ttl) < 0)
        res = -1;
    if (n2 &&
        redis_client_mset(redis, k2, v2, l2, n2, conn->sc_negative_ttl) < 0)
        res = -1;
out:
    free(k1);
    free(v1);
    free(l1);
    free(k2);
    free(v2);
    free(l2);
    return res;
}

/* Nonzero if `code' says the database, not the data, was the problem. */
static int transient(enum DbError code)
{
    return code == DB_ERR_CONNECT || code == DB_ERR_TIMEOUT ||
           code == DB_ERR_UNAVAILABLE || code == DB_ERR_RESOURCE ||
           code == DB_ERR_RETRY;
}

/*************************************************************************/
/******************************** Fetches ********************************/
/*************************************************************************/

static pthread_key_t fetch_link_key;
static pthread_once_t fetch_link_once = PTHREAD_ONCE_INIT;

static void fetch_link_destroy(void* p)
{
    link_close(p);
    free(p);
}

static void fetch_link_init(void)
{
    pthread_key_create(&fetch_link_key, fetch_link_destroy);
}

static struct StoreLink* fetch_link(void)
{
    struct StoreLink* link;

    pthread_once(&fetch_link_once, fetch_link_init);
    if (!(link = pthread_getspecific(fetch_link_key))) {
        if ((link = calloc(1, sizeof(*link))) != NULL)
            pthread_setspecific(fetch_link_key, link);
    }
    return link;
}

/* A JSON array of strings, for the batch query's $1.  Keys are ours (a
 * normalized nick or channel name, a number), but they are escaped all the
 * same: this is a value, bound as one. */
static char* json_string_array(char** items, const int* skip, int n)
{
    size_t size = 3, len = 0;
    char* out;
    int i, first = 1;

    for (i = 0; i < n; i++)
        size += strlen(items[i]) * 6 + 3;
    if (!(out = malloc(size)))
        return NULL;
    out[len++] = '[';
    for (i = 0; i < n; i++) {
        const unsigned char* s;
        if (skip[i])
            continue;
        if (!first)
            out[len++] = ',';
        first = 0;
        out[len++] = '"';
        for (s = (const unsigned char*)items[i]; *s; s++) {
            if (*s == '"' || *s == '\\') {
                out[len++] = '\\';
                out[len++] = (char)*s;
            }
            else if (*s < 0x20) {
                len += (size_t)snprintf(out + len, size - len, "\\u%04x", *s);
            }
            else {
                out[len++] = (char)*s;
            }
        }
        out[len++] = '"';
    }
    out[len++] = ']';
    out[len] = 0;
    return out;
}

void store_fetch_run(struct StoreFetch* f)
{
    struct StoreLink* link = fetch_link();
    struct PgDeadline deadline;
    struct RedisClient* redis;
    size_t* lens;
    char* keys_json = NULL;
    enum DbError code;
    PGresult* res;
    PGconn* pg;
    int i, row, misses = 0;

    f->sf_bundles = calloc(f->sf_nkeys, sizeof(*f->sf_bundles));
    f->sf_found = calloc(f->sf_nkeys, sizeof(*f->sf_found));
    lens = calloc(f->sf_nkeys, sizeof(*lens));
    if (!link || !f->sf_bundles || !f->sf_found || !lens) {
        f->sf_failed = 1;
        free(lens);
        return;
    }

    /* Redis first, all at once. */
    redis = link_redis(link, f->sf_conn);
    if (redis &&
        redis_client_mget(redis, (const char**)f->sf_redis_keys, f->sf_nkeys,
                          f->sf_bundles, lens) == 0) {
        for (i = 0; i < f->sf_nkeys; i++) {
            if (!f->sf_bundles[i])
                continue;
            f->sf_found[i] = 1;
            if (strcmp(f->sf_bundles[i], STORE_NEGATIVE) == 0) {
                free(f->sf_bundles[i]);
                f->sf_bundles[i] = NULL;
            }
        }
    }
    free(lens);
    for (i = 0; i < f->sf_nkeys; i++) {
        if (!f->sf_found[i])
            misses++;
    }
    if (!misses)
        return;

    /* PostgreSQL for the misses, in one query. */
    pg_deadline_set(&deadline, f->sf_conn->sc_timeout_ms);
    if (!(pg = link_pg(link, f->sf_conn, &deadline, &code)) ||
        !(keys_json = json_string_array(f->sf_keys, f->sf_found,
                                        f->sf_nkeys))) {
        f->sf_failed = 1;
        free(keys_json);
        return;
    }
    {
        const char* values[1] = {keys_json};
        if (!pg_run_params(pg, &deadline, f->sf_sql, 1, values,
                           "fetching records", &res, &code)) {
            f->sf_failed = 1;
            free(keys_json);
            if (transient(code) && link->sl_pg &&
                PQstatus(link->sl_pg) != CONNECTION_OK) {
                PQfinish(link->sl_pg);
                link->sl_pg = NULL;
            }
            return;
        }
    }
    free(keys_json);
    {
        char** mkeys = calloc(misses, sizeof(*mkeys));
        char** mbundles = calloc(misses, sizeof(*mbundles));
        int nm = 0;

        for (row = 0; row < PQntuples(res); row++) {
            const char* key = PQgetvalue(res, row, 0);
            int missing = *PQgetvalue(res, row, 1) == 't';
            for (i = 0; i < f->sf_nkeys; i++) {
                if (!f->sf_found[i] && strcmp(f->sf_keys[i], key) == 0)
                    break;
            }
            if (i >= f->sf_nkeys)
                continue;
            f->sf_found[i] = 1;
            if (!missing)
                f->sf_bundles[i] = store_strdup(PQgetvalue(res, row, 2));
            if (mkeys && mbundles && nm < misses) {
                mkeys[nm] = f->sf_redis_keys[i];
                mbundles[nm++] = f->sf_bundles[i];
            }
        }
        PQclear(res);
        /* For the next time.  A failure here costs a database query later,
         * nothing else. */
        if (mkeys && mbundles && redis)
            redis_store(redis, f->sf_conn, mkeys, mbundles, nm);
        free(mkeys);
        free(mbundles);
    }
}

/*************************************************************************/
/******************************** The writer *****************************/
/*************************************************************************/

/* Most writes in one transaction. */
#define WRITER_BATCH 256

static struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    struct StoreWrite *head, *tail;
    unsigned int queued;
    struct StoreConn* conn;    /* Current configuration (under lock) */
    struct Worker* worker;
    StoreWriteDoneFn done;
    int running;
} wr = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

void store_writer_push(struct StoreWrite* w)
{
    pthread_mutex_lock(&wr.lock);
    w->sw_next = NULL;
    if (wr.tail)
        wr.tail->sw_next = w;
    else
        wr.head = w;
    wr.tail = w;
    wr.queued++;
    pthread_cond_signal(&wr.cond);
    pthread_mutex_unlock(&wr.lock);
}

unsigned int store_writer_queued(void)
{
    unsigned int n;

    pthread_mutex_lock(&wr.lock);
    n = wr.queued;
    pthread_mutex_unlock(&wr.lock);
    return n;
}

void store_writer_reconfigure(struct StoreConn* conn)
{
    pthread_mutex_lock(&wr.lock);
    store_conn_free(wr.conn);
    wr.conn = conn;
    pthread_mutex_unlock(&wr.lock);
}

/* Take up to WRITER_BATCH writes, waiting a little for the first. */
static struct StoreWrite* writer_take(struct Worker* worker, int* count)
{
    struct StoreWrite *list = NULL, **tail = &list;
    struct timespec until;

    *count = 0;
    pthread_mutex_lock(&wr.lock);
    if (!wr.head && !worker_stopping(worker)) {
        clock_gettime(CLOCK_REALTIME, &until);
        until.tv_nsec += 100 * 1000000L;
        if (until.tv_nsec >= 1000000000L) {
            until.tv_sec++;
            until.tv_nsec -= 1000000000L;
        }
        pthread_cond_timedwait(&wr.cond, &wr.lock, &until);
    }
    while (wr.head && *count < WRITER_BATCH) {
        struct StoreWrite* w = wr.head;
        wr.head = w->sw_next;
        wr.queued--;
        w->sw_next = NULL;
        *tail = w;
        tail = &w->sw_next;
        (*count)++;
    }
    if (!wr.head)
        wr.tail = NULL;
    pthread_mutex_unlock(&wr.lock);
    return list;
}

/* Run one write's statements (inside a transaction).  Nonzero on
 * success. */
static int writer_apply(PGconn* pg, const struct PgDeadline* deadline,
                        struct StoreWrite* w, enum DbError* code)
{
    int i;

    for (i = 0; i < w->sw_nstmts; i++) {
        const char* values[1];
        values[0] = w->sw_param_bundle[i] ? w->sw_bundle : w->sw_key;
        if (!pg_run_params(pg, deadline, w->sw_stmts[i], 1, values,
                           "writing a record", NULL, code)) {
            snprintf(w->sw_error, sizeof(w->sw_error), "%s",
                     pg_error_message(*code));
            return 0;
        }
    }
    return 1;
}

/* Is the schema still ours?  Takes the instance row's lock for the rest of
 * the transaction. */
static int writer_check(PGconn* pg, const struct StoreConn* conn,
                        const struct PgDeadline* deadline, enum DbError* code)
{
    PGresult* res;
    int ok;

    if (!conn->sc_token)
        return 1;
    if (!pg_run(pg, deadline, conn->sc_check, 0, "checking the instance",
                &res, code))
        return 0;
    ok = PQntuples(res) == 1 &&
         strcmp(PQgetvalue(res, 0, 0), conn->sc_token) == 0;
    PQclear(res);
    if (!ok)
        *code = DB_ERR_PERMISSION;
    return ok;
}

/* Run `ws' (count `n') in one transaction.  Returns DB_OK, or the code of
 * the failure; `*bad' is the write that failed, if one did. */
static enum DbError writer_transaction(PGconn* pg,
                                       const struct StoreConn* conn,
                                       struct StoreWrite** ws, int n,
                                       struct StoreWrite** bad)
{
    struct PgDeadline deadline;
    enum DbError code = DB_OK, ignored;
    int i;

    *bad = NULL;
    pg_deadline_set(&deadline, conn->sc_timeout_ms);
    if (!pg_run(pg, &deadline, "begin", 1, "beginning a write", NULL, &code))
        return code;
    if (!writer_check(pg, conn, &deadline, &code))
        goto fail;
    for (i = 0; i < n; i++) {
        if (!writer_apply(pg, &deadline, ws[i], &code)) {
            *bad = ws[i];
            goto fail;
        }
    }
    if (!pg_run(pg, &deadline, "commit", 1, "committing a write", NULL,
                &code))
        goto fail;
    return DB_OK;

fail:
    if (PQstatus(pg) == CONNECTION_OK &&
        PQtransactionStatus(pg) != PQTRANS_ACTIVE)
        pg_run(pg, &deadline, "rollback", 1, "rolling back a write", NULL,
               &ignored);
    return code;
}

/* Hand a list of finished writes back to the main thread. */
static void writer_done_task(struct WorkTask* task)
{
    if (wr.done)
        (*wr.done)(task->wt_in);
    task->wt_in = NULL;
}

static void writer_free_task(struct WorkTask* task)
{
    struct StoreWrite* w = task->wt_in;

    while (w) {
        struct StoreWrite* next = w->sw_next;
        store_write_free(w);
        w = next;
    }
    task->wt_in = NULL;
}

static void writer_report(struct Worker* worker, struct StoreWrite* list)
{
    struct WorkTask* task = worker_task_new(NULL, writer_done_task);

    if (!task)
        return; /* leaked rather than lost: this cannot happen short of OOM */
    task->wt_in = list;
    task->wt_free = writer_free_task;
    while (!worker_post(worker, task)) {
        /* The reply queue is full: the main thread is behind.  Waiting
         * here is right -- the next batch can wait too. */
        struct pollfd pfd = {worker_stop_fd(worker), POLLIN, 0};
        if (poll(&pfd, 1, 50) > 0 && worker_stopping(worker)) {
            worker_task_free(task);
            return;
        }
    }
}

/* Sleep up to `ms' or until asked to stop. */
static void writer_nap(struct Worker* worker, int ms)
{
    struct pollfd pfd = {worker_stop_fd(worker), POLLIN, 0};

    poll(&pfd, 1, ms);
}

/* Redis could not take what the database just did: remember the keys in
 * the database, so that they are deleted from Redis when it answers again
 * (or when Services next start), whatever happens to this process. */
static void writer_mark_stale(struct StoreLink* link,
                              const struct StoreConn* conn, char** keys, int n)
{
    struct PgDeadline deadline;
    enum DbError code;
    int i;

    pg_deadline_set(&deadline, conn->sc_timeout_ms);
    if (!link_pg(link, conn, &deadline, &code))
        return;
    for (i = 0; i < n; i++) {
        const char* values[1] = {keys[i]};
        if (!pg_run_params(link->sl_pg, &deadline,
                           "insert into store_stale (redis_key) values ($1)"
                           " on conflict do nothing",
                           1, values, "recording a stale cache key", NULL,
                           &code))
            break;
    }
}

/* Write one batch: to PostgreSQL (retrying while the database is down,
 * if there is a thread to wait in), then to Redis, and hand it back.
 * `worker' is NULL when the main thread does this itself (before the
 * worker threads exist, or after they are gone): it then tries once. */
static void writer_batch(struct Worker* worker, struct StoreLink* link,
                         struct StoreWrite* list, int* complained)
{
    struct StoreWrite *w, *run[WRITER_BATCH], *bad = NULL;
    struct StoreConn* conn = NULL;
    struct PgDeadline deadline;
    enum DbError code;
    PGconn* pg;
    int n = 0, i, j;

    /* Within the batch, only the newest write of each record runs; the
     * older ones are superseded, and done. */
    for (w = list; w; w = w->sw_next) {
        struct StoreWrite* v;
        int superseded = 0;
        for (v = w->sw_next; v; v = v->sw_next) {
            if (strcmp(v->sw_ident, w->sw_ident) == 0) {
                superseded = 1;
                break;
            }
        }
        if (superseded)
            w->sw_done_pg = w->sw_done_redis = 1;
        else
            run[n++] = w;
    }

    /* Until the database takes the batch. */
    for (;;) {
        pthread_mutex_lock(&wr.lock);
        conn = store_conn_copy(wr.conn);
        pthread_mutex_unlock(&wr.lock);
        if (!conn) {
            if (!worker || worker_stopping(worker))
                break;
            writer_nap(worker, 1000);
            continue;
        }
        pg_deadline_set(&deadline, conn->sc_timeout_ms);
        if (!(pg = link_pg(link, conn, &deadline, &code))) {
            code = DB_ERR_CONNECT;
        }
        else {
            code = writer_transaction(pg, conn, run, n, &bad);
            if (code != DB_OK && bad && !transient(code)) {
                /* The database refused one record: write the others one
                 * by one, so that only the bad ones are lost. */
                for (i = 0; i < n; i++) {
                    struct StoreWrite* one = run[i];
                    enum DbError c1 =
                        writer_transaction(pg, conn, &one, 1, &bad);
                    if (c1 == DB_OK) {
                        run[i]->sw_done_pg = 1;
                    }
                    else if (!transient(c1)) {
                        run[i]->sw_done_pg = 1;
                        run[i]->sw_failed = 1;
                    }
                    else {
                        code = c1;
                        break;
                    }
                }
                if (i >= n)
                    code = DB_OK;
            }
            else if (code == DB_OK) {
                for (i = 0; i < n; i++)
                    run[i]->sw_done_pg = 1;
            }
        }
        if (code == DB_OK) {
            *complained = 0;
            break;
        }
        if (code == DB_ERR_PERMISSION && !bad) {
            /* Another copy of Services owns the schema: writing on would
             * undo its work.  Report the writes as failed. */
            worker_log("database: another copy of Services has claimed the"
                       " schema; %d write%s dropped",
                       n, n == 1 ? "" : "s");
            for (i = 0; i < n; i++) {
                run[i]->sw_done_pg = 1;
                run[i]->sw_failed = 1;
                snprintf(run[i]->sw_error, sizeof(run[i]->sw_error),
                         "schema claimed by another copy");
            }
            break;
        }
        if (!(*complained)++)
            worker_log("database: %d write%s waiting for the database (%s);"
                       " retrying",
                       n, n == 1 ? "" : "s", pg_error_message(code));
        if (link->sl_pg && PQstatus(link->sl_pg) != CONNECTION_OK) {
            PQfinish(link->sl_pg);
            link->sl_pg = NULL;
        }
        store_conn_free(conn);
        conn = NULL;
        /* Without a thread to wait in, or when told to stop: leave them
         * pending -- the main thread knows they are not in the database. */
        if (!worker || worker_stopping(worker))
            break;
        writer_nap(worker, 1000);
    }

    /* Then Redis, for what the database took. */
    if (conn) {
        char *keys[WRITER_BATCH], *bundles[WRITER_BATCH];
        struct RedisClient* redis = link_redis(link, conn);
        for (i = j = 0; i < n; i++) {
            if (run[i]->sw_done_pg && !run[i]->sw_failed) {
                keys[j] = run[i]->sw_redis_key;
                bundles[j++] = run[i]->sw_bundle;
            }
        }
        if (redis && redis_store(redis, conn, keys, bundles, j) == 0) {
            for (i = 0; i < n; i++) {
                if (run[i]->sw_done_pg && !run[i]->sw_failed)
                    run[i]->sw_done_redis = 1;
            }
        }
        else if (j > 0) {
            writer_mark_stale(link, conn, keys, j);
        }
        store_conn_free(conn);
    }
    if (worker)
        writer_report(worker, list);
    else if (wr.done)
        (*wr.done)(list);
}

static void writer_main(struct Worker* worker, void* arg)
{
    struct StoreLink link = {NULL, NULL, 0};
    int complained = 0;

    (void)arg;
    for (;;) {
        struct StoreWrite* list;
        int count;

        if (!(list = writer_take(worker, &count))) {
            if (worker_stopping(worker))
                break;
            continue;
        }
        writer_batch(worker, &link, list, &complained);
    }
    link_close(&link);
}

void store_writer_drain_sync(void)
{
    struct StoreLink link = {NULL, NULL, 0};
    int complained = 0;

    for (;;) {
        struct StoreWrite *list = NULL, **tail = &list;
        int count = 0;

        pthread_mutex_lock(&wr.lock);
        while (wr.head && count < WRITER_BATCH) {
            struct StoreWrite* w = wr.head;
            wr.head = w->sw_next;
            wr.queued--;
            w->sw_next = NULL;
            *tail = w;
            tail = &w->sw_next;
            count++;
        }
        if (!wr.head)
            wr.tail = NULL;
        pthread_mutex_unlock(&wr.lock);
        if (!list)
            break;
        writer_batch(NULL, &link, list, &complained);
    }
    link_close(&link);
}

int store_writer_start(struct StoreConn* conn, StoreWriteDoneFn done)
{
    store_writer_reconfigure(conn);
    wr.done = done;
    if (!(wr.worker = worker_spawn("store-writer", writer_main, NULL)))
        return 0;
    wr.running = 1;
    return 1;
}

void store_writer_stop(void)
{
    struct StoreWrite* w;

    if (wr.running) {
        worker_stop(wr.worker);
        wr.running = 0;
    }
    pthread_mutex_lock(&wr.lock);
    w = wr.head;
    wr.head = wr.tail = NULL;
    wr.queued = 0;
    store_conn_free(wr.conn);
    wr.conn = NULL;
    pthread_mutex_unlock(&wr.lock);
    while (w) {
        struct StoreWrite* next = w->sw_next;
        store_write_free(w);
        w = next;
    }
}
