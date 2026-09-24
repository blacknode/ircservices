/* The Redis cache driver: what cache.c sees of it.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/redis/redis.c), where it is a module;
 * here it is part of the core, registered by init() when Services start.
 *
 * From then on every cache_get(), cache_set() and cache_del() -- from any
 * module, without any of them linking against hiredis -- lands here, runs
 * on a pooled connection in a worker thread, and comes back as bytes:
 *
 *     redis {
 *         host = "127.0.0.1";         # or: socket = "/run/redis.sock";
 *         port = 6379;
 *         password = "secret";
 *         pool = 2;
 *         timeout = 0.2;
 *         prefix = "services:";
 *     };
 *
 * hiredis is used synchronously on purpose.  An event-driven client would
 * have to be woven into the main loop, and what that would avoid -- the
 * main thread waiting on a socket -- the worker threads already avoid.
 */

#include "redis.h"

#include <stdio.h>
#include <string.h>

/* The queue the connection threads take from. */
static struct RedisPool redis_pool;

/* The connection threads. */
static struct Worker* redis_workers[CACHE_MAX_POOL];
static int redis_worker_count;

/* The configuration the threads were started against. */
static unsigned int redis_generation;

/* Non-zero once the pool has been set up. */
static int redis_started;

/* Stop every connection thread and empty the queue. */
static void redis_stop_pool(void)
{
    int i;

    if (!redis_started)
        return;

    redis_pool_stop(&redis_pool);

    for (i = 0; i < redis_worker_count; i++) {
        if (redis_workers[i])
            worker_stop(redis_workers[i]);
        redis_workers[i] = 0;
    }

    redis_worker_count = 0;

    /* Anything still queued is answered by the core's own deadline; what is
     * dropped here is the request, not the caller's expectation. */
    redis_pool_destroy(&redis_pool);
    redis_started = 0;
}

/* Start the connection threads against the current Redis{} block.
 * @return Non-zero when at least one thread is running.
 */
static int redis_start_pool(void)
{
    const struct CacheConf* conf = cache_conf();
    int wanted;
    int i;

    if (!conf)
        return 0;

    if (redis_started && redis_generation == conf->cconf_generation)
        return redis_worker_count > 0;

    redis_stop_pool();

    redis_pool_init(&redis_pool);
    redis_started = 1;
    redis_generation = conf->cconf_generation;

    snprintf(redis_pool.rp_host, sizeof(redis_pool.rp_host), "%s",
             conf->cconf_host ? conf->cconf_host : "");
    snprintf(redis_pool.rp_socket, sizeof(redis_pool.rp_socket), "%s",
             conf->cconf_socket ? conf->cconf_socket : "");
    snprintf(redis_pool.rp_password, sizeof(redis_pool.rp_password), "%s",
             conf->cconf_password ? conf->cconf_password : "");
    redis_pool.rp_port = conf->cconf_port;
    redis_pool.rp_database = conf->cconf_database;
    redis_pool.rp_timeout_ms = conf->cconf_timeout_ms;

    wanted = conf->cconf_pool;
    if (wanted > CACHE_MAX_POOL)
        wanted = CACHE_MAX_POOL;

    for (i = 0; i < wanted; i++) {
        char name[WORKER_NAMELEN + 1];

        snprintf(name, sizeof(name), "redis-%d", i);
        redis_workers[i] = worker_spawn(name, redis_conn_main, &redis_pool);
        if (!redis_workers[i])
            break;

        redis_worker_count++;
    }

    if (!redis_worker_count) {
        worker_log("cache: no connection threads could be started");
        return 0;
    }

    worker_log("cache: %d connection%s to %s", redis_worker_count,
               redis_worker_count == 1 ? "" : "s",
               redis_pool.rp_socket[0] ? redis_pool.rp_socket
                                       : redis_pool.rp_host);

    return 1;
}

/* Build a request and put it on the queue.
 * @return CACHE_OK when it was queued.
 */
static enum CacheError redis_submit(cache_id_t id, enum RedisOp op,
                                    const char* key, const char* value,
                                    size_t len, int ttl)
{
    struct RedisRequest* req;

    if (!redis_start_pool())
        return CACHE_ERR_CONNECT;

    if (!(req = (struct RedisRequest*)worker_alloc(sizeof(*req))))
        return CACHE_ERR_BACKEND;

    req->rr_id = id;
    req->rr_op = op;
    req->rr_ttl = ttl;

    if (!(req->rr_key = (char*)worker_alloc(strlen(key) + 1))) {
        redis_request_free(req);
        return CACHE_ERR_BACKEND;
    }
    strcpy(req->rr_key, key);

    if (value) {
        if (!(req->rr_value = (char*)worker_alloc(len + 1))) {
            redis_request_free(req);
            return CACHE_ERR_BACKEND;
        }
        memcpy(req->rr_value, value, len);
        req->rr_value[len] = '\0';
        req->rr_len = len;
    }

    if (!redis_pool_push(&redis_pool, req)) {
        redis_request_free(req);
        return CACHE_ERR_BACKEND;
    }

    return CACHE_OK;
}

/* Deliver one answer to the core.  Main thread, from the worker drain.
 * @param[in] task The finished request.
 */
void redis_deliver(struct WorkTask* task)
{
    struct RedisRequest* req = (struct RedisRequest*)task->wt_in;

    if (!req)
        return;

    if (req->rr_code != CACHE_OK && req->rr_error[0])
        worker_log("cache: %s: %s", req->rr_key, req->rr_error);

    cache_complete(req->rr_id, req->rr_hit ? req->rr_reply : 0,
                   req->rr_replylen, req->rr_code,
                   req->rr_error[0] ? req->rr_error : 0);
}

/* Release a task's payload.  The server calls this after wt_done.
 * @param[in] task Task being released.
 */
void redis_task_free(struct WorkTask* task)
{
    redis_request_free((struct RedisRequest*)task->wt_in);
    task->wt_in = 0;
}

/* ------------------------------------------------------------------------
 * The driver.
 * ------------------------------------------------------------------------ */

static enum CacheError redis_driver_get(cache_id_t id, const char* key)
{
    return redis_submit(id, REDIS_OP_GET, key, 0, 0, 0);
}

static enum CacheError redis_driver_set(cache_id_t id, const char* key,
                                        const char* value, size_t len, int ttl)
{
    return redis_submit(id, REDIS_OP_SET, key, value, len, ttl);
}

static enum CacheError redis_driver_del(cache_id_t id, const char* key)
{
    return redis_submit(id, REDIS_OP_DEL, key, 0, 0, 0);
}

/* Forget a call.
 *
 * Nothing to do: a request already handed to a connection thread cannot
 * be recalled, and the answer it eventually posts is delivered to a
 * handle the core no longer holds, which cache_complete() shrugs off.
 */
static void redis_driver_cancel(cache_id_t id)
{
    (void)id;
}

/* Re-read the configuration.
 *
 * The threads are only restarted when the block actually changed (cache.c
 * does not call this otherwise): a REHASH that did not touch it must not
 * drop connections and fail every call in flight for nothing.  A removed
 * block stops them.
 */
static void redis_driver_reconfigure(void)
{
    const struct CacheConf* conf = cache_conf();

    if (!conf) {
        redis_stop_pool();
        return;
    }

    if (redis_started && redis_generation != conf->cconf_generation)
        redis_start_pool();
}

static const struct CacheDriver redis_driver = {
    "redis",          redis_driver_get,    redis_driver_set,
    redis_driver_del, redis_driver_cancel, redis_driver_reconfigure,
    redis_stop_pool};

/* Register the driver.  The threads are not started here: the first call
 * starts them against whatever the configuration turned out to be, so a
 * Services without a redis block never opens a connection.
 */
int redis_driver_init(void)
{
    return cache_register_driver(&redis_driver);
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
