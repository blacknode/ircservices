/* Private to the Redis cache driver.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (modules/workers/redis/redis.h).  Like postgres.h, this
 * header does not include services.h: redis_conn.c and redis_pool.c run in
 * worker threads.
 */

#ifndef REDIS_H
#define REDIS_H

#include "cache.h"
#include "worker.h"

#include <pthread.h>

/* What one request is. */
enum RedisOp { REDIS_OP_GET, REDIS_OP_SET, REDIS_OP_DEL };

/* One request, on its way to a connection thread and back.  Allocated with
 * worker_alloc(): it crosses into another thread. */
struct RedisRequest {
    struct RedisRequest* rr_next; /* Next on whichever queue it is on */
    cache_id_t rr_id;             /* Handle to answer with */
    enum RedisOp rr_op;           /* What to do */
    char* rr_key;                 /* Key, prefix already applied */
    char* rr_value;               /* Value for a SET */
    size_t rr_len;                /* Its length */
    int rr_ttl;                   /* Expiry for a SET, in seconds */

    /* --- filled in by the connection thread --- */
    enum CacheError rr_code; /* What happened */
    int rr_hit;              /* Nonzero when GET found the key */
    char* rr_reply;          /* Bytes read, or NULL */
    size_t rr_replylen;      /* Their length */
    char rr_error[128];      /* Detail, or "" */
};

/* The queue every connection thread takes from. */
struct RedisPool {
    pthread_mutex_t rp_lock;
    pthread_cond_t rp_cond;
    struct RedisRequest* rp_head;
    struct RedisRequest* rp_tail;
    unsigned int rp_queued;
    int rp_stopping;

    /* Read by the connection threads; written once, before they start. */
    char rp_host[256];
    char rp_socket[256];
    char rp_password[256];
    int rp_port;
    int rp_database;
    int rp_timeout_ms;
};

/* How long a connection thread waits before looking at worker_stopping(). */
#define REDIS_POLL_INTERVAL_MS 200

/* Most requests that may be queued before the driver refuses. */
#define REDIS_QUEUE_MAX 4096

/* redis_pool.c */
extern void redis_pool_init(struct RedisPool* pool);
extern void redis_pool_destroy(struct RedisPool* pool);
extern void redis_request_free(struct RedisRequest* req);
extern int redis_pool_push(struct RedisPool* pool, struct RedisRequest* req);
extern struct RedisRequest* redis_pool_take(struct RedisPool* pool,
                                            struct Worker* worker);
extern void redis_pool_stop(struct RedisPool* pool);
extern void redis_pool_flush(struct RedisPool* pool);

/* redis.c; both run in the main thread. */
extern void redis_deliver(struct WorkTask* task);
extern void redis_task_free(struct WorkTask* task);

/* redis.c: register the driver with cache.c.  Main thread, at start-up. */
extern int redis_driver_init(void);

/* redis_conn.c */
extern void redis_conn_main(struct Worker* worker, void* arg);

#endif /* REDIS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
