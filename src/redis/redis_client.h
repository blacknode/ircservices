/* A Redis connection owned by one thread, used synchronously.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The cache API (cache.h) is asynchronous and meant for modules.  The
 * entity store (store.h) needs something more direct: the main thread asks
 * Redis before PostgreSQL, with a deadline of a few hundred milliseconds,
 * and the store's own threads fetch and write in batches.  Each of them owns
 * a RedisClient -- never shared between threads -- built from a copy of the
 * `redis' block.
 *
 * A client that fails is marked down for a moment (REDIS_CLIENT_RETRY_MS),
 * and every call during that moment fails at once, without touching the
 * network: when Redis is down, the store goes to PostgreSQL directly rather
 * than waiting out a timeout on every read.
 *
 * No services.h here: this runs in worker threads too.  Values returned
 * are on the system allocator; release them with redis_client_free().
 */

#ifndef REDIS_CLIENT_H
#define REDIS_CLIENT_H

#include <stddef.h>

/* How long a client stays down after a failure. */
#define REDIS_CLIENT_RETRY_MS 1000

/* Where to connect: a copy of the redis block. */
struct RedisClientConf {
    char rcc_host[256];
    char rcc_socket[256];
    char rcc_password[256];
    int rcc_port;
    int rcc_database;
    int rcc_timeout_ms;
};

struct RedisClient;

/* A client for `conf' (copied); the connection is opened on first use. */
extern struct RedisClient* redis_client_new(const struct RedisClientConf* conf);
extern void redis_client_close(struct RedisClient* client);

/* Open the connection now.  Returns nonzero on success. */
extern int redis_client_connect(struct RedisClient* client);

/* GET: 1 and `*value'/`*len' on a hit, 0 on a miss, -1 on an error. */
extern int redis_client_get(struct RedisClient* client, const char* key,
                            char** value, size_t* len);

/* MGET of `n' keys: `values[i]' is NULL for a miss.  Returns 0, or -1 on
 * an error (no value is then returned). */
extern int redis_client_mget(struct RedisClient* client, const char** keys,
                             int n, char** values, size_t* lens);

/* SET with an expiry in seconds (none if `ttl' <= 0).  0 or -1. */
extern int redis_client_set(struct RedisClient* client, const char* key,
                            const char* value, size_t len, int ttl);

/* Several SETs in one round trip (pipelined).  0 or -1. */
extern int redis_client_mset(struct RedisClient* client, const char** keys,
                             const char** values, const size_t* lens, int n,
                             int ttl);

/* DEL.  0 or -1. */
extern int redis_client_del(struct RedisClient* client, const char* key);

/* The last error, for a log line. */
extern const char* redis_client_error(const struct RedisClient* client);

/* Release a value returned above. */
extern void redis_client_free(void* value);

#endif /* REDIS_CLIENT_H */
