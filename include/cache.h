/* A key-value cache (Redis), for the core and for modules.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/cache.h).  The same shape as db.h, and for the
 * same reasons: the core is the meeting point between whoever caches and
 * whoever implements the cache (the core's own Redis driver, src/redis/),
 * and holding the calls in flight here is what lets a module be unloaded
 * with some still outstanding.  The core knows nothing about Redis here: it
 * knows there is a store with keys, values and an expiry, and that it may
 * not be there.
 *
 * THE CACHE IS NEVER THE TRUTH.  Every caller reads it first and the
 * database second, and a cache that is missing, empty, stale or broken only
 * ever costs a query.  That is why a missing cache is not an error anybody
 * has to handle specially: with no `redis' block in ircservices.conf,
 * cache_get() returns zero, the callback never runs, and the caller goes to
 * the database exactly as it would on a miss.
 *
 * Nothing waits: a call hands the work to a connection thread (see
 * worker.h) and returns; the answer arrives later, in the main thread.
 * Values are opaque bytes to the core (binary-safe: they may contain NUL);
 * what goes in them is best JSON, because the things worth caching have
 * more than one field.
 *
 * INVALIDATE ON WRITE, DO NOT WAIT FOR THE EXPIRY.  A TTL is the net under
 * whatever is written outside Services, not the mechanism: the store is
 * shared, so deleting a key after a write deletes it for every reader, and
 * the alternative is every reader serving a stale answer until the clock
 * runs out.
 *
 * See docs/readme.database.
 */

#ifndef CACHE_H
#define CACHE_H

#include <sys/types.h>
#include <time.h>

struct Module_;

/* Handle for one call in flight.  Zero is never a valid one. */
typedef unsigned long cache_id_t;

/* Longest key the core will carry, prefix included. */
#define CACHE_KEY_MAX            255
/* Longest value.  Bigger than this belongs in the database. */
#define CACHE_VALUE_MAX          65536
/* Default expiry, in seconds, when a caller asks for none. */
#define CACHE_TTL_DEFAULT        300
/* Ceiling on the expiry: a day. */
#define CACHE_TTL_MAX            86400
/* Default milliseconds a call may take, and the ceiling on that: a cache
 * slower than this is not a cache. */
#define CACHE_TIMEOUT_DEFAULT_MS 200
#define CACHE_TIMEOUT_MAX_MS     2000
/* Connections to the store, by default and at most. */
#define CACHE_DEFAULT_POOL       2
#define CACHE_MAX_POOL           16

/* What went wrong, or CACHE_OK. */
enum CacheError {
    CACHE_OK,              /* The call worked.  A miss is still OK. */
    CACHE_ERR_UNAVAILABLE, /* No driver, or it went away mid-call */
    CACHE_ERR_CONFIG,      /* The redis block is missing or unusable */
    CACHE_ERR_CONNECT,     /* Could not reach the store */
    CACHE_ERR_TIMEOUT,     /* It did not answer in time */
    CACHE_ERR_TOO_BIG,     /* Key or value past the limits above */
    CACHE_ERR_BACKEND,     /* The store refused, or the driver failed */
    CACHE_ERR_LAST
};

/* The answer to one call.  Nothing in it outlives the callback. */
struct CacheResult {
    enum CacheError cres_code; /* CACHE_OK, or what went wrong */
    const char* cres_key;      /* Key that was asked about */
    const char* cres_value;    /* Value, or NULL on a miss or an error */
    size_t cres_len;           /* Its length */
    int cres_hit;              /* Nonzero when the key was there */
    const char* cres_message;  /* Detail, or NULL for cache_strerror() */
};

/* Called in the main thread when a call is answered. */
typedef void (*CacheResultFn)(const struct CacheResult* res, void* user);

/*************************************************************************/

/* Using it.  `mod' is THIS_MODULE (NULL from the core): the module's calls
 * are dropped if it is unloaded before they are answered.  `key' does not
 * include the configured prefix, which the core adds.  `fn' may be NULL to
 * fire and forget.  Each returns the call's handle, or 0 when there is no
 * cache (or the call was refused) -- in which case `fn' is not called and
 * the caller simply goes to the database. */

/* Read a key. */
extern cache_id_t cache_get(struct Module_* mod, const char* key,
                            CacheResultFn fn, void* user);

/* Write a key: `len' bytes of `value' (strlen() if 0), expiring in `ttl'
 * seconds (CACHE_TTL_DEFAULT if 0). */
extern cache_id_t cache_set(struct Module_* mod, const char* key,
                            const char* value, size_t len, int ttl,
                            CacheResultFn fn, void* user);

/* Delete a key: what every writer calls after it has written to the
 * database. */
extern cache_id_t cache_del(struct Module_* mod, const char* key,
                            CacheResultFn fn, void* user);

/* Nonzero if the cache is configured and its driver is up. */
extern int cache_available(void);

/* Name of the driver ("redis"), or "none". */
extern const char* cache_driver_name(void);

/* Text for an error, for a log line. */
extern const char* cache_strerror(enum CacheError code);

/* Instrumentation: calls in flight; calls made; of those, how many failed
 * (a miss is not a failure) and how many found the key. */
extern unsigned int cache_calls_pending(void);
extern unsigned int cache_calls_total(void);
extern unsigned int cache_calls_failed(void);
extern unsigned int cache_calls_hit(void);

/*************************************************************************/

/* The `redis' block.  The core does not use a field of it; it keeps it so
 * that the driver can ask. */

struct CacheConf {
    char* cconf_host;              /* Host to connect to */
    int cconf_port;                /* Port */
    char* cconf_password;          /* AUTH password, or NULL */
    char* cconf_socket;            /* Unix socket, instead of host/port */
    int cconf_database;            /* Which numbered database */
    int cconf_pool;                /* Connections to keep */
    int cconf_timeout_ms;          /* Per-call deadline, clamped */
    char* cconf_prefix;            /* Prepended to every key */
    int cconf_ttl;                 /* Seconds a record stays cached */
    int cconf_negative_ttl;        /* ... and a "no such record" */
    unsigned int cconf_generation; /* Bumped every time this changes */
};

/* The configuration, or NULL if there is no `redis' block. */
extern const struct CacheConf* cache_conf(void);

/*************************************************************************/

/* The driver side.  The core's Redis driver implements these. */

struct CacheDriver {
    const char* cdrv_name;
    /* Start a call, and answer it later with cache_complete(id, ...).  A
     * refusal (anything but CACHE_OK) means cache_complete() must not be
     * called for `id'.  `key' has the prefix applied already. */
    enum CacheError (*cdrv_get)(cache_id_t id, const char* key);
    enum CacheError (*cdrv_set)(cache_id_t id, const char* key,
                                const char* value, size_t len, int ttl);
    enum CacheError (*cdrv_del)(cache_id_t id, const char* key);
    /* Forget a call: the core has stopped caring about the answer
     * (optional). */
    void (*cdrv_cancel)(cache_id_t id);
    /* Notice a changed (or removed) redis block (optional). */
    void (*cdrv_reconfigure)(void);
    /* Stop everything (optional). */
    void (*cdrv_shutdown)(void);
};

/* Register `driver'; one at a time.  Returns nonzero on success. */
extern int cache_register_driver(const struct CacheDriver* driver);

/* Withdraw the driver, failing every call in flight first. */
extern void cache_unregister_driver(void);

/* Deliver the answer to call `id'.  Main thread only; exactly once for
 * every call accepted.  A handle the core no longer holds (it expired, or
 * its module went away) is not an error.  Returns nonzero if the handle
 * was outstanding. */
extern int cache_complete(cache_id_t id, const char* value, size_t len,
                          enum CacheError code, const char* message);

/*************************************************************************/

/* Core-side interface.  Not for modules. */

/* Publish what the `redis' block read (its directive table is
 * cache_directives[] in cache.c, bound by init.c); called after
 * CONFIGURE_SET. */
extern void cache_config_apply(void);
/* Nonzero if the configuration being loaded has a usable redis block
 * (required); errors are reported. */
extern int cache_config_ok(void);
/* Drop a module's pending calls; the module loader calls this. */
extern void cache_drop_module(struct Module_* mod);
/* Release everything, at exit. */
extern void cache_shutdown(void);

/*************************************************************************/

#endif /* CACHE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
