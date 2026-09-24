/* The request queue, shared by the main thread and the connections.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/redis/redis_pool.c).
 *
 *
 * The only thing in the driver both threads touch.  Everything else
 * belongs to one or the other: the request is built in the main thread,
 * handed over through this queue, filled in by a connection thread, and
 * handed back through worker_post().
 */

#include "redis.h"

#include <string.h>
#include <time.h>

/** Set up an empty queue. */
void redis_pool_init(struct RedisPool* pool)
{
    memset(pool, 0, sizeof(*pool));
    pthread_mutex_init(&pool->rp_lock, 0);
    pthread_cond_init(&pool->rp_cond, 0);
}

/** Tear one down.  Every thread that used it must have stopped. */
void redis_pool_destroy(struct RedisPool* pool)
{
    redis_pool_flush(pool);
    pthread_cond_destroy(&pool->rp_cond);
    pthread_mutex_destroy(&pool->rp_lock);
}

/** Release a request and everything hanging off it. */
void redis_request_free(struct RedisRequest* req)
{
    if (!req)
        return;

    worker_free(req->rr_key);
    worker_free(req->rr_value);
    worker_free(req->rr_reply);
    worker_free(req);
}

/** Put a request on the queue and wake a connection.
 * @return Non-zero when it was queued.
 */
int redis_pool_push(struct RedisPool* pool, struct RedisRequest* req)
{
    int ok = 0;

    pthread_mutex_lock(&pool->rp_lock);

    if (!pool->rp_stopping && pool->rp_queued < REDIS_QUEUE_MAX) {
        if (pool->rp_tail)
            pool->rp_tail->rr_next = req;
        else
            pool->rp_head = req;

        pool->rp_tail = req;
        pool->rp_queued++;
        ok = 1;

        pthread_cond_signal(&pool->rp_cond);
    }

    pthread_mutex_unlock(&pool->rp_lock);

    return ok;
}

/** Take the next request, waiting a little if there is none.
 *
 * A short wait rather than an indefinite one: worker_stop() does not know
 * about this condition variable, so the thread has to come up for air and
 * look at worker_stopping() itself.
 *
 * @param[in,out] pool The queue.
 * @param[in] worker The calling connection's worker.
 * @return A request, or NULL if none arrived before the wait ran out.
 */
struct RedisRequest* redis_pool_take(struct RedisPool* pool,
                                     struct Worker* worker)
{
    struct RedisRequest* req;
    struct timespec until;

    pthread_mutex_lock(&pool->rp_lock);

    if (!pool->rp_head && !pool->rp_stopping && !worker_stopping(worker)) {
        clock_gettime(CLOCK_REALTIME, &until);

        until.tv_nsec += (long)REDIS_POLL_INTERVAL_MS * 1000000L;
        if (until.tv_nsec >= 1000000000L) {
            until.tv_sec++;
            until.tv_nsec -= 1000000000L;
        }

        pthread_cond_timedwait(&pool->rp_cond, &pool->rp_lock, &until);
    }

    if ((req = pool->rp_head)) {
        pool->rp_head = req->rr_next;
        if (!pool->rp_head)
            pool->rp_tail = 0;
        pool->rp_queued--;
        req->rr_next = 0;
    }

    pthread_mutex_unlock(&pool->rp_lock);

    return req;
}

/** Stop accepting requests and wake every waiting connection. */
void redis_pool_stop(struct RedisPool* pool)
{
    pthread_mutex_lock(&pool->rp_lock);
    pool->rp_stopping = 1;
    pthread_cond_broadcast(&pool->rp_cond);
    pthread_mutex_unlock(&pool->rp_lock);
}

/** Throw away everything still queued. */
void redis_pool_flush(struct RedisPool* pool)
{
    struct RedisRequest* req;
    struct RedisRequest* next;

    pthread_mutex_lock(&pool->rp_lock);
    req = pool->rp_head;
    pool->rp_head = pool->rp_tail = 0;
    pool->rp_queued = 0;
    pthread_mutex_unlock(&pool->rp_lock);

    for (; req; req = next) {
        next = req->rr_next;
        redis_request_free(req);
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
