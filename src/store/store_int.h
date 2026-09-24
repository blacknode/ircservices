/* The entity store: what the main thread and the store's threads share.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Core-private.  No services.h: store_io.c runs in worker threads.  Every
 * string in these structures is on the system allocator (store_strdup()),
 * built by the main thread and freed by it.
 */

#ifndef STORE_INT_H
#define STORE_INT_H

#include "postgres/pg_save.h" /* postgres.h, and the growable buffers */
#include "redis/redis_client.h"

/*************************************************************************/

/* Where the store's threads connect: copied from the configuration by the
 * main thread, with the generation it was copied at, so that a thread
 * holding a connection built from an older copy reconnects. */
struct StoreConn {
    unsigned int sc_generation;
    char* sc_dsn;           /* PostgreSQL, write side */
    char* sc_search_path;   /* The schema, then public */
    int sc_timeout_ms;      /* Per statement / per batch */
    struct RedisClientConf sc_redis;
    int sc_ttl;             /* Seconds a bundle stays in Redis */
    int sc_negative_ttl;    /* ... and a "no such record" */
    char* sc_token;         /* This copy's claim on the schema, or NULL */
    char* sc_check;         /* Query returning the owning token */
};

extern struct StoreConn* store_conn_copy(const struct StoreConn* conn);
extern void store_conn_free(struct StoreConn* conn);

extern char* store_strdup(const char* s);
extern void* store_calloc(size_t n, size_t size);
extern void store_free(void* p);

/*************************************************************************/

/* One write, handed to the writer: the SQL to run (built by the main
 * thread from the type; each statement takes one parameter, $1, which is
 * either the key or the bundle), and the Redis update to make once
 * PostgreSQL has it. */
struct StoreWrite {
    struct StoreWrite* sw_next;
    unsigned long sw_seq;      /* Order of the write */
    char* sw_ident;            /* "<type>:<key>": coalescing, pending map */
    char* sw_key;              /* The record's key */
    char* sw_bundle;           /* Its bundle, or NULL for a delete */
    int sw_nstmts;
    char** sw_stmts;           /* The statements */
    int* sw_param_bundle;      /* Nonzero: $1 is the bundle; else the key */
    char* sw_redis_key;        /* Key in Redis */

    /* --- set by the writer --- */
    int sw_done_pg;            /* Committed (or skipped as superseded) */
    int sw_done_redis;         /* Redis has it */
    int sw_failed;             /* The database refused it for good */
    char sw_error[160];
};

extern void store_write_free(struct StoreWrite* w);

/* The writer: a dedicated worker thread.  store_writer_start() takes
 * ownership of `conn'; writes are queued with store_writer_push() and come
 * back, done or failed, to `done' in the main thread (through
 * worker_post()); `done' owns them. */
typedef void (*StoreWriteDoneFn)(struct StoreWrite* list);
extern int store_writer_start(struct StoreConn* conn, StoreWriteDoneFn done);
extern void store_writer_reconfigure(struct StoreConn* conn);
extern void store_writer_push(struct StoreWrite* w);
extern unsigned int store_writer_queued(void);
/* Write everything queued, here and now, in the calling thread (for when
 * there are no worker threads: before the fork, after the shutdown). */
extern void store_writer_drain_sync(void);
extern void store_writer_stop(void);

/*************************************************************************/

/* A fetch, for store_prefetch(): a batch of keys of one type, looked up in
 * Redis (MGET), the misses in PostgreSQL (one query for the batch), and
 * the answers copied into Redis.  Run on the worker pool. */
struct StoreFetch {
    struct StoreConn* sf_conn;  /* Its own copy */
    char* sf_sql;               /* Batch load: $1 is a JSON array of keys;
                                 * returns rows (key, bundle) */
    int sf_nkeys;
    char** sf_keys;             /* Keys, normalized */
    char** sf_redis_keys;       /* The same, as Redis keys */

    /* --- set by the worker --- */
    char** sf_bundles;          /* Bundle, or NULL if there is no record */
    int* sf_found;              /* Nonzero if sf_bundles[i] is an answer */
    int sf_failed;              /* PostgreSQL could not be asked */
};

extern void store_fetch_run(struct StoreFetch* f);
extern void store_fetch_free(struct StoreFetch* f);

/* The value Redis holds for a record that does not exist. */
#define STORE_NEGATIVE "!"

#endif /* STORE_INT_H */
