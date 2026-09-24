/* The cache: the driver, the calls in flight, and the redis block.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/cache.c).  See include/cache.h.
 */

#include "cache.h"
#include "conffile.h"
#include "modules.h"
#include "services.h"
#include "timeout.h"

/*************************************************************************/

/* One call the driver has not answered yet. */
typedef struct CacheCall_ CacheCall;
struct CacheCall_ {
    CacheCall* next;
    cache_id_t id;               /* What the driver holds */
    CacheResultFn fn;            /* Callback, or NULL */
    void* user;                  /* The caller's opaque pointer */
    struct Module_* owner;       /* Module that asked, or NULL */
    time_t deadline;             /* When it is given up on */
    char key[CACHE_KEY_MAX + 1]; /* Key, prefix included */
};

/* The registered driver, or NULL. */
static const struct CacheDriver* cache_driver;

/* Calls accepted and not yet answered, and the last handle given out. */
static CacheCall* cache_calls;
static cache_id_t cache_last_id;

/* The published configuration, or NULL. */
static struct CacheConf* cache_config;

/* Statistics. */
static unsigned int cache_stat_pending, cache_stat_total, cache_stat_failed,
    cache_stat_hit;

/* Ceiling on calls in flight, so a driver that accepts and never answers
 * cannot grow the list without bound. */
#define CACHE_PENDING_MAX 8192

static const char* const cache_error_text[CACHE_ERR_LAST] = {
    "no error",
    "no cache is available",
    "the redis block is missing or unusable",
    "could not connect to the cache",
    "the cache did not answer in time",
    "the key or value is too large",
    "the cache driver failed",
};

const char* cache_strerror(enum CacheError code)
{
    if ((int)code < 0 || code >= CACHE_ERR_LAST)
        return "unknown cache error";
    return cache_error_text[code];
}

/*************************************************************************/
/**************************** Calls in flight ****************************/
/*************************************************************************/

/* Timer over the earliest deadline, armed only while something is asked,
 * so that Services with no cache pay for none of it. */
static Timeout* cache_timer;

static void cache_timeout(Timeout* t);
static int cache_expire(time_t now);

static CacheCall* cache_find(cache_id_t id)
{
    CacheCall* call;

    for (call = cache_calls; call; call = call->next) {
        if (call->id == id)
            return call;
    }
    return NULL;
}

/* Make sure the timer will fire by the earliest deadline. */
static void cache_arm(void)
{
    CacheCall* call;
    time_t earliest = 0, now;

    if (cache_timer)
        return;
    for (call = cache_calls; call; call = call->next) {
        if (!earliest || call->deadline < earliest)
            earliest = call->deadline;
    }
    if (!earliest)
        return;
    now = time(NULL);
    cache_timer =
        add_timeout_ms(earliest > now ? (uint32)(earliest - now) * 1000 : 0,
                       cache_timeout, 0);
}

/* Fail whatever has waited too long, and set the timer for the rest. */
static void cache_timeout(Timeout* t)
{
    cache_timer = NULL;
    cache_expire(time(NULL));
    cache_arm();
}

/* A handle that is neither zero nor already in flight. */
static cache_id_t cache_new_id(void)
{
    do {
        ++cache_last_id;
    } while (cache_last_id == 0 || cache_find(cache_last_id));
    return cache_last_id;
}

/* Unlink and free one call.  Does not call anything back. */
static void cache_free(CacheCall* call)
{
    CacheCall** call_p;

    for (call_p = &cache_calls; *call_p; call_p = &(*call_p)->next) {
        if (*call_p == call) {
            *call_p = call->next;
            if (cache_stat_pending)
                cache_stat_pending--;
            break;
        }
    }
    free(call);
}

/* Take a call off the list and answer it.  Off the list before the
 * callback runs: a callback that asks something else of the cache must not
 * find this entry still linked. */
static void cache_answer(CacheCall* call, const char* value, size_t len,
                         enum CacheError code, const char* message)
{
    struct CacheResult res;
    CacheResultFn fn = call->fn;
    void* user = call->user;
    char key[CACHE_KEY_MAX + 1];

    strbcpy(key, call->key);
    cache_free(call);
    if (code != CACHE_OK)
        cache_stat_failed++;
    else if (value)
        cache_stat_hit++;
    if (!fn)
        return;
    memset(&res, 0, sizeof(res));
    res.cres_code = code;
    res.cres_key = key;
    res.cres_value = value;
    res.cres_len = len;
    res.cres_hit = (code == CACHE_OK && value != NULL);
    res.cres_message = message;
    (*fn)(&res, user);
}

/* Fail every call whose deadline has passed, one at a time, restarting the
 * walk after each: a callback can ask for something else. */
static int cache_expire(time_t now)
{
    CacheCall* call;
    int expired = 0;

    for (;;) {
        for (call = cache_calls; call; call = call->next) {
            if (call->deadline <= now)
                break;
        }
        if (!call)
            break;
        log("cache: %s did not answer %s in time", cache_driver_name(),
            call->key);
        if (cache_driver && cache_driver->cdrv_cancel)
            (*cache_driver->cdrv_cancel)(call->id);
        cache_answer(call, NULL, 0, CACHE_ERR_TIMEOUT, NULL);
        expired++;
    }
    return expired;
}

/*************************************************************************/
/******************************** The driver *****************************/
/*************************************************************************/

int cache_register_driver(const struct CacheDriver* driver)
{
    if (!driver || !driver->cdrv_name || !driver->cdrv_get ||
        !driver->cdrv_set || !driver->cdrv_del) {
        log("cache: refusing an incomplete driver");
        return 0;
    }
    if (cache_driver) {
        log("cache: refusing driver %s: %s is already registered",
            driver->cdrv_name, cache_driver->cdrv_name);
        return 0;
    }
    cache_driver = driver;
    log_debug(1, "cache: driver %s registered", driver->cdrv_name);
    return 1;
}

void cache_unregister_driver(void)
{
    const struct CacheDriver* driver = cache_driver;

    if (!driver)
        return;
    /* The register goes first, so a callback that asks for something on
     * its way out is refused rather than reaching a driver that is
     * stopping. */
    cache_driver = NULL;
    while (cache_calls)
        cache_answer(cache_calls, NULL, 0, CACHE_ERR_UNAVAILABLE, NULL);
    if (driver->cdrv_shutdown)
        (*driver->cdrv_shutdown)();
}

int cache_available(void)
{
    return cache_driver != NULL && cache_config != NULL;
}

const char* cache_driver_name(void)
{
    return cache_driver ? cache_driver->cdrv_name : "none";
}

void cache_drop_module(struct Module_* mod)
{
    CacheCall *call, *next;

    if (!mod)
        return;
    for (call = cache_calls; call; call = next) {
        next = call->next;
        if (call->owner != mod)
            continue;
        if (cache_driver && cache_driver->cdrv_cancel)
            (*cache_driver->cdrv_cancel)(call->id);
        cache_free(call);
    }
}

/*************************************************************************/
/******************************** Using it *******************************/
/*************************************************************************/

/* Start a call, with the key prefixed and the deadline set.  Returns the
 * new entry, or NULL if the cache cannot be asked. */
static CacheCall* cache_begin(struct Module_* mod, const char* key,
                              CacheResultFn fn, void* user)
{
    CacheCall* call;
    const char* prefix;

    if (!cache_available() || !key || !*key)
        return NULL;
    if (cache_stat_pending >= CACHE_PENDING_MAX) {
        log("cache: %s has %u calls outstanding; refusing another",
            cache_driver->cdrv_name, cache_stat_pending);
        return NULL;
    }
    /* The prefix is applied here and nowhere else: a driver that had to
     * remember to do it would be a driver that one day forgot, and two
     * networks sharing a store would read each other's keys. */
    prefix = cache_config->cconf_prefix ? cache_config->cconf_prefix : "";
    if (strlen(prefix) + strlen(key) > CACHE_KEY_MAX)
        return NULL;

    call = scalloc(1, sizeof(*call));
    snprintf(call->key, sizeof(call->key), "%s%s", prefix, key);
    call->id = cache_new_id();
    call->fn = fn;
    call->user = user;
    call->owner = mod;
    call->deadline =
        time(NULL) + (cache_config->cconf_timeout_ms + 999) / 1000 + 1;
    call->next = cache_calls;
    cache_calls = call;
    cache_stat_pending++;
    cache_stat_total++;
    cache_arm();
    return call;
}

/* Give up on a call the driver refused outright. */
static cache_id_t cache_refused(CacheCall* call, enum CacheError code)
{
    cache_answer(call, NULL, 0, code, NULL);
    return 0;
}

cache_id_t cache_get(struct Module_* mod, const char* key, CacheResultFn fn,
                     void* user)
{
    CacheCall* call;
    enum CacheError code;
    cache_id_t id;

    if (!(call = cache_begin(mod, key, fn, user)))
        return 0;
    id = call->id;
    if ((code = (*cache_driver->cdrv_get)(id, call->key)) != CACHE_OK)
        return cache_refused(call, code);
    return id;
}

cache_id_t cache_set(struct Module_* mod, const char* key, const char* value,
                     size_t len, int ttl, CacheResultFn fn, void* user)
{
    CacheCall* call;
    enum CacheError code;
    cache_id_t id;

    if (!value)
        return 0;
    if (!len)
        len = strlen(value);
    if (len > CACHE_VALUE_MAX) {
        log("cache: refusing to cache %s: %lu bytes is past the %d limit", key,
            (unsigned long)len, CACHE_VALUE_MAX);
        return 0;
    }
    if (ttl <= 0)
        ttl = CACHE_TTL_DEFAULT;
    if (ttl > CACHE_TTL_MAX)
        ttl = CACHE_TTL_MAX;
    if (!(call = cache_begin(mod, key, fn, user)))
        return 0;
    id = call->id;
    if ((code = (*cache_driver->cdrv_set)(id, call->key, value, len, ttl)) !=
        CACHE_OK)
        return cache_refused(call, code);
    return id;
}

cache_id_t cache_del(struct Module_* mod, const char* key, CacheResultFn fn,
                     void* user)
{
    CacheCall* call;
    enum CacheError code;
    cache_id_t id;

    if (!(call = cache_begin(mod, key, fn, user)))
        return 0;
    id = call->id;
    if ((code = (*cache_driver->cdrv_del)(id, call->key)) != CACHE_OK)
        return cache_refused(call, code);
    return id;
}

int cache_complete(cache_id_t id, const char* value, size_t len,
                   enum CacheError code, const char* message)
{
    CacheCall* call = cache_find(id);

    if (!call) {
        log_debug(1, "cache: answer for %lu, which is no longer outstanding",
                  (unsigned long)id);
        return 0;
    }
    cache_answer(call, value, len, code, message);
    return 1;
}

unsigned int cache_calls_pending(void)
{
    return cache_stat_pending;
}

unsigned int cache_calls_total(void)
{
    return cache_stat_total;
}

unsigned int cache_calls_failed(void)
{
    return cache_stat_failed;
}

unsigned int cache_calls_hit(void)
{
    return cache_stat_hit;
}

/*************************************************************************/
/***************************** The redis block ***************************/
/*************************************************************************/

/* Values as the directive table reads them. */
static char *cf_host, *cf_password, *cf_socket, *cf_prefix;
static int32 cf_port, cf_database, cf_pool, cf_timeout;
static time_t cf_ttl, cf_negative_ttl;

ConfigDirective cache_directives[] = {
    {"database", {{CD_INT, 0, &cf_database}}},
    {"host", {{CD_STRING, 0, &cf_host}}},
    {"negative_ttl", {{CD_TIME, 0, &cf_negative_ttl}}},
    {"password", {{CD_STRING, 0, &cf_password}}},
    {"pool", {{CD_POSINT, 0, &cf_pool}}},
    {"port", {{CD_PORT, 0, &cf_port}}},
    {"prefix", {{CD_STRING, 0, &cf_prefix}}},
    {"socket", {{CD_STRING, 0, &cf_socket}}},
    {"timeout", {{CD_TIMEMSEC, 0, &cf_timeout}}},
    {"ttl", {{CD_TIME, 0, &cf_ttl}}},
    {NULL}};

static void cache_conf_release(struct CacheConf* conf)
{
    free(conf->cconf_host);
    free(conf->cconf_password);
    free(conf->cconf_socket);
    free(conf->cconf_prefix);
    memset(conf, 0, sizeof(*conf));
}

static int cache_str_differ(const char* a, const char* b)
{
    if (!a || !b)
        return a != b;
    return strcmp(a, b) != 0;
}

/* A string from the directive table, or NULL if unset or empty. */
static char* cache_dup(const char* s)
{
    return s && *s ? sstrdup(s) : NULL;
}

const struct CacheConf* cache_conf(void)
{
    return cache_config;
}

int cache_config_ok(void)
{
    const ConfNode* block = conf_find_block(conf_root(), "redis", NULL);

    if (!block) {
        config_error(conf_filename(), 0,
                     "Required block `redis' missing: Services keep their"
                     " cached data in Redis");
        return 0;
    }
    if (!conf_find_entry(block, "host") && !conf_find_entry(block, "socket")) {
        config_error(block->file, block->line,
                     "redis: host or socket is required");
        return 0;
    }
    return 1;
}

void cache_config_apply(void)
{
    static unsigned int generation;
    struct CacheConf conf;

    if (!conf_find_block(conf_root(), "redis", NULL)) {
        /* Required (the entity store keeps its copy of the data there), so
         * this only happens when the configuration is broken; see
         * cache_config_ok(). */
        if (cache_config) {
            log("cache: the redis block is gone; the cache is no longer"
                " used");
            cache_conf_release(cache_config);
            free(cache_config);
            cache_config = NULL;
            if (cache_driver && cache_driver->cdrv_reconfigure)
                (*cache_driver->cdrv_reconfigure)();
        }
        return;
    }

    memset(&conf, 0, sizeof(conf));
    conf.cconf_host = cache_dup(cf_host);
    conf.cconf_password = cache_dup(cf_password);
    conf.cconf_socket = cache_dup(cf_socket);
    conf.cconf_prefix = cache_dup(cf_prefix);
    conf.cconf_port = cf_port ? cf_port : 6379;
    conf.cconf_database = cf_database;
    conf.cconf_pool = cf_pool ? cf_pool : CACHE_DEFAULT_POOL;
    conf.cconf_ttl = cf_ttl > 0 ? (int)cf_ttl : 0;
    conf.cconf_negative_ttl = cf_negative_ttl > 0 ? (int)cf_negative_ttl : 0;
    if (conf.cconf_pool > CACHE_MAX_POOL) {
        config_error(conf_filename(), 0,
                     "redis: pool of %d exceeds the maximum of %d; using %d",
                     conf.cconf_pool, CACHE_MAX_POOL, CACHE_MAX_POOL);
        conf.cconf_pool = CACHE_MAX_POOL;
    }
    conf.cconf_timeout_ms =
        cf_timeout > 0 ? cf_timeout : CACHE_TIMEOUT_DEFAULT_MS;
    if (conf.cconf_timeout_ms > CACHE_TIMEOUT_MAX_MS) {
        /* Corrected rather than refused, but said out loud: asking the
         * cache first only pays if it answers before the database would. */
        config_error(conf_filename(), 0,
                     "redis: timeout of %dms exceeds the %dms maximum;"
                     " using %dms",
                     conf.cconf_timeout_ms, CACHE_TIMEOUT_MAX_MS,
                     CACHE_TIMEOUT_MAX_MS);
        conf.cconf_timeout_ms = CACHE_TIMEOUT_MAX_MS;
    }
    if (!conf.cconf_host && !conf.cconf_socket) {
        config_error(conf_filename(), 0,
                     "redis: host or socket is required; the cache is not"
                     " used");
        cache_conf_release(&conf);
        return;
    }

    if (cache_config &&
        !cache_str_differ(cache_config->cconf_host, conf.cconf_host) &&
        !cache_str_differ(cache_config->cconf_password, conf.cconf_password) &&
        !cache_str_differ(cache_config->cconf_socket, conf.cconf_socket) &&
        !cache_str_differ(cache_config->cconf_prefix, conf.cconf_prefix) &&
        cache_config->cconf_port == conf.cconf_port &&
        cache_config->cconf_database == conf.cconf_database &&
        cache_config->cconf_pool == conf.cconf_pool &&
        cache_config->cconf_ttl == conf.cconf_ttl &&
        cache_config->cconf_negative_ttl == conf.cconf_negative_ttl &&
        cache_config->cconf_timeout_ms == conf.cconf_timeout_ms) {
        /* Unchanged: a REHASH that did not touch the block must not drop
         * connections and fail every call in flight for nothing. */
        cache_conf_release(&conf);
        return;
    }

    if (!cache_config)
        cache_config = scalloc(1, sizeof(*cache_config));
    else
        cache_conf_release(cache_config);
    *cache_config = conf;
    cache_config->cconf_generation = ++generation;
    if (cache_driver && cache_driver->cdrv_reconfigure)
        (*cache_driver->cdrv_reconfigure)();
}

/*************************************************************************/

void cache_shutdown(void)
{
    cache_unregister_driver();
    if (cache_timer) {
        del_timeout(cache_timer);
        cache_timer = NULL;
    }
    if (cache_config) {
        cache_conf_release(cache_config);
        free(cache_config);
        cache_config = NULL;
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
