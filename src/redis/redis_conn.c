/* One connection to Redis, in a thread of its own.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/redis/redis_conn.c).
 *
 *
 * Everything in this file runs in a worker thread, so it obeys the one
 * rule without exception: no User, no core globals, no smalloc(), no
 * log(), no send_*().  It reads a request off the
 * queue, talks to the store with the synchronous hiredis API -- which is
 * exactly why it is on a thread of its own -- and posts the answer back.
 *
 * The connection is opened lazily and reopened after a failure.  A cache
 * that is down is not an error Services have to survive specially: every
 * call fails, every caller goes to the database, and the next call tries
 * to connect again.
 */

#include "redis.h"

#include <hiredis/hiredis.h>

#include <string.h>

/** What one connection thread keeps to itself. */
struct RedisConn {
    redisContext* rc_ctx; /**< Open connection, or NULL. */
    int rc_failed;        /**< Non-zero after a failure, until reopened. */
};

/** Copy a message into the request, for the log the main thread writes. */
static void redis_set_error(struct RedisRequest* req, const char* text)
{
    size_t len;

    if (!text)
        return;

    len = strlen(text);
    if (len >= sizeof(req->rr_error))
        len = sizeof(req->rr_error) - 1;

    memcpy(req->rr_error, text, len);
    req->rr_error[len] = '\0';
}

/** Close whatever is open. */
static void redis_close(struct RedisConn* conn)
{
    if (conn->rc_ctx) {
        redisFree(conn->rc_ctx);
        conn->rc_ctx = 0;
    }
}

/** Open the connection, authenticate and select the database.
 * @return Non-zero when there is a usable connection.
 */
static int redis_open(struct RedisConn* conn, const struct RedisPool* pool,
                      struct RedisRequest* req)
{
    struct timeval tv;
    redisReply* reply;

    if (conn->rc_ctx)
        return 1;

    tv.tv_sec = pool->rp_timeout_ms / 1000;
    tv.tv_usec = (pool->rp_timeout_ms % 1000) * 1000;

    if (pool->rp_socket[0])
        conn->rc_ctx = redisConnectUnixWithTimeout(pool->rp_socket, tv);
    else
        conn->rc_ctx =
            redisConnectWithTimeout(pool->rp_host, pool->rp_port, tv);

    if (!conn->rc_ctx || conn->rc_ctx->err) {
        redis_set_error(req,
                        conn->rc_ctx ? conn->rc_ctx->errstr : "out of memory");
        redis_close(conn);
        return 0;
    }

    /* Both directions, so a store that accepts the connection and then goes
     * quiet cannot hold a connection thread for ever.
     */
    redisSetTimeout(conn->rc_ctx, tv);

    if (pool->rp_password[0]) {
        reply = (redisReply*)redisCommand(conn->rc_ctx, "AUTH %s",
                                          pool->rp_password);
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            redis_set_error(req, reply ? reply->str : "AUTH failed");
            if (reply)
                freeReplyObject(reply);
            redis_close(conn);
            return 0;
        }
        freeReplyObject(reply);
    }

    if (pool->rp_database) {
        reply = (redisReply*)redisCommand(conn->rc_ctx, "SELECT %d",
                                          pool->rp_database);
        if (!reply || reply->type == REDIS_REPLY_ERROR) {
            redis_set_error(req, reply ? reply->str : "SELECT failed");
            if (reply)
                freeReplyObject(reply);
            redis_close(conn);
            return 0;
        }
        freeReplyObject(reply);
    }

    return 1;
}

/** Run one request on an open connection. */
static void redis_run(struct RedisConn* conn, struct RedisRequest* req)
{
    redisReply* reply = 0;

    switch (req->rr_op) {
        case REDIS_OP_GET:
            reply =
                (redisReply*)redisCommand(conn->rc_ctx, "GET %s", req->rr_key);
            break;

        case REDIS_OP_SET:
            /* %b, not %s: a value is bytes, and JSON with a NUL in a string
             * would be cut short by anything that assumed otherwise. */
            reply = (redisReply*)redisCommand(conn->rc_ctx, "SETEX %s %d %b",
                                              req->rr_key, req->rr_ttl,
                                              req->rr_value, req->rr_len);
            break;

        case REDIS_OP_DEL:
            reply =
                (redisReply*)redisCommand(conn->rc_ctx, "DEL %s", req->rr_key);
            break;
    }

    if (!reply) {
        /* The context carries why, and it is no longer usable either way. */
        redis_set_error(req, conn->rc_ctx->errstr);
        req->rr_code = (conn->rc_ctx->err == REDIS_ERR_TIMEOUT)
                           ? CACHE_ERR_TIMEOUT
                           : CACHE_ERR_CONNECT;
        redis_close(conn);
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        redis_set_error(req, reply->str);
        req->rr_code = CACHE_ERR_BACKEND;
        freeReplyObject(reply);
        return;
    }

    req->rr_code = CACHE_OK;

    /* A GET that found nothing is a miss, which is an answer and not a
     * failure: the caller goes to the database, which is what it would have
     * done anyway. */
    if (req->rr_op == REDIS_OP_GET && reply->type == REDIS_REPLY_STRING) {
        req->rr_reply = (char*)worker_alloc(reply->len + 1);

        if (req->rr_reply) {
            memcpy(req->rr_reply, reply->str, reply->len);
            req->rr_reply[reply->len] = '\0';
            req->rr_replylen = reply->len;
            req->rr_hit = 1;
        }
        else
            req->rr_code = CACHE_ERR_BACKEND;
    }

    freeReplyObject(reply);
}

/** The body of one connection thread.
 * @param[in] worker This thread's worker.
 * @param[in] arg The shared queue.
 */
void redis_conn_main(struct Worker* worker, void* arg)
{
    struct RedisPool* pool = (struct RedisPool*)arg;
    struct RedisConn conn;

    memset(&conn, 0, sizeof(conn));

    while (!worker_stopping(worker)) {
        struct RedisRequest* req = redis_pool_take(pool, worker);
        struct WorkTask* task;

        if (!req)
            continue;

        req->rr_code = CACHE_ERR_CONNECT;

        if (redis_open(&conn, pool, req))
            redis_run(&conn, req);

        /* Back to the main thread.  The task carries the request and nothing
         * else; wt_work is never called, because the work is done. */
        if (!(task = worker_task_new(0, redis_deliver))) {
            redis_request_free(req);
            continue;
        }

        task->wt_in = req;
        /* The request hangs on to allocations of its own, so the default
         * payload release -- one worker_free() of wt_in -- would leak them. */
        task->wt_free = redis_task_free;

        if (!worker_post(worker, task)) {
            /* The reply queue is full, which means the main thread is not
             * draining; dropping the answer is better than blocking here, and
             * the core's own deadline will fail the call. */
            task->wt_in = 0;
            worker_task_free(task);
            redis_request_free(req);
        }
    }

    redis_close(&conn);
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
