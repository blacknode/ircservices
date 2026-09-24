/* The connection pools: a queue, some threads, and the copying.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/postgres/pg_pool.c).
 *
 * There is one pool per role.  A pool is a queue and a fixed number of
 * connection threads that take from it, and it is the only thing the two
 * sides of this driver share -- so it is also the only thing with a lock.
 *
 * NOTHING IS SHARED BUT THE QUEUE
 *
 * A query arrives in the main thread as a DbQuery pointing at the caller's
 * memory, which may be a local variable and is certainly not going to
 * outlive the call.  pg_pools_submit() copies all of it -- the statement,
 * every parameter, every byte of every value -- into a PgRequest allocated
 * with worker_alloc(), and that copy is what crosses the boundary.  The
 * answer comes back the same way: a JSON value built in the connection
 * thread, handed over through worker_post(), released in the main thread.
 *
 * POOLS START WHEN THEY ARE FIRST NEEDED
 *
 * The pool for a role is built the first time a query asks for it, and
 * torn down when db_conf_generation() says the configuration underneath it
 * has changed.  Services that never run a query never open a pooled
 * connection.  A query sent from a module's init_module(), before Services
 * have forked into the background, builds its pool all the same: the
 * connection threads are held until the worker subsystem comes up after
 * the fork (see worker_spawn_owned()), and the query waits in the queue.
 */

#include "postgres.h"

#include <string.h>

/* One role's queue, connections and counters. */
struct PgPool {
    enum DbRole pgp_role;  /* Role this pool serves. */
    const char* pgp_label; /* "read" or "write", for logs. */
    char* pgp_dsn;         /* Connection string, this pool's own. */
    char* pgp_search_path; /* Search path, this pool's own. */
    int pgp_timeout_ms;    /* Deadline given to every query. */
    unsigned int pgp_size; /* Connections wanted. */

    struct PgConn** pgp_conns;   /* #pgp_size connections. */
    struct Worker** pgp_workers; /* Their threads. */
    unsigned int pgp_running;    /* Threads actually started. */

    pthread_mutex_t pgp_lock;   /* Guards everything below. */
    pthread_cond_t pgp_cond;    /* Signalled when work arrives. */
    struct PgRequest* pgp_head; /* Oldest queued request. */
    struct PgRequest* pgp_tail; /* Newest queued request. */
    unsigned int pgp_queued;    /* Requests on the queue. */
    unsigned int pgp_max_queue; /* Queue depth before DB_ERR_BUSY. */
    int pgp_stopping;           /* Set when the pool is going away. */

    unsigned int pgp_submitted; /* Queries queued. */
    unsigned int pgp_ok;        /* Queries that succeeded. */
    unsigned int pgp_failed;    /* Queries that failed. */
    unsigned int pgp_timeouts;  /* Queries that ran out of time. */
    unsigned int pgp_rejected;  /* Queries refused for a full queue. */
    unsigned int pgp_connects;  /* Connections established. */
    unsigned int pgp_connfail;  /* Connection attempts that failed. */
};

/* The pools, one per role, built on demand. */
static struct PgPool* pg_pools[DB_ROLE_LAST];
/* Configuration generation the pools were built from. */
static unsigned int pg_pools_generation;
/* Set once the "workers are off" warning has been given. */
static int pg_warned_workers;

/* Word for each role, for logs and thread names. */
static const char* pg_role_label[DB_ROLE_LAST] = {"read", "write"};

/* ------------------------------------------------------------------------
 * Requests: copying in.
 * ------------------------------------------------------------------------ */

void pg_request_free(struct PgRequest* req)
{
    unsigned int n;

    if (!req)
        return;

    if (req->pgr_params) {
        for (n = 0; n < req->pgr_nparams; n++)
            worker_free(req->pgr_params[n].pgb_value);
        worker_free(req->pgr_params);
    }

    worker_free(req->pgr_sql);
    worker_free(req);
}

/* Copy one parameter into worker memory.
 *
 * @param[out] bound Receives the copy.
 * @param[in] param The caller's parameter.
 * @return DB_OK on success.
 */
static enum DbError pg_bind(struct PgBound* bound, const struct DbParam* param)
{
    size_t length;

    if (!param)
        return DB_ERR_PARAM;

    bound->pgb_type = pg_type_oid(param->type);
    bound->pgb_format = (param->format == DB_FORMAT_BINARY) ? 1 : 0;

    if (param->type == DB_TYPE_NULL || !param->value) {
        /* A NULL value is a NULL pointer in the array libpq receives; there is
         * nothing to copy and nothing to declare.
         */
        bound->pgb_value = 0;
        bound->pgb_length = 0;
        return DB_OK;
    }

    if (bound->pgb_format) {
        int fixed = pg_type_binary_length(param->type);

        /* DbParam has no length, so a binary value is only unambiguous when
         * its type has a fixed width.  See the note on DbFormat: the answer
         * for the rest is to send them as text.
         */
        if (fixed < 0)
            return DB_ERR_PARAM;

        length = (size_t)fixed;
    }
    else
        length = strlen(param->value);

    if (!(bound->pgb_value = (char*)worker_alloc(length + 1)))
        return DB_ERR_RESOURCE;

    memcpy(bound->pgb_value, param->value, length);
    bound->pgb_value[length] = '\0';
    bound->pgb_length = (int)length;

    return DB_OK;
}

/* Copy \a query into a request the connection thread can own.
 *
 * @param[in] id Handle for db_complete().
 * @param[in] query The caller's query.
 * @param[in] timeout_ms Deadline for the round trip.
 * @param[out] err Receives the reason when this returns NULL.
 * @return The request, or NULL.
 */
static struct PgRequest* pg_request_new(unsigned long id,
                                        const struct DbQuery* query,
                                        int timeout_ms, enum DbError* err)
{
    struct PgRequest* req;
    unsigned int nparams = 0;
    unsigned int n;

    if (query->params)
        while (query->params[nparams])
            nparams++;

    if (nparams > DB_MAX_PARAMS) {
        *err = DB_ERR_PARAM;
        return 0;
    }

    if (!(req = (struct PgRequest*)worker_alloc(sizeof(*req)))) {
        *err = DB_ERR_RESOURCE;
        return 0;
    }

    req->pgr_id = id;
    req->pgr_timeout_ms = timeout_ms;
    req->pgr_nparams = nparams;

    if (!(req->pgr_sql = (char*)worker_alloc(strlen(query->sql) + 1))) {
        pg_request_free(req);
        *err = DB_ERR_RESOURCE;
        return 0;
    }
    strcpy(req->pgr_sql, query->sql);

    if (nparams) {
        req->pgr_params =
            (struct PgBound*)worker_alloc(sizeof(struct PgBound) * nparams);

        if (!req->pgr_params) {
            pg_request_free(req);
            *err = DB_ERR_RESOURCE;
            return 0;
        }

        for (n = 0; n < nparams; n++) {
            enum DbError code = pg_bind(&req->pgr_params[n], query->params[n]);

            if (code != DB_OK) {
                /* pg_request_free() walks all nparams entries, and the ones
                 * past this are still zeroed, so there is nothing to unwind by
                 * hand.
                 */
                pg_request_free(req);
                *err = code;
                return 0;
            }
        }
    }

    return req;
}

/* ------------------------------------------------------------------------
 * The queue.  Both threads.
 * ------------------------------------------------------------------------ */

struct PgRequest* pg_pool_take(struct PgPool* pool, struct Worker* worker)
{
    struct PgRequest* req;
    struct timespec until;

    pthread_mutex_lock(&pool->pgp_lock);

    if (!pool->pgp_head && !pool->pgp_stopping && !worker_stopping(worker)) {
        /* A short wait rather than an indefinite one: worker_stop() does not
         * know about this condition variable, so the thread has to come up for
         * air and look at worker_stopping() by itself.  The clock is the
         * default one for a condition variable, which makes this at worst a
         * slightly early or slightly late wake-up -- and the loop above is
         * what decides, not the wait.
         */
        clock_gettime(CLOCK_REALTIME, &until);

        until.tv_nsec += (long)PG_POLL_INTERVAL_MS * 1000000L;
        if (until.tv_nsec >= 1000000000L) {
            until.tv_sec++;
            until.tv_nsec -= 1000000000L;
        }

        pthread_cond_timedwait(&pool->pgp_cond, &pool->pgp_lock, &until);
    }

    if ((req = pool->pgp_head)) {
        pool->pgp_head = req->pgr_next;
        if (!pool->pgp_head)
            pool->pgp_tail = 0;
        pool->pgp_queued--;
        req->pgr_next = 0;
    }

    pthread_mutex_unlock(&pool->pgp_lock);

    return req;
}

const char* pg_pool_dsn(const struct PgPool* pool)
{
    return pool->pgp_dsn;
}

const char* pg_pool_label(const struct PgPool* pool)
{
    return pool->pgp_label;
}

int pg_pool_timeout(const struct PgPool* pool)
{
    return pool->pgp_timeout_ms;
}

const char* pg_pool_search_path(const struct PgPool* pool)
{
    return pool->pgp_search_path;
}

void pg_pool_count(struct PgPool* pool, enum DbError code)
{
    pthread_mutex_lock(&pool->pgp_lock);

    if (code == DB_OK)
        pool->pgp_ok++;
    else {
        pool->pgp_failed++;
        if (code == DB_ERR_TIMEOUT)
            pool->pgp_timeouts++;
    }

    pthread_mutex_unlock(&pool->pgp_lock);
}

void pg_pool_count_connect(struct PgPool* pool, int ok)
{
    pthread_mutex_lock(&pool->pgp_lock);

    if (ok)
        pool->pgp_connects++;
    else
        pool->pgp_connfail++;

    pthread_mutex_unlock(&pool->pgp_lock);
}

/* ------------------------------------------------------------------------
 * Answers: copying out.
 * ------------------------------------------------------------------------ */

/* What a connection thread hands to the main thread. */
struct PgReply {
    unsigned long pgy_id;  /* Handle for db_complete(). */
    json_t* pgy_data;      /* Rows, or NULL.  Owned by this reply. */
    unsigned int pgy_rows; /* Rows returned or affected. */
    enum DbError pgy_code; /* How the query ended. */
};

/* Release a reply.  Main thread; runs on every path.
 * @param[in] task Task carrying the reply.
 */
static void pg_reply_free(struct WorkTask* task)
{
    struct PgReply* reply = (struct PgReply*)task->wt_out;

    if (reply) {
        /* Non-NULL only when the answer never reached db_complete() -- the
         * server was shutting down, say.  Letting it leak would be a leak
         * nobody ever looks for.
         */
        if (reply->pgy_data)
            json_decref(reply->pgy_data);
        worker_free(reply);
        task->wt_out = 0;
    }
}

/* Deliver one answer.  Main thread.
 * @param[in] task Task carrying the reply.
 */
static void pg_reply_done(struct WorkTask* task)
{
    struct PgReply* reply = (struct PgReply*)task->wt_out;
    json_t* data;

    if (!reply)
        return;

    /* db_complete() takes the reference: it releases the value through the
     * driver once the caller's callback has looked at it.
     */
    data = reply->pgy_data;
    reply->pgy_data = 0;

    db_complete(reply->pgy_id, data, reply->pgy_rows, reply->pgy_code,
                pg_error_message(reply->pgy_code));
}

void pg_pool_answer(struct Worker* worker, unsigned long id, json_t* data,
                    unsigned int rows, enum DbError code)
{
    struct WorkTask* task;
    struct PgReply* reply;

    if (!(task = worker_task_new(0, pg_reply_done))) {
        if (data)
            json_decref(data);
        return;
    }

    if (!(reply = (struct PgReply*)worker_alloc(sizeof(*reply)))) {
        worker_task_free(task);
        if (data)
            json_decref(data);
        return;
    }

    reply->pgy_id = id;
    reply->pgy_data = data;
    reply->pgy_rows = rows;
    reply->pgy_code = code;

    task->wt_out = reply;
    task->wt_out_len = sizeof(*reply);
    task->wt_free = pg_reply_free;

    if (!worker_post(worker, task)) {
        /* The reply queue is full or the subsystem is going down.  The caller
         * will be answered when the module unloads; saying so here is the only
         * way anybody finds out why it took that long.
         */
        worker_log("database: could not deliver a result; the reply queue is "
                   "full");
        worker_task_free(task);
    }
}

/* ------------------------------------------------------------------------
 * The pools.  Main thread only.
 * ------------------------------------------------------------------------ */

/* Take every queued request off \a pool and fail it.
 *
 * Main thread, with the connection threads already stopped.
 * @param[in,out] pool Pool to drain.
 */
static void pg_pool_drain(struct PgPool* pool)
{
    struct PgRequest* req;

    pthread_mutex_lock(&pool->pgp_lock);
    req = pool->pgp_head;
    pool->pgp_head = 0;
    pool->pgp_tail = 0;
    pool->pgp_queued = 0;
    pthread_mutex_unlock(&pool->pgp_lock);

    while (req) {
        struct PgRequest* next = req->pgr_next;

        db_complete(req->pgr_id, 0, 0, DB_ERR_UNAVAILABLE,
                    pg_error_message(DB_ERR_UNAVAILABLE));
        pg_request_free(req);
        req = next;
    }
}

/* Stop a pool's threads, drain it and release it.
 * @param[in] pool Pool to release, or NULL.
 */
static void pg_pool_free(struct PgPool* pool)
{
    unsigned int n;

    if (!pool)
        return;

    pthread_mutex_lock(&pool->pgp_lock);
    pool->pgp_stopping = 1;
    pthread_cond_broadcast(&pool->pgp_cond);
    pthread_mutex_unlock(&pool->pgp_lock);

    /* Each stop waits for its thread, which can be in the middle of a query;
     * that is bounded by the timeout, which is bounded by five seconds.  The
     * shutdown path may have stopped them already, in which case this finds
     * a handle that is no longer a worker and does nothing -- which is
     * exactly what worker_stop() documents.
     */
    if (pool->pgp_workers)
        for (n = 0; n < pool->pgp_running; n++)
            if (pool->pgp_workers[n])
                worker_stop(pool->pgp_workers[n]);

    pg_pool_drain(pool);

    if (pool->pgp_conns)
        for (n = 0; n < pool->pgp_size; n++)
            if (pool->pgp_conns[n])
                pg_conn_free(pool->pgp_conns[n]);

    pthread_cond_destroy(&pool->pgp_cond);
    pthread_mutex_destroy(&pool->pgp_lock);

    worker_free(pool->pgp_workers);
    worker_free(pool->pgp_conns);
    worker_free(pool->pgp_dsn);
    worker_free(pool->pgp_search_path);
    worker_free(pool);
}

/* Build a pool for \a role from the configuration, and start its threads.
 *
 * @param[in] role Role to serve.
 * @param[out] err Receives the reason when this returns NULL.
 * @return The pool, running, or NULL.
 */
static struct PgPool* pg_pool_start(enum DbRole role, enum DbError* err)
{
    const char* dsn = db_conf_dsn(role);
    char search_path[300];
    struct PgPool* pool;
    unsigned int size;
    unsigned int n;

    if (!dsn || !*dsn) {
        *err = DB_ERR_CONFIG;
        return 0;
    }

    size = (unsigned int)db_conf_pool(role);
    if (size < 1)
        size = 1;

    if (!(pool = (struct PgPool*)worker_alloc(sizeof(*pool)))) {
        *err = DB_ERR_RESOURCE;
        return 0;
    }

    pool->pgp_role = role;
    pool->pgp_label = pg_role_label[role];
    pool->pgp_timeout_ms = db_conf_timeout();
    pool->pgp_size = size;
    pool->pgp_max_queue = size * PG_QUEUE_PER_CONN;

    pthread_mutex_init(&pool->pgp_lock, 0);
    pthread_cond_init(&pool->pgp_cond, 0);

    pg_search_path(db_conf_schema(), search_path, sizeof(search_path));
    pool->pgp_dsn = (char*)worker_alloc(strlen(dsn) + 1);
    pool->pgp_search_path = (char*)worker_alloc(strlen(search_path) + 1);
    pool->pgp_conns =
        (struct PgConn**)worker_alloc(sizeof(struct PgConn*) * size);
    pool->pgp_workers =
        (struct Worker**)worker_alloc(sizeof(struct Worker*) * size);

    if (!pool->pgp_dsn || !pool->pgp_search_path || !pool->pgp_conns ||
        !pool->pgp_workers) {
        pg_pool_free(pool);
        *err = DB_ERR_RESOURCE;
        return 0;
    }

    strcpy(pool->pgp_dsn, dsn);
    strcpy(pool->pgp_search_path, search_path);

    for (n = 0; n < size; n++) {
        if (!(pool->pgp_conns[n] = pg_conn_new(pool, n))) {
            pg_pool_free(pool);
            *err = DB_ERR_RESOURCE;
            return 0;
        }
    }

    for (n = 0; n < size; n++) {
        pool->pgp_workers[n] = worker_spawn(pg_conn_name(pool->pgp_conns[n]),
                                            pg_conn_main, pool->pgp_conns[n]);

        if (!pool->pgp_workers[n]) {
            /* The worker subsystem is shutting down, or a thread could not be
             * created: this driver is threads or nothing, and there is no
             * degraded mode worth having -- a query on the main thread would
             * stall Services for as long as the database felt like taking.
             */
            pool->pgp_running = n;
            pg_pool_free(pool);
            *err = DB_ERR_UNAVAILABLE;
            return 0;
        }

        pool->pgp_running = n + 1;
    }

    worker_log("database: %s pool started with %u connection%s, %dms timeout",
               pool->pgp_label, size, size == 1 ? "" : "s",
               pool->pgp_timeout_ms);

    return pool;
}

void pg_pools_sync(void)
{
    unsigned int generation = db_conf_generation();
    enum DbRole role;

    if (generation == pg_pools_generation)
        return;

    /* The DSN, the pool size or the timeout may all have moved, and a
     * connection thread reads its pool's copy of them without a lock.  So the
     * pools are not adjusted in place: they are stopped, and the next query
     * builds them again from what the configuration says now.
     */
    for (role = 0; role < DB_ROLE_LAST; role++) {
        if (pg_pools[role]) {
            pg_pool_free(pg_pools[role]);
            pg_pools[role] = 0;
        }
    }

    pg_pools_generation = generation;
    pg_warned_workers = 0;
}

struct PgPool* pg_pools_get(enum DbRole role, enum DbError* err)
{
    pg_pools_sync();

    if (!db_conf()) {
        *err = DB_ERR_CONFIG;
        return 0;
    }

    if (!pg_pools[role]) {
        if (!(pg_pools[role] = pg_pool_start(role, err))) {
            if (*err == DB_ERR_UNAVAILABLE && !pg_warned_workers) {
                pg_warned_workers = 1;
                worker_log("database: no connection threads could be started;"
                           " queries are refused");
            }
            return 0;
        }
    }

    return pg_pools[role];
}

enum DbError pg_pools_submit(struct PgPool* pool, unsigned long id,
                             const struct DbQuery* query)
{
    struct PgRequest* req;
    enum DbError err = DB_ERR_INTERNAL;

    if (!(req = pg_request_new(id, query, pool->pgp_timeout_ms, &err)))
        return err;

    pthread_mutex_lock(&pool->pgp_lock);

    if (pool->pgp_stopping)
        err = DB_ERR_UNAVAILABLE;
    else if (pool->pgp_queued >= pool->pgp_max_queue) {
        pool->pgp_rejected++;
        err = DB_ERR_BUSY;
    }
    else {
        if (pool->pgp_tail)
            pool->pgp_tail->pgr_next = req;
        else
            pool->pgp_head = req;
        pool->pgp_tail = req;
        pool->pgp_queued++;
        pool->pgp_submitted++;
        err = DB_OK;

        pthread_cond_signal(&pool->pgp_cond);
    }

    pthread_mutex_unlock(&pool->pgp_lock);

    if (err != DB_OK)
        pg_request_free(req);

    return err;
}

void pg_pools_stop(void)
{
    enum DbRole role;

    for (role = 0; role < DB_ROLE_LAST; role++) {
        pg_pool_free(pg_pools[role]);
        pg_pools[role] = 0;
    }

    pg_pools_generation = 0;
}

void pg_pools_report(void)
{
    enum DbRole role;

    for (role = 0; role < DB_ROLE_LAST; role++) {
        struct PgPool* pool = pg_pools[role];
        unsigned int queued;
        unsigned int ok;
        unsigned int failed;
        unsigned int timeouts;
        unsigned int rejected;
        unsigned int connects;
        unsigned int connfail;

        if (!pool) {
            worker_log("database: %s pool is not running",
                       pg_role_label[role]);
            continue;
        }

        pthread_mutex_lock(&pool->pgp_lock);
        queued = pool->pgp_queued;
        ok = pool->pgp_ok;
        failed = pool->pgp_failed;
        timeouts = pool->pgp_timeouts;
        rejected = pool->pgp_rejected;
        connects = pool->pgp_connects;
        connfail = pool->pgp_connfail;
        pthread_mutex_unlock(&pool->pgp_lock);

        worker_log("database: %s pool: %u connections, %u queued, %u ok, "
                   "%u failed (%u timed out), %u refused, %u connects "
                   "(%u failed)",
                   pool->pgp_label, pool->pgp_running, queued, ok, failed,
                   timeouts, rejected, connects, connfail);
    }
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
