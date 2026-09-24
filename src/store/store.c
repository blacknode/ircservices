/* The entity store: the working set, reads, and the flow of writes.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * See include/store.h for what this is.  This file is the main thread's
 * half: the working set of each type (records in use, found by key and by
 * address), the pending writes (found by type and key), the synchronous
 * read path, the sweep that notices changes in records that stay pinned,
 * and the glue to the writer thread and the background fetches, which are
 * store_io.c.
 */

/* System, libpq and jansson headers before services.h, whose memory.h may
 * redefine malloc() and free() as macros. */
#include "store_int.h"

#include "services.h"
#include "cache.h"
#include "databases.h"
#include "db.h"
#include "encrypt.h"
#include "modules.h"
#include "store.h"
#include "timeout.h"

/*************************************************************************/

/* How often the sweep runs, and how long it takes to get round every
 * pinned record once. */
#define SWEEP_INTERVAL_MS 1000
#define SWEEP_ROUND_SECS  30

/* How often writes that reached PostgreSQL but not Redis are retried. */
#define REDIS_RETRY_SECS 5

/* Keys per background fetch. */
#define FETCH_BATCH 500

/* Default expiries in Redis (overridden by the redis block). */
#define DEFAULT_TTL          86400
#define DEFAULT_NEGATIVE_TTL 600

/*************************************************************************/
/***************************** Hash tables *******************************/
/*************************************************************************/

static uint32 hash_string(const char* s)
{
    uint32 h = 2166136261u;

    while (*s) {
        h ^= (unsigned char)*s++;
        h *= 16777619u;
    }
    return h;
}

static uint32 hash_pointer(const void* p)
{
    uintptr_t v = (uintptr_t)p;

    v ^= v >> 17;
    v *= 0xed5ad4bbu;
    v ^= v >> 11;
    return (uint32)v;
}

static uint64 hash_bytes(const char* s, size_t len)
{
    uint64 h = 14695981039346656037ULL;
    size_t i;

    for (i = 0; i < len; i++) {
        h ^= (unsigned char)s[i];
        h *= 1099511628211ULL;
    }
    return h;
}

/*************************************************************************/
/************************ Types and their records ************************/
/*************************************************************************/

typedef struct Entry_ Entry;
struct Entry_ {
    Entry* knext;        /* Next in its key bucket */
    Entry* pnext;        /* Next in its address bucket */
    Entry *next, *prev;  /* Every entry of the type */
    Entry *inext, *iprev;/* The idle list, while unpinned */
    int idle;            /* On the idle list */
    StoreType* type;
    char* key;
    void* record;
    int pins;
    int hashed;          /* `hash' is valid */
    uint64 hash;         /* Of the bundle last written or loaded */
};

struct StoreTypeState_ {
    Entry** kbuckets;
    Entry** pbuckets;
    uint32 nbuckets;
    uint32 count;
    Entry* all;
    Entry* sweep;         /* Where the sweep goes on from */
    char* load_sql;       /* $1 = key; returns one row (bundle) */
    char* batch_sql;      /* $1 = JSON array of keys; (key, missing, bundle) */
    int nwrite;           /* Statements of a write */
    char** write_sql;
    int* write_bundle;    /* Nonzero: the statement takes the bundle */
    int ndelete;          /* Statements of a delete (all take the key) */
    char** delete_sql;
    /* Statistics. */
    unsigned long st_gets, st_mem, st_pending, st_redis, st_pg, st_missing;
    unsigned long st_writes, st_deletes, st_prefetched;
};

/* The registered types. */
typedef struct TypeNode_ {
    struct TypeNode_* next;
    StoreType* type;
} TypeNode;
static TypeNode* types;

/*************************************************************************/

/* The main thread's own Redis connection, and the configuration handed to
 * the store's threads. */
static struct RedisClient* main_redis;
static struct StoreConn* conn_template;
static unsigned int conn_generation;
static int store_up;

/* Records whose last pin went in this pass of the main loop: released by
 * store_collect() at the end of it, so that code which still looks at a
 * record just after putting it -- which was harmless when every record
 * lived in memory for ever -- keeps working. */
static Entry* idle_list;

/* Timers. */
static Timeout* sweep_timer;
static Timeout* retry_timer;

/*************************************************************************/

static void entry_rehash(struct StoreTypeState_* st, uint32 nbuckets)
{
    Entry **kb = scalloc(nbuckets, sizeof(*kb));
    Entry **pb = scalloc(nbuckets, sizeof(*pb));
    Entry* e;

    for (e = st->all; e; e = e->next) {
        uint32 k = hash_string(e->key) & (nbuckets - 1);
        uint32 p = hash_pointer(e->record) & (nbuckets - 1);
        e->knext = kb[k];
        kb[k] = e;
        e->pnext = pb[p];
        pb[p] = e;
    }
    free(st->kbuckets);
    free(st->pbuckets);
    st->kbuckets = kb;
    st->pbuckets = pb;
    st->nbuckets = nbuckets;
}

static Entry* entry_by_key(struct StoreTypeState_* st, const char* key)
{
    Entry* e;

    for (e = st->kbuckets[hash_string(key) & (st->nbuckets - 1)]; e;
         e = e->knext) {
        if (strcmp(e->key, key) == 0)
            return e;
    }
    return NULL;
}

static Entry* entry_by_record(struct StoreTypeState_* st, const void* record)
{
    Entry* e;

    for (e = st->pbuckets[hash_pointer(record) & (st->nbuckets - 1)]; e;
         e = e->pnext) {
        if (e->record == record)
            return e;
    }
    return NULL;
}

static Entry* entry_add(struct StoreTypeState_* st, const char* key,
                        void* record)
{
    Entry* e = scalloc(1, sizeof(*e));
    uint32 k, p;

    e->key = sstrdup(key);
    e->record = record;
    e->type = NULL; /* set by the caller that knows it */
    /* Grow first: the rehash goes through `all', which must not have the
     * new entry yet (it would be linked into its bucket twice). */
    if (++st->count > st->nbuckets)
        entry_rehash(st, st->nbuckets * 2);
    e->next = st->all;
    if (st->all)
        st->all->prev = e;
    st->all = e;
    k = hash_string(key) & (st->nbuckets - 1);
    p = hash_pointer(record) & (st->nbuckets - 1);
    e->knext = st->kbuckets[k];
    st->kbuckets[k] = e;
    e->pnext = st->pbuckets[p];
    st->pbuckets[p] = e;
    return e;
}

static void entry_remove(struct StoreTypeState_* st, Entry* e)
{
    Entry** pp;

    for (pp = &st->kbuckets[hash_string(e->key) & (st->nbuckets - 1)]; *pp;
         pp = &(*pp)->knext) {
        if (*pp == e) {
            *pp = e->knext;
            break;
        }
    }
    for (pp = &st->pbuckets[hash_pointer(e->record) & (st->nbuckets - 1)];
         *pp; pp = &(*pp)->pnext) {
        if (*pp == e) {
            *pp = e->pnext;
            break;
        }
    }
    if (st->sweep == e)
        st->sweep = e->next;
    if (e->idle) {
        if (e->iprev)
            e->iprev->inext = e->inext;
        else
            idle_list = e->inext;
        if (e->inext)
            e->inext->iprev = e->iprev;
    }
    if (e->prev)
        e->prev->next = e->next;
    else
        st->all = e->next;
    if (e->next)
        e->next->prev = e->prev;
    st->count--;
    free(e->key);
    free(e);
}

/*************************************************************************/
/***************************** Pending writes ****************************/
/*************************************************************************/

/* A write handed to the writer and not yet in both PostgreSQL and Redis:
 * reads of the record are served from here meanwhile. */
typedef struct Pending_ Pending;
struct Pending_ {
    Pending* next;
    char* ident;          /* "<type>:<key>" */
    char* redis_key;
    unsigned long seq;    /* Of the newest write */
    char* bundle;         /* Newest bundle, NULL if deleted */
    int pg_done;          /* The newest write is in PostgreSQL */
};

#define PENDING_BUCKETS 4096
static Pending* pending[PENDING_BUCKETS];
static int pending_count;
static unsigned long write_seq;

static Pending* pending_find(const char* ident)
{
    Pending* p;

    for (p = pending[hash_string(ident) % PENDING_BUCKETS]; p; p = p->next) {
        if (strcmp(p->ident, ident) == 0)
            return p;
    }
    return NULL;
}

static void pending_remove(Pending* p)
{
    Pending** pp = &pending[hash_string(p->ident) % PENDING_BUCKETS];

    for (; *pp; pp = &(*pp)->next) {
        if (*pp == p) {
            *pp = p->next;
            break;
        }
    }
    pending_count--;
    free(p->ident);
    free(p->redis_key);
    free(p->bundle);
    free(p);
}

/*************************************************************************/
/******************************** Helpers ********************************/
/*************************************************************************/

static void make_ident(const StoreType* type, const char* key, char* buf,
                       size_t size)
{
    snprintf(buf, size, "%s:%s", type->name, key);
}

static void make_redis_key(const StoreType* type, const char* key, char* buf,
                           size_t size)
{
    const struct CacheConf* conf = cache_conf();

    snprintf(buf, size, "%ssvc:%s:%s",
             conf && conf->cconf_prefix ? conf->cconf_prefix : "",
             type->name, key);
}

static void normalize(const StoreType* type, const char* key, char* buf,
                      size_t size)
{
    if (type->normalize)
        (*type->normalize)(key, buf, size);
    else
        strscpy(buf, key, size);
}

/* The canonical text of a bundle, and its hash.  Canonical: keys sorted,
 * compact, so that two encodings of the same record are the same text. */
static char* bundle_dump(json_t* bundle)
{
    return json_dumps(bundle, JSON_COMPACT | JSON_SORT_KEYS);
}

/* Encode `record' and return its canonical text (system allocator), and
 * its hash in `*hash'. */
static char* record_dump(StoreType* type, const void* record, uint64* hash)
{
    json_t* bundle = (*type->encode)(record);
    char* text;

    if (!bundle)
        return NULL;
    text = bundle_dump(bundle);
    json_decref(bundle);
    if (text)
        *hash = hash_bytes(text, strlen(text));
    return text;
}

/* A new record from the text of a bundle (from Redis or a pending write),
 * or NULL. */
static void* record_from_text(StoreType* type, const char* text)
{
    json_error_t err;
    json_t* bundle = json_loads(text, 0, &err);
    void* record;

    if (!bundle) {
        log("store: %s: undecodable bundle (%s)", type->name, err.text);
        return NULL;
    }
    record = (*type->decode)(bundle);
    json_decref(bundle);
    return record;
}

/* Put a freshly loaded record in the working set, pinned once, with its
 * baseline: the hash of its own encoding, so that only a real change is
 * ever written. */
static Entry* adopt(StoreType* type, const char* key, void* record)
{
    Entry* e = entry_add(type->state, key, record);
    char* text = record_dump(type, record, &e->hash);

    e->hashed = text != NULL;
    store_free(text);
    e->pins = 1;
    return e;
}

/*************************************************************************/
/********************************* Writes ********************************/
/*************************************************************************/

static struct StoreWrite* write_new(StoreType* type, const char* key,
                                    char* bundle_text)
{
    struct StoreTypeState_* st = type->state;
    struct StoreWrite* w = store_calloc(1, sizeof(*w));
    char buf[STORE_KEY_MAX + STORE_NAME_MAX + 64];
    int i, n = bundle_text ? st->nwrite : st->ndelete;

    w->sw_seq = ++write_seq;
    make_ident(type, key, buf, sizeof(buf));
    w->sw_ident = store_strdup(buf);
    w->sw_key = store_strdup(key);
    w->sw_bundle = bundle_text;
    make_redis_key(type, key, buf, sizeof(buf));
    w->sw_redis_key = store_strdup(buf);
    w->sw_nstmts = n;
    w->sw_stmts = store_calloc(n, sizeof(*w->sw_stmts));
    w->sw_param_bundle = store_calloc(n, sizeof(*w->sw_param_bundle));
    for (i = 0; i < n; i++) {
        w->sw_stmts[i] = store_strdup(bundle_text ? st->write_sql[i]
                                                  : st->delete_sql[i]);
        w->sw_param_bundle[i] = bundle_text ? st->write_bundle[i] : 0;
    }
    return w;
}

/* Queue a write of `key' (`text' is the new bundle, taken over; NULL for a
 * delete), and make it the pending copy that reads see. */
static void write_queue(StoreType* type, const char* key, char* text)
{
    struct StoreWrite* w = write_new(type, key, text);
    Pending* p = pending_find(w->sw_ident);

    if (!p) {
        uint32 b = hash_string(w->sw_ident) % PENDING_BUCKETS;
        p = scalloc(1, sizeof(*p));
        p->ident = sstrdup(w->sw_ident);
        p->redis_key = sstrdup(w->sw_redis_key);
        p->next = pending[b];
        pending[b] = p;
        pending_count++;
    }
    free(p->bundle);
    p->bundle = text ? sstrdup(text) : NULL;
    p->seq = w->sw_seq;
    p->pg_done = 0;
    if (text)
        type->state->st_writes++;
    else
        type->state->st_deletes++;
    store_writer_push(w);
}

/* Write `e' if it changed since it was last written or loaded. */
static void entry_flush(StoreType* type, Entry* e)
{
    uint64 hash;
    char* text = record_dump(type, e->record, &hash);

    if (!text) {
        log("store: %s %s: cannot encode the record; not written",
            type->name, e->key);
        return;
    }
    if ((e->hashed && e->hash == hash) || readonly) {
        /* Unchanged -- or changed in a copy that must not write. */
        e->hash = hash;
        e->hashed = 1;
        store_free(text);
        return;
    }
    e->hash = hash;
    e->hashed = 1;
    write_queue(type, e->key, text);
}

/* The writer is done with a batch.  Main thread. */
static void writes_done(struct StoreWrite* list)
{
    while (list) {
        struct StoreWrite* w = list;
        Pending* p = pending_find(w->sw_ident);

        list = w->sw_next;
        if (w->sw_failed)
            log("store: %s could not be written: %s -- the change is lost",
                w->sw_ident, w->sw_error);
        if (p && p->seq == w->sw_seq) {
            if (w->sw_failed || (w->sw_done_pg && w->sw_done_redis))
                pending_remove(p);
            else if (w->sw_done_pg)
                p->pg_done = 1; /* Redis still to go: see redis_retry() */
        }
        store_write_free(w);
    }
}

/* Writes that reached PostgreSQL but not Redis: try Redis again, and once
 * it answers, clear the stale marks the writer left in the database. */
static void redis_retry(Timeout* t)
{
    struct DbParam param = {DB_TYPE_TEXT, NULL, DB_FORMAT_TEXT};
    struct DbParam* params[] = {&param, NULL};
    struct DbQuery clear = {"delete from store_stale where redis_key = $1",
                            params};
    int i, fixed = 0, tried = 0;

    for (i = 0; i < PENDING_BUCKETS; i++) {
        Pending *p, *next;
        for (p = pending[i]; p; p = next) {
            int res;
            next = p->next;
            if (!p->pg_done)
                continue;
            if (tried++ > 1000)
                return; /* the rest next time */
            if (p->bundle)
                res = redis_client_set(main_redis, p->redis_key, p->bundle,
                                       strlen(p->bundle),
                                       conn_template->sc_ttl);
            else
                res = redis_client_set(main_redis, p->redis_key,
                                       STORE_NEGATIVE, strlen(STORE_NEGATIVE),
                                       conn_template->sc_negative_ttl);
            if (res < 0)
                return; /* still down */
            param.value = p->redis_key;
            db_exec(NULL, &clear, NULL, NULL);
            pending_remove(p);
            fixed++;
        }
    }
    if (fixed)
        log("store: %d cached record%s brought up to date", fixed,
            fixed == 1 ? "" : "s");
}

/*************************************************************************/
/********************************* Reads *********************************/
/*************************************************************************/

/* Keys known to be missing, during a prefetch callback. */
static char** negative_hints;
static int negative_hints_count;

static int negative_hinted(const char* ident)
{
    int i;

    for (i = 0; i < negative_hints_count; i++) {
        if (strcmp(negative_hints[i], ident) == 0)
            return 1;
    }
    return 0;
}

/* Load `key' from PostgreSQL, synchronously, and copy the answer into
 * Redis.  Returns the record (not in the working set), or NULL: missing
 * (`*missing' set) or unreadable. */
static void* load_from_db(StoreType* type, const char* key,
                          const char* redis_key, int* missing)
{
    struct DbParam param = {DB_TYPE_TEXT, key, DB_FORMAT_TEXT};
    struct DbParam* params[] = {&param, NULL};
    struct DbQuery query = {type->state->load_sql, params};
    struct DbResult res;
    json_t *row, *bundle, *main;
    void* record = NULL;
    char* text;

    *missing = 0;
    if (db_query_sync(NULL, &query, &res) != DB_OK) {
        log("store: cannot read %s %s: %s", type->name, key,
            res.err.dberr_message);
        db_result_free(&res);
        return NULL;
    }
    row = json_array_get((json_t*)res.data, 0);
    bundle = row ? json_object_get(row, "bundle") : NULL;
    main = bundle ? json_object_get(bundle, "main") : NULL;
    if (!main || json_is_null(main)) {
        *missing = 1;
        redis_client_set(main_redis, redis_key, STORE_NEGATIVE,
                         strlen(STORE_NEGATIVE),
                         conn_template->sc_negative_ttl);
    }
    else {
        record = (*type->decode)(bundle);
        if (record && (text = bundle_dump(bundle)) != NULL) {
            redis_client_set(main_redis, redis_key, text, strlen(text),
                             conn_template->sc_ttl);
            store_free(text);
        }
    }
    db_result_free(&res);
    return record;
}

/* What a lookup beyond the working set found. */
enum Lookup {
    LOOKUP_FOUND,   /* `*record' is set (not in the working set) */
    LOOKUP_MISSING, /* There is no such record */
    LOOKUP_UNKNOWN  /* Nobody could say: Redis missed and PostgreSQL failed */
};

/* Look for `nkey' (normalized) among the pending writes, in Redis and in
 * PostgreSQL, in that order.  The caller has looked in the working set. */
static enum Lookup lookup(StoreType* type, const char* nkey, void** record)
{
    struct StoreTypeState_* st = type->state;
    char ident[STORE_KEY_MAX + STORE_NAME_MAX + 2];
    char rkey[STORE_KEY_MAX + STORE_NAME_MAX + 128];
    Pending* p;
    char* value;
    size_t len;
    int missing, res;

    *record = NULL;
    make_ident(type, nkey, ident, sizeof(ident));

    /* Written, and not yet in the database (or in Redis). */
    if ((p = pending_find(ident)) != NULL) {
        st->st_pending++;
        if (!p->bundle)
            return LOOKUP_MISSING;
        if (!(*record = record_from_text(type, p->bundle)))
            return LOOKUP_UNKNOWN;
        return LOOKUP_FOUND;
    }
    if (negative_hinted(ident))
        return LOOKUP_MISSING;

    /* Redis. */
    make_redis_key(type, nkey, rkey, sizeof(rkey));
    res = redis_client_get(main_redis, rkey, &value, &len);
    if (res > 0) {
        if (strcmp(value, STORE_NEGATIVE) == 0) {
            redis_client_free(value);
            st->st_missing++;
            return LOOKUP_MISSING;
        }
        *record = record_from_text(type, value);
        redis_client_free(value);
        if (*record) {
            st->st_redis++;
            return LOOKUP_FOUND;
        }
        /* An undecodable copy: forget it, and ask the database. */
        redis_client_del(main_redis, rkey);
    }

    /* PostgreSQL, the truth. */
    if ((*record = load_from_db(type, nkey, rkey, &missing)) != NULL) {
        st->st_pg++;
        return LOOKUP_FOUND;
    }
    if (missing) {
        st->st_missing++;
        return LOOKUP_MISSING;
    }
    return LOOKUP_UNKNOWN;
}

void* store_get(StoreType* type, const char* key)
{
    struct StoreTypeState_* st;
    char nkey[STORE_KEY_MAX + 1];
    Entry* e;
    void* record;

    if (!type || !(st = type->state) || !key || !*key)
        return NULL;
    st->st_gets++;
    normalize(type, key, nkey, sizeof(nkey));

    /* In use already. */
    if ((e = entry_by_key(st, nkey)) != NULL) {
        e->pins++;
        st->st_mem++;
        return e->record;
    }
    if (lookup(type, nkey, &record) != LOOKUP_FOUND)
        return NULL;
    return adopt(type, nkey, record)->record;
}

void* store_peek(StoreType* type, const char* key)
{
    char nkey[STORE_KEY_MAX + 1];
    Entry* e;

    if (!type || !type->state || !key)
        return NULL;
    normalize(type, key, nkey, sizeof(nkey));
    if (!(e = entry_by_key(type->state, nkey)))
        return NULL;
    e->pins++;
    return e->record;
}

void store_hold(StoreType* type, void* record)
{
    Entry* e;

    if (!record || !type || !type->state)
        return;
    if ((e = entry_by_record(type->state, record)) != NULL)
        e->pins++;
    else
        log("store: BUG: store_hold() of a %s that is not in the store",
            type->name);
}

void store_put(StoreType* type, void* record)
{
    Entry* e;

    if (!record || !type || !type->state)
        return;
    if (!(e = entry_by_record(type->state, record))) {
        log("store: BUG: store_put() of a %s that is not in the store",
            type->name);
        return;
    }
    if (e->pins <= 0) {
        log("store: BUG: store_put() of %s %s, which is not pinned",
            type->name, e->key);
        return;
    }
    if (--e->pins > 0 || e->idle)
        return;
    /* Released at the end of this pass of the main loop. */
    e->type = type;
    e->idle = 1;
    e->iprev = NULL;
    e->inext = idle_list;
    if (idle_list)
        idle_list->iprev = e;
    idle_list = e;
}

static void fetch_flush(void);

void store_collect(void)
{
    Entry* e;

    fetch_flush();
    e = idle_list;

    while (e) {
        Entry* next = e->inext;
        if (e->pins > 0) {
            /* Pinned again since: off the list. */
            if (e->iprev)
                e->iprev->inext = e->inext;
            else
                idle_list = e->inext;
            if (e->inext)
                e->inext->iprev = e->iprev;
            e->idle = 0;
        }
        else {
            StoreType* type = e->type;
            void* record = e->record;
            entry_flush(type, e);
            entry_remove(type->state, e); /* also unlinks it from here */
            (*type->release)(record);
        }
        e = next;
    }
}

int store_add(StoreType* type, void* record)
{
    char key[STORE_KEY_MAX + 1];
    void* existing;
    Entry* e;

    if (!record || !type || !type->state)
        return 0;
    (*type->keyof)(record, key, sizeof(key));
    if (entry_by_key(type->state, key)) {
        log("store: BUG: store_add() of %s %s, which exists", type->name,
            key);
        (*type->release)(record);
        return 0;
    }
    /* Only a key known to be free: a lookup that failed must not pass for
     * "not registered", or the new record would overwrite the real one
     * once the database is back.  Usually a Redis hit on the negative
     * answer the caller's own lookup just left there. */
    switch (lookup(type, key, &existing)) {
    case LOOKUP_MISSING:
        break;
    case LOOKUP_FOUND:
        /* The caller's own lookup failed, or raced with another copy of
         * the data: either way, not ours to replace. */
        log("store: not creating %s %s: it exists", type->name, key);
        (*type->release)(existing);
        (*type->release)(record);
        return 0;
    case LOOKUP_UNKNOWN:
        log("store: not creating %s %s: cannot tell whether it exists",
            type->name, key);
        (*type->release)(record);
        return 0;
    }
    e = entry_add(type->state, key, record);
    e->pins = 1;
    e->hashed = 0; /* never written: the flush writes it */
    entry_flush(type, e);
    return 1;
}

void store_delete(StoreType* type, void* record)
{
    Entry* e;

    if (!record || !type || !type->state)
        return;
    if (!(e = entry_by_record(type->state, record))) {
        log("store: BUG: store_delete() of a %s that is not in the store",
            type->name);
        return;
    }
    if (!readonly)
        write_queue(type, e->key, NULL);
    entry_remove(type->state, e);
    (*type->release)(record);
}

void store_sync(StoreType* type, void* record)
{
    Entry* e;

    if (record && type && type->state &&
        (e = entry_by_record(type->state, record)) != NULL)
        entry_flush(type, e);
}

unsigned int store_resident(StoreType* type)
{
    return type && type->state ? type->state->count : 0;
}

void store_invalidate(StoreType* type, const char* key)
{
    char nkey[STORE_KEY_MAX + 1];
    char rkey[STORE_KEY_MAX + STORE_NAME_MAX + 128];

    if (!type || !key)
        return;
    normalize(type, key, nkey, sizeof(nkey));
    make_redis_key(type, nkey, rkey, sizeof(rkey));
    redis_client_del(main_redis, rkey);
}

/*************************************************************************/
/************************** Background fetches ***************************/
/*************************************************************************/

typedef struct Fetch_ Fetch;
struct Fetch_ {
    Fetch* next;
    struct Module_* owner;
    StoreType* type;    /* NULL once cancelled with its type */
    StoreFetchFn done;
    void* arg;
    int cancelled;
    int queued;         /* Keys not submitted yet (fetch_flush()) */
    int outstanding;    /* Batches not back yet */
    char** keys;        /* Keys asked for, normalized */
    int nkeys;
    char** pinned_keys; /* Records loaded for the callback: key... */
    void** pinned;      /* ...and address, checked before unpinning */
    int pinned_count;
    char** missing;     /* Idents known missing */
    int missing_count;
};
static Fetch* fetches;
static int fetches_queued;

/* A batch of keys in flight, and the fetches waiting for any of them. */
typedef struct {
    StoreType* type;
    Fetch** waiters;
    int nwaiters;
} FetchBatch;

static void fetch_free(Fetch* f)
{
    Fetch** pp;
    int i;

    for (pp = &fetches; *pp; pp = &(*pp)->next) {
        if (*pp == f) {
            *pp = f->next;
            break;
        }
    }
    for (i = 0; i < f->nkeys; i++)
        free(f->keys[i]);
    for (i = 0; i < f->missing_count; i++)
        free(f->missing[i]);
    for (i = 0; i < f->pinned_count; i++)
        free(f->pinned_keys[i]);
    free(f->keys);
    free(f->missing);
    free(f->pinned_keys);
    free(f->pinned);
    free(f);
}

/* Every batch of `f' is back: run the callback, with the records pinned
 * and the missing ones known, then let go of them. */
static void fetch_finish(Fetch* f)
{
    int i;

    if (!f->cancelled) {
        negative_hints = f->missing;
        negative_hints_count = f->missing_count;
        (*f->done)(f->arg);
        negative_hints = NULL;
        negative_hints_count = 0;
    }
    /* Let go of what the fetch pinned -- unless the callback deleted it (or
     * replaced it with a new record under the same key), or the type is
     * gone with its records. */
    for (i = 0; f->type && f->type->state && i < f->pinned_count; i++) {
        Entry* e = entry_by_key(f->type->state, f->pinned_keys[i]);
        if (e && e->record == f->pinned[i])
            store_put(f->type, e->record);
    }
    fetch_free(f);
}

static void fetch_work(struct WorkTask* task)
{
    store_fetch_run(task->wt_in);
}

static void fetch_task_free(struct WorkTask* task)
{
    FetchBatch* b = task->wt_arg;

    store_fetch_free(task->wt_in);
    task->wt_in = NULL;
    if (b) {
        /* Dropped without an answer (shutdown): nobody is waiting any
         * more -- the fetches themselves are freed with the store. */
        free(b->waiters);
        free(b);
        task->wt_arg = NULL;
    }
}

/* Index of `key' among the keys of a batch, or -1. */
static int batch_index(const struct StoreFetch* io, const char* key)
{
    int i;

    for (i = 0; i < io->sf_nkeys; i++) {
        if (strcmp(io->sf_keys[i], key) == 0)
            return i;
    }
    return -1;
}

/* A batch is back.  Main thread.  The records it found join the working
 * set (pinned by the batch for the moment), every fetch waiting on them
 * pins what it asked for, and the batch then lets go of its own pins. */
static void fetch_done(struct WorkTask* task)
{
    struct StoreFetch* io = task->wt_in;
    FetchBatch* b = task->wt_arg;
    StoreType* type = b->type;
    struct StoreTypeState_* st = NULL;
    void** adopted;
    char* found; /* 0 unknown, 1 record, 2 missing */
    int i, j, k;

    task->wt_arg = NULL;
    for (i = 0; i < b->nwaiters; i++) {
        if (!b->waiters[i]->cancelled && b->waiters[i]->type)
            st = b->waiters[i]->type->state;
    }
    adopted = scalloc(io->sf_nkeys + 1, sizeof(*adopted));
    found = scalloc(io->sf_nkeys + 1, 1);
    for (i = 0; st && i < io->sf_nkeys; i++) {
        char ident[STORE_KEY_MAX + STORE_NAME_MAX + 2];
        Entry* e;
        void* record;

        if (!io->sf_found || !io->sf_found[i])
            continue; /* not known: store_get() will ask */
        make_ident(type, io->sf_keys[i], ident, sizeof(ident));
        if ((e = entry_by_key(st, io->sf_keys[i])) != NULL) {
            found[i] = 1; /* known better already */
            continue;
        }
        if (pending_find(ident))
            continue; /* store_get() takes it from the pending write */
        if (!io->sf_bundles[i]) {
            found[i] = 2;
            continue;
        }
        if (!(record = record_from_text(type, io->sf_bundles[i])))
            continue;
        adopted[i] = adopt(type, io->sf_keys[i], record)->record;
        found[i] = 1;
        st->st_prefetched++;
    }

    for (j = 0; j < b->nwaiters; j++) {
        Fetch* f = b->waiters[j];
        for (k = 0; st && !f->cancelled && k < f->nkeys; k++) {
            Entry* e;
            if ((i = batch_index(io, f->keys[k])) < 0 || !found[i])
                continue;
            if (found[i] == 2) {
                char ident[STORE_KEY_MAX + STORE_NAME_MAX + 2];
                make_ident(type, f->keys[k], ident, sizeof(ident));
                f->missing = srealloc(f->missing, sizeof(*f->missing) *
                                                      (f->missing_count + 1));
                f->missing[f->missing_count++] = sstrdup(ident);
            }
            else if ((e = entry_by_key(st, f->keys[k])) != NULL) {
                e->pins++;
                f->pinned = srealloc(f->pinned, sizeof(*f->pinned) *
                                                    (f->pinned_count + 1));
                f->pinned_keys =
                    srealloc(f->pinned_keys,
                             sizeof(*f->pinned_keys) * (f->pinned_count + 1));
                f->pinned[f->pinned_count] = e->record;
                f->pinned_keys[f->pinned_count++] = sstrdup(f->keys[k]);
            }
        }
    }
    /* The batch's own pins go before the callbacks run, which may delete
     * what they were given. */
    for (i = 0; st && i < io->sf_nkeys; i++) {
        Entry* e;
        if (adopted[i] && (e = entry_by_key(st, io->sf_keys[i])) != NULL &&
            e->record == adopted[i])
            store_put(type, adopted[i]);
    }
    free(adopted);
    free(found);
    for (j = 0; j < b->nwaiters; j++) {
        Fetch* f = b->waiters[j];
        if (--f->outstanding == 0)
            fetch_finish(f);
    }
    free(b->waiters);
    free(b);
}

/* The current connection settings, for a thread. */
static struct StoreConn* conn_current(void);

static void batch_add_waiter(FetchBatch* b, Fetch* f)
{
    if (b->nwaiters > 0 && b->waiters[b->nwaiters - 1] == f)
        return;
    b->waiters = srealloc(b->waiters, sizeof(*b->waiters) * (b->nwaiters + 1));
    b->waiters[b->nwaiters++] = f;
    f->outstanding++;
}

/* Submit one batch; nonzero if it went to the pool. */
static int batch_submit(StoreType* type, FetchBatch* b, char** keys, int n)
{
    struct StoreTypeState_* st = type->state;
    struct StoreFetch* io = store_calloc(1, sizeof(*io));
    struct WorkTask* task;
    char rkey[STORE_KEY_MAX + STORE_NAME_MAX + 128];
    int i;

    io->sf_conn = conn_current();
    io->sf_sql = store_strdup(st->batch_sql);
    io->sf_nkeys = n;
    io->sf_keys = store_calloc(n, sizeof(char*));
    io->sf_redis_keys = store_calloc(n, sizeof(char*));
    for (i = 0; i < n; i++) {
        io->sf_keys[i] = store_strdup(keys[i]);
        make_redis_key(type, keys[i], rkey, sizeof(rkey));
        io->sf_redis_keys[i] = store_strdup(rkey);
    }
    task = worker_task_new(fetch_work, fetch_done);
    task->wt_in = io;
    task->wt_arg = b;
    task->wt_free = fetch_task_free;
    if (!worker_submit(task)) {
        task->wt_arg = NULL;
        worker_task_free(task);
        return 0;
    }
    return 1;
}

/* Send the keys asked for during this pass of the main loop, all fetches
 * together: a burst of a thousand users is two batches, not a thousand
 * round trips. */
static void fetch_flush(void)
{
    Fetch* f;

    while (fetches_queued) {
        StoreType* type = NULL;
        char** keys = NULL;
        int nkeys = 0, start, i, k;

        /* One type at a time.  The fetches of this round are marked 2: a
         * callback run below may ask for more, which wait for the next. */
        for (f = fetches; f; f = f->next) {
            if (f->queued && !type)
                type = f->type;
            if (f->queued && f->type == type)
                f->queued = 2;
        }
        for (f = fetches; f; f = f->next) {
            if (f->queued != 2)
                continue;
            for (k = 0; k < f->nkeys; k++) {
                char ident[STORE_KEY_MAX + STORE_NAME_MAX + 2];
                make_ident(type, f->keys[k], ident, sizeof(ident));
                if (entry_by_key(type->state, f->keys[k]) ||
                    pending_find(ident))
                    continue;
                for (i = 0; i < nkeys; i++) {
                    if (strcmp(keys[i], f->keys[k]) == 0)
                        break;
                }
                if (i < nkeys)
                    continue;
                keys = srealloc(keys, sizeof(*keys) * (nkeys + 1));
                keys[nkeys++] = f->keys[k];
            }
        }
        for (start = 0; start < nkeys; start += FETCH_BATCH) {
            int n = nkeys - start < FETCH_BATCH ? nkeys - start : FETCH_BATCH;
            FetchBatch* b = scalloc(1, sizeof(*b));
            b->type = type;
            for (f = fetches; f; f = f->next) {
                if (f->queued != 2)
                    continue;
                for (k = 0; k < f->nkeys; k++) {
                    for (i = start; i < start + n; i++) {
                        if (keys[i] == f->keys[k] ||
                            strcmp(keys[i], f->keys[k]) == 0)
                            break;
                    }
                    if (i < start + n) {
                        batch_add_waiter(b, f);
                        break;
                    }
                }
            }
            if (!batch_submit(type, b, keys + start, n)) {
                /* No pool: the callbacks will read synchronously. */
                for (i = 0; i < b->nwaiters; i++)
                    b->waiters[i]->outstanding--;
                free(b->waiters);
                free(b);
            }
        }
        free(keys);
        /* Done with this type: every fetch that is not waiting for a batch
         * finishes now. */
        for (;;) {
            for (f = fetches; f; f = f->next) {
                if (f->queued == 2)
                    break;
            }
            if (!f)
                break;
            f->queued = 0;
            fetches_queued--;
            if (f->outstanding == 0)
                fetch_finish(f); /* changes the list: start over */
        }
    }
}

int store_prefetch(struct Module_* owner, StoreType* type, const char** keys,
                   int nkeys, StoreFetchFn done, void* arg)
{
    struct StoreTypeState_* st;
    Fetch* f;
    int i, j;

    if (!type || !(st = type->state) || !done)
        return 0;
    f = scalloc(1, sizeof(*f));
    f->owner = owner;
    f->type = type;
    f->done = done;
    f->arg = arg;
    f->keys = smalloc(sizeof(*f->keys) * (nkeys + 1));
    for (i = 0; i < nkeys; i++) {
        char nkey[STORE_KEY_MAX + 1];
        char ident[STORE_KEY_MAX + STORE_NAME_MAX + 2];
        if (!keys[i] || !*keys[i])
            continue;
        normalize(type, keys[i], nkey, sizeof(nkey));
        make_ident(type, nkey, ident, sizeof(ident));
        if (entry_by_key(st, nkey) || pending_find(ident))
            continue;
        for (j = 0; j < f->nkeys; j++) {
            if (strcmp(f->keys[j], nkey) == 0)
                break;
        }
        if (j < f->nkeys)
            continue;
        f->keys[f->nkeys++] = sstrdup(nkey);
    }
    f->next = fetches;
    fetches = f;
    if (f->nkeys == 0) {
        fetch_finish(f); /* everything is at hand */
        return 1;
    }
    /* Sent with everything else asked for in this pass (store_collect()). */
    f->queued = 1;
    fetches_queued++;
    return 1;
}

/* Defined with the registration code below. */
static char* qname(const char* name, char* buf, size_t size);
static void build_bundle_expr(const StoreType* type, const char* keyexpr,
                              struct PgBuf* out);
static char* pgbuf_take(struct PgBuf* buf);

/*************************************************************************/
/***************************** Going through *****************************/
/*************************************************************************/

/* Records per page in store_foreach(). */
#define FOREACH_PAGE 200

/* The smallest key of a type, to start a page walk from. */
static const char* min_key(const StoreType* type)
{
    return strcmp(type->key_type, "text") == 0 ? ""
                                               : "-9223372036854775808";
}

int store_foreach(StoreType* type, const char* where,
                  const char* const* params, int nparams, StoreEachFn fn,
                  void* arg)
{
    struct StoreTypeState_* st;
    char qt[STORE_NAME_MAX * 2 + 3], qk[STORE_NAME_MAX * 2 + 3];
    char keyexpr[STORE_NAME_MAX + 32];
    struct DbParam* pv;
    struct DbParam** pl;
    struct PgBuf buf;
    char* sql;
    char* last;
    int seen = 0, stop = 0, i;

    if (!type || !(st = type->state) || !fn)
        return -1;
    qname(type->table, qt, sizeof(qt));
    qname(type->key_column, qk, sizeof(qk));
    snprintf(keyexpr, sizeof(keyexpr), "p.key::%s", type->key_type);
    pgbuf_init(&buf);
    pgbuf_printf(&buf,
                 "select p.key::text as key, ");
    build_bundle_expr(type, keyexpr, &buf);
    pgbuf_printf(&buf,
                 " as bundle from (select t.%s as key from %s t where t.%s >"
                 " $1::%s and (%s) order by t.%s limit %d) p order by p.key",
                 qk, qt, qk, type->key_type, where ? where : "true", qk,
                 FOREACH_PAGE);
    sql = pgbuf_take(&buf);

    pv = scalloc(nparams + 1, sizeof(*pv));
    pl = scalloc(nparams + 2, sizeof(*pl));
    for (i = 0; i < nparams; i++) {
        pv[i + 1].type = DB_TYPE_UNKNOWN;
        pv[i + 1].value = params[i];
        pv[i + 1].format = DB_FORMAT_TEXT;
        pl[i + 1] = &pv[i + 1];
    }
    pv[0].type = DB_TYPE_TEXT;
    pv[0].format = DB_FORMAT_TEXT;
    pl[0] = &pv[0];
    last = sstrdup(min_key(type));

    while (!stop) {
        struct DbQuery query = {sql, pl};
        struct DbResult res;
        unsigned int rows, r;

        pv[0].value = last;
        if (db_query_sync(NULL, &query, &res) != DB_OK) {
            log("store: cannot go through %s: %s", type->name,
                res.err.dberr_message);
            db_result_free(&res);
            seen = -1;
            break;
        }
        rows = db_rows(res.data);
        for (r = 0; r < rows && !stop; r++) {
            json_t* row = json_array_get((json_t*)res.data, r);
            const char* key =
                json_string_value(json_object_get(row, "key"));
            char ident[STORE_KEY_MAX + STORE_NAME_MAX + 2];
            Entry* e;
            Pending* p;
            void* record = NULL;

            if (!key)
                continue;
            free(last);
            last = sstrdup(key);
            make_ident(type, key, ident, sizeof(ident));
            /* The copy that is in use, or about to be written, wins. */
            if ((e = entry_by_key(st, key)) != NULL) {
                e->pins++;
                record = e->record;
            }
            else if ((p = pending_find(ident)) != NULL) {
                if (p->bundle && (record = record_from_text(type, p->bundle)))
                    record = adopt(type, key, record)->record;
            }
            else {
                json_t* bundle = json_object_get(row, "bundle");
                json_t* main = json_object_get(bundle, "main");
                if (main && !json_is_null(main) &&
                    (record = (*type->decode)(bundle)) != NULL)
                    record = adopt(type, key, record)->record;
            }
            if (!record)
                continue;
            seen++;
            stop = (*fn)(record, arg);
            /* `fn' may have deleted it. */
            if ((e = entry_by_key(st, key)) != NULL && e->record == record)
                store_put(type, record);
        }
        db_result_free(&res);
        if (rows < FOREACH_PAGE)
            break;
    }
    free(last);
    free(pv);
    free(pl);
    free(sql);
    return seen;
}

long store_count(StoreType* type, const char* where, const char* const* params,
                 int nparams)
{
    char qt[STORE_NAME_MAX * 2 + 3];
    struct DbParam* pv;
    struct DbParam** pl;
    struct DbQuery query;
    struct DbResult res;
    struct PgBuf buf;
    long count = -1;
    char* sql;
    int i;

    if (!type || !type->state)
        return -1;
    pgbuf_init(&buf);
    /* $1 is unused here but numbered as in store_foreach(), so that one
     * condition serves both; it is compared with itself to be typed. */
    pgbuf_printf(&buf, "select count(*) as n from %s t where ($1::text is null"
                       " or true) and (%s)",
                 qname(type->table, qt, sizeof(qt)), where ? where : "true");
    sql = pgbuf_take(&buf);
    pv = scalloc(nparams + 1, sizeof(*pv));
    pl = scalloc(nparams + 2, sizeof(*pl));
    pv[0].type = DB_TYPE_TEXT;
    pv[0].value = NULL;
    pl[0] = &pv[0];
    for (i = 0; i < nparams; i++) {
        pv[i + 1].type = DB_TYPE_UNKNOWN;
        pv[i + 1].value = params[i];
        pl[i + 1] = &pv[i + 1];
    }
    query.sql = sql;
    query.params = pl;
    if (db_query_sync(NULL, &query, &res) == DB_OK)
        count = (long)db_row_int(res.data, 0, "n");
    else
        log("store: cannot count %s: %s", type->name, res.err.dberr_message);
    db_result_free(&res);
    free(pv);
    free(pl);
    free(sql);
    return count;
}

/*************************************************************************/
/******************************** The sweep ******************************/
/*************************************************************************/

/* Look at a slice of the records that stay pinned, so that a change to a
 * record nobody lets go of (the nick of a user who stays online) reaches
 * the database within SWEEP_ROUND_SECS. */
static void sweep(Timeout* t)
{
    TypeNode* tn;

    for (tn = types; tn; tn = tn->next) {
        struct StoreTypeState_* st = tn->type->state;
        uint32 n = st->count / (SWEEP_ROUND_SECS * 1000 / SWEEP_INTERVAL_MS);

        if (n < 16)
            n = 16;
        while (n-- > 0 && st->count) {
            if (!st->sweep)
                st->sweep = st->all;
            if (!st->sweep)
                break;
            entry_flush(tn->type, st->sweep);
            st->sweep = st->sweep->next;
        }
    }
}

/*************************************************************************/
/***************************** Registration ******************************/
/*************************************************************************/

/* Quote a name the module chose, for SQL. */
static char* qname(const char* name, char* buf, size_t size)
{
    if (!pg_quote_ident(name, buf, size))
        snprintf(buf, size, "\"\"");
    return buf;
}

/* The json_build_object() that loads a record, with `keyexpr' as the key
 * (already cast to the key's type). */
static void build_bundle_expr(const StoreType* type, const char* keyexpr,
                              struct PgBuf* out)
{
    char qt[STORE_NAME_MAX * 2 + 3], qk[STORE_NAME_MAX * 2 + 3];
    const StoreChild* c;

    pgbuf_printf(out,
                 "json_build_object('main', (select row_to_json(t) from %s t"
                 " where t.%s = %s)",
                 qname(type->table, qt, sizeof(qt)),
                 qname(type->key_column, qk, sizeof(qk)), keyexpr);
    for (c = type->children; c && c->name; c++) {
        char qc[STORE_NAME_MAX * 2 + 3], qf[STORE_NAME_MAX * 2 + 3];
        pgbuf_printf(out,
                     ", '%s', coalesce((select json_agg(d) from (select %s"
                     " from %s x where x.%s = %s order by %s) d), '[]'::json)",
                     c->name, c->columns ? c->columns : "*",
                     qname(c->table ? c->table : c->name, qc, sizeof(qc)),
                     qname(c->fk, qf, sizeof(qf)), keyexpr,
                     c->order ? c->order : "1");
    }
    pgbuf_puts(out, ")");
}

static char* pgbuf_take(struct PgBuf* buf)
{
    char* text = pgbuf_steal(buf);
    char* copy = text ? sstrdup(text) : NULL;

    pg_save_free(text);
    return copy;
}

static void add_stmt(char*** list, int** flags, int* n, char* sql, int bundle)
{
    *list = srealloc(*list, sizeof(**list) * (*n + 1));
    if (flags) {
        *flags = srealloc(*flags, sizeof(**flags) * (*n + 1));
        (*flags)[*n] = bundle;
    }
    (*list)[(*n)++] = sql;
}

int store_register(StoreType* type)
{
    struct StoreTypeState_* st;
    char qt[STORE_NAME_MAX * 2 + 3], qk[STORE_NAME_MAX * 2 + 3];
    char keyexpr[STORE_NAME_MAX + 32];
    const StoreChild* c;
    struct PgBuf buf;
    TypeNode* tn;

    if (!type || !type->name || !type->table || !type->key_column ||
        !type->key_type || !type->decode || !type->encode || !type->release ||
        !type->keyof) {
        log("store: BUG: incomplete type %s",
            type && type->name ? type->name : "(null)");
        return 0;
    }
    if (type->state) {
        log("store: BUG: type %s registered twice", type->name);
        return 0;
    }
    st = scalloc(1, sizeof(*st));
    st->nbuckets = 256;
    st->kbuckets = scalloc(st->nbuckets, sizeof(*st->kbuckets));
    st->pbuckets = scalloc(st->nbuckets, sizeof(*st->pbuckets));
    qname(type->table, qt, sizeof(qt));
    qname(type->key_column, qk, sizeof(qk));

    /* Loading one record: $1 is the key, as text. */
    pgbuf_init(&buf);
    snprintf(keyexpr, sizeof(keyexpr), "$1::%s", type->key_type);
    pgbuf_puts(&buf, "select ");
    build_bundle_expr(type, keyexpr, &buf);
    pgbuf_puts(&buf, " as bundle");
    st->load_sql = pgbuf_take(&buf);

    /* Loading a batch: $1 is a JSON array of keys. */
    pgbuf_init(&buf);
    snprintf(keyexpr, sizeof(keyexpr), "k.key::%s", type->key_type);
    pgbuf_printf(&buf,
                 "select k.key, not exists (select 1 from %s t where t.%s ="
                 " %s) as missing, ",
                 qt, qk, keyexpr);
    build_bundle_expr(type, keyexpr, &buf);
    pgbuf_puts(&buf, " as bundle from jsonb_array_elements_text($1::jsonb)"
                     " as k(key)");
    st->batch_sql = pgbuf_take(&buf);

    /* Writing: delete, then insert, the main row and every child that is
     * not read-only.  $1 is the key or the bundle.  No value is ever
     * pasted into these: the names are the module's, the data is bound. */
    pgbuf_init(&buf);
    pgbuf_printf(&buf, "delete from %s where %s = $1::%s", qt, qk,
                 type->key_type);
    add_stmt(&st->write_sql, &st->write_bundle, &st->nwrite, pgbuf_take(&buf),
             0);
    pgbuf_init(&buf);
    pgbuf_printf(&buf, "delete from %s where %s = $1::%s", qt, qk,
                 type->key_type);
    add_stmt(&st->delete_sql, NULL, &st->ndelete, pgbuf_take(&buf), 0);
    pgbuf_init(&buf);
    pgbuf_printf(&buf,
                 "insert into %s select * from jsonb_populate_record(null::%s,"
                 " $1::jsonb -> 'main')",
                 qt, qt);
    add_stmt(&st->write_sql, &st->write_bundle, &st->nwrite, pgbuf_take(&buf),
             1);
    for (c = type->children; c && c->name; c++) {
        char qc[STORE_NAME_MAX * 2 + 3], qf[STORE_NAME_MAX * 2 + 3];
        if (c->readonly)
            continue;
        qname(c->table ? c->table : c->name, qc, sizeof(qc));
        qname(c->fk, qf, sizeof(qf));
        pgbuf_init(&buf);
        pgbuf_printf(&buf, "delete from %s where %s = $1::%s", qc, qf,
                     type->key_type);
        add_stmt(&st->write_sql, &st->write_bundle, &st->nwrite,
                 pgbuf_take(&buf), 0);
        pgbuf_init(&buf);
        pgbuf_printf(&buf, "delete from %s where %s = $1::%s", qc, qf,
                     type->key_type);
        add_stmt(&st->delete_sql, NULL, &st->ndelete, pgbuf_take(&buf), 0);
        pgbuf_init(&buf);
        pgbuf_printf(&buf,
                     "insert into %s select * from"
                     " jsonb_populate_recordset(null::%s, coalesce($1::jsonb"
                     " -> '%s', '[]'::jsonb))",
                     qc, qc, c->name);
        add_stmt(&st->write_sql, &st->write_bundle, &st->nwrite,
                 pgbuf_take(&buf), 1);
    }

    type->state = st;
    tn = scalloc(1, sizeof(*tn));
    tn->type = type;
    tn->next = types;
    types = tn;
    return 1;
}

/* Cancel the fetches of `type' or of `owner'. */
static void fetches_cancel(StoreType* type, struct Module_* owner)
{
    Fetch* f;

    for (f = fetches; f; f = f->next) {
        if ((type && f->type == type) || (owner && f->owner == owner)) {
            f->cancelled = 1;
            /* Their pinned records are released below or by the finish. */
        }
    }
    /* The ones not sent yet go now; the others when their batches are
     * back (fetch_done() checks). */
    for (;;) {
        for (f = fetches; f; f = f->next) {
            if (f->cancelled && f->queued)
                break;
        }
        if (!f)
            break;
        f->queued = 0;
        fetches_queued--;
        if (f->outstanding == 0)
            fetch_free(f);
    }
    /* A type going away takes its StoreType (in the module) with it. */
    for (f = fetches; type && f; f = f->next) {
        if (f->type == type) {
            int i;
            for (i = 0; type->state && i < f->pinned_count; i++) {
                Entry* e = entry_by_key(type->state, f->pinned_keys[i]);
                if (e && e->record == f->pinned[i])
                    e->pins--;
            }
            for (i = 0; i < f->pinned_count; i++)
                free(f->pinned_keys[i]);
            f->pinned_count = 0;
            f->type = NULL;
        }
    }
}

void store_unregister(StoreType* type)
{
    struct StoreTypeState_* st;
    TypeNode** tp;
    int i, dropped = 0;

    if (!type || !(st = type->state))
        return;
    fetches_cancel(type, NULL);
    while (st->all) {
        Entry* e = st->all;
        void* record = e->record;
        entry_flush(type, e);
        if (e->pins > 1)
            dropped++;
        entry_remove(st, e);
        (*type->release)(record);
    }
    if (dropped)
        log("store: %s: %d record%s still pinned when the type went away",
            type->name, dropped, dropped == 1 ? " was" : "s were");
    for (tp = &types; *tp; tp = &(*tp)->next) {
        if ((*tp)->type == type) {
            TypeNode* tn = *tp;
            *tp = tn->next;
            free(tn);
            break;
        }
    }
    for (i = 0; i < st->nwrite; i++)
        free(st->write_sql[i]);
    for (i = 0; i < st->ndelete; i++)
        free(st->delete_sql[i]);
    free(st->write_sql);
    free(st->write_bundle);
    free(st->delete_sql);
    free(st->load_sql);
    free(st->batch_sql);
    free(st->kbuckets);
    free(st->pbuckets);
    free(st);
    type->state = NULL;
}

void store_drop_module(struct Module_* mod)
{
    fetches_cancel(NULL, mod);
}

/*************************************************************************/
/*************************** Encoding helpers ****************************/
/*************************************************************************/

/* A column name from a field name. */
static void column_name(const char* field, char* buf, size_t size)
{
    size_t n = 0;

    for (; *field && n + 1 < size; field++) {
        unsigned char c = (unsigned char)*field;
        if (c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
        else if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
            c = '_';
        buf[n++] = (char)c;
    }
    buf[n] = 0;
}

char* store_like_pattern(const char* glob, char* buf, size_t size)
{
    size_t n = 0;

    for (; *glob && n + 2 < size; glob++) {
        switch (*glob) {
        case '*':
            buf[n++] = '%';
            break;
        case '?':
            buf[n++] = '_';
            break;
        case '%':
        case '_':
        case '\\':
            buf[n++] = '\\';
            buf[n++] = *glob;
            break;
        default:
            buf[n++] = *glob;
            break;
        }
    }
    buf[n] = 0;
    return buf;
}

json_t* store_json_string(const char* str)
{
    const unsigned char* s;
    json_t* value;
    char* conv;
    size_t n = 0;

    if (!str)
        return json_null();
    if ((value = json_string(str)) != NULL)
        return value;
    /* Not valid UTF-8: legacy ISO-8859-1, converted. */
    conv = smalloc(strlen(str) * 2 + 1);
    for (s = (const unsigned char*)str; *s; s++) {
        if (*s < 0x80) {
            conv[n++] = (char)*s;
        }
        else {
            conv[n++] = (char)(0xC0 | (*s >> 6));
            conv[n++] = (char)(0x80 | (*s & 0x3F));
        }
    }
    conv[n] = 0;
    value = json_string(conv);
    free(conv);
    return value ? value : json_null();
}

char* store_dup_string(json_t* value)
{
    if (!value || !json_is_string(value))
        return NULL;
    return sstrdup(json_string_value(value));
}

/* The bytea text form of a byte string. */
static json_t* bytea_value(const unsigned char* data, size_t len)
{
    static const char hex[] = "0123456789abcdef";
    char* text = smalloc(len * 2 + 3);
    json_t* value;
    size_t i;

    text[0] = '\\';
    text[1] = 'x';
    for (i = 0; i < len; i++) {
        text[2 + i * 2] = hex[data[i] >> 4];
        text[3 + i * 2] = hex[data[i] & 15];
    }
    text[2 + len * 2] = 0;
    value = json_string(text);
    free(text);
    return value;
}

static void bytea_decode(const char* text, unsigned char* out, size_t size)
{
    size_t n = 0;

    memset(out, 0, size);
    if (!text || text[0] != '\\' || text[1] != 'x')
        return;
    for (text += 2; text[0] && text[1] && n < size; text += 2) {
        int hi = isdigit((unsigned char)text[0]) ? text[0] - '0'
                                                 : (tolower(text[0]) - 'a' + 10);
        int lo = isdigit((unsigned char)text[1]) ? text[1] - '0'
                                                 : (tolower(text[1]) - 'a' + 10);
        if (hi < 0 || hi > 15 || lo < 0 || lo > 15)
            return;
        out[n++] = (unsigned char)(hi << 4 | lo);
    }
}

json_t* store_encode_fields(const void* record, const DBField* fields)
{
    json_t* row = json_object();
    char name[STORE_NAME_MAX + 8];
    const DBField* f;

    for (f = fields; f->name; f++) {
        union {
            int8 i8;
            uint8 u8;
            int16 i16;
            uint16 u16;
            int32 i32;
            uint32 u32;
            time_t t;
            char* s;
            Password pass;
        } v;
        json_t* value = NULL;

        if (f->load_only)
            continue;
        column_name(f->name, name, sizeof(name) - 8);
        if (f->type == DBTYPE_BUFFER) {
            char* buf = scalloc(f->length + 1, 1);
            get_dbfield(record, f, buf);
            buf[f->length] = 0;
            value = store_json_string(buf);
            free(buf);
            json_object_set_new(row, name, value);
            continue;
        }
        memset(&v, 0, sizeof(v));
        get_dbfield(record, f, &v);
        switch (f->type) {
        case DBTYPE_INT8:
            value = json_integer(v.i8);
            break;
        case DBTYPE_UINT8:
            value = json_integer(v.u8);
            break;
        case DBTYPE_INT16:
            value = json_integer(v.i16);
            break;
        case DBTYPE_UINT16:
            value = json_integer(v.u16);
            break;
        case DBTYPE_INT32:
            value = json_integer(v.i32);
            break;
        case DBTYPE_UINT32:
            value = json_integer(v.u32);
            break;
        case DBTYPE_TIME:
            value = json_integer((json_int_t)v.t);
            break;
        case DBTYPE_STRING:
            value = store_json_string(v.s);
            break;
        case DBTYPE_PASSWORD: {
            size_t len = PASSMAX;
            char cname[STORE_NAME_MAX + 16];
            while (len > 0 && !v.pass.password[len - 1])
                len--;
            value = bytea_value((const unsigned char*)v.pass.password, len);
            snprintf(cname, sizeof(cname), "%s_cipher", name);
            json_object_set_new(row, cname, store_json_string(v.pass.cipher));
            break;
        }
        case DBTYPE_BUFFER:
            break;
        }
        json_object_set_new(row, name, value ? value : json_null());
    }
    return row;
}

void store_decode_fields(json_t* row, void* record, const DBField* fields)
{
    char name[STORE_NAME_MAX + 8];
    const DBField* f;

    if (!row || !json_is_object(row))
        return;
    for (f = fields; f->name; f++) {
        union {
            int8 i8;
            uint8 u8;
            int16 i16;
            uint16 u16;
            int32 i32;
            uint32 u32;
            time_t t;
            char* s;
            Password pass;
        } v;
        json_t* value;
        json_int_t n;

        column_name(f->name, name, sizeof(name) - 8);
        value = json_object_get(row, name);
        if (!value || json_is_null(value)) {
            if (f->type == DBTYPE_STRING && value) {
                v.s = NULL;
                put_dbfield(record, f, &v);
            }
            continue;
        }
        n = json_is_integer(value) ? json_integer_value(value)
            : json_is_real(value)  ? (json_int_t)json_real_value(value)
            : json_is_string(value)
                ? (json_int_t)strtoll(json_string_value(value), NULL, 10)
                : 0;
        switch (f->type) {
        case DBTYPE_INT8:
            v.i8 = (int8)n;
            break;
        case DBTYPE_UINT8:
            v.u8 = (uint8)n;
            break;
        case DBTYPE_INT16:
            v.i16 = (int16)n;
            break;
        case DBTYPE_UINT16:
            v.u16 = (uint16)n;
            break;
        case DBTYPE_INT32:
            v.i32 = (int32)n;
            break;
        case DBTYPE_UINT32:
            v.u32 = (uint32)n;
            break;
        case DBTYPE_TIME:
            v.t = (time_t)n;
            break;
        case DBTYPE_STRING:
            /* The record (or the field's put routine) takes it. */
            v.s = store_dup_string(value);
            break;
        case DBTYPE_BUFFER: {
            char* buf = scalloc(f->length > 0 ? f->length : 1, 1);
            if (json_is_string(value) && f->length > 0)
                strscpy(buf, json_string_value(value), f->length);
            put_dbfield(record, f, buf);
            free(buf);
            continue;
        }
        case DBTYPE_PASSWORD: {
            char password[PASSMAX], cname[STORE_NAME_MAX + 16];
            json_t* cipher;
            snprintf(cname, sizeof(cname), "%s_cipher", name);
            cipher = json_object_get(row, cname);
            bytea_decode(json_is_string(value) ? json_string_value(value) : NULL,
                         (unsigned char*)password, sizeof(password));
            init_password(&v.pass);
            set_password(&v.pass, password,
                         cipher && json_is_string(cipher)
                             ? json_string_value(cipher)
                             : NULL);
            break;
        }
        }
        put_dbfield(record, f, &v);
    }
}

/*************************************************************************/
/******************************* Lifecycle *******************************/
/*************************************************************************/

/* The connection settings for the store's threads, rebuilt when the
 * database or redis block changes. */
static struct StoreConn* conn_current(void)
{
    const struct CacheConf* cc = cache_conf();
    char search_path[300], check[300], qschema[130];
    unsigned int gen = db_conf_generation() * 65536 +
                       (cc ? cc->cconf_generation : 0);

    if (!conn_template || conn_generation != gen) {
        struct StoreConn c;
        memset(&c, 0, sizeof(c));
        c.sc_generation = gen;
        c.sc_dsn = (char*)db_conf_dsn(DB_ROLE_WRITE);
        c.sc_search_path =
            pg_search_path(db_conf_schema(), search_path, sizeof(search_path));
        c.sc_timeout_ms = db_conf_timeout();
        if (cc) {
            snprintf(c.sc_redis.rcc_host, sizeof(c.sc_redis.rcc_host), "%s",
                     cc->cconf_host ? cc->cconf_host : "");
            snprintf(c.sc_redis.rcc_socket, sizeof(c.sc_redis.rcc_socket),
                     "%s", cc->cconf_socket ? cc->cconf_socket : "");
            snprintf(c.sc_redis.rcc_password, sizeof(c.sc_redis.rcc_password),
                     "%s", cc->cconf_password ? cc->cconf_password : "");
            c.sc_redis.rcc_port = cc->cconf_port;
            c.sc_redis.rcc_database = cc->cconf_database;
            c.sc_redis.rcc_timeout_ms = cc->cconf_timeout_ms;
            c.sc_ttl = cc->cconf_ttl > 0 ? cc->cconf_ttl : DEFAULT_TTL;
            c.sc_negative_ttl = cc->cconf_negative_ttl > 0
                                    ? cc->cconf_negative_ttl
                                    : DEFAULT_NEGATIVE_TTL;
        }
        c.sc_token = (char*)pg_sync_token();
        pg_quote_ident(db_conf_schema(), qschema, sizeof(qschema));
        snprintf(check, sizeof(check),
                 "select token from %s.instance where id = 1 for update",
                 qschema);
        c.sc_check = check;
        store_conn_free(conn_template);
        conn_template = store_conn_copy(&c);
        conn_generation = gen;
        if (main_redis) {
            redis_client_close(main_redis);
            main_redis = NULL;
        }
        if (store_up)
            store_writer_reconfigure(store_conn_copy(conn_template));
    }
    if (!main_redis)
        main_redis = redis_client_new(&conn_template->sc_redis);
    return store_conn_copy(conn_template);
}

/* Delete from Redis the keys the writer could not update there, from this
 * run or an earlier one. */
static void clear_stale(void)
{
    struct DbQuery query = {"select redis_key from store_stale", NULL};
    struct DbResult res;
    unsigned int i;

    if (db_query_sync(NULL, &query, &res) != DB_OK) {
        db_result_free(&res);
        return;
    }
    for (i = 0; i < res.rows; i++) {
        struct DbParam param = {DB_TYPE_TEXT, NULL, DB_FORMAT_TEXT};
        struct DbParam* params[] = {&param, NULL};
        struct DbQuery clear = {"delete from store_stale where redis_key = $1",
                                params};
        struct DbResult r2;
        param.value = db_row_str(res.data, i, "redis_key");
        if (redis_client_del(main_redis, param.value) < 0)
            break;
        db_query_sync(NULL, &clear, &r2);
        db_result_free(&r2);
    }
    if (i)
        log("store: %u stale cached record%s cleared", i, i == 1 ? "" : "s");
    db_result_free(&res);
}

int store_init(void)
{
    struct StoreConn* conn = conn_current();

    store_conn_free(conn);
    if (!redis_client_connect(main_redis)) {
        log("store: cannot connect to Redis: %s",
            redis_client_error(main_redis));
        return 0;
    }
    clear_stale();
    if (!store_writer_start(store_conn_copy(conn_template), writes_done)) {
        log("store: cannot start the writer thread");
        return 0;
    }
    store_up = 1;
    sweep_timer = add_timeout_ms(SWEEP_INTERVAL_MS, sweep, 1);
    retry_timer = add_timeout(REDIS_RETRY_SECS, redis_retry, 1);
    return 1;
}

void store_flush_all(void)
{
    TypeNode* tn;
    Entry* e;

    for (tn = types; tn; tn = tn->next) {
        for (e = tn->type->state->all; e; e = e->next)
            entry_flush(tn->type, e);
    }
}

int store_pending(void)
{
    int i, n = 0;

    for (i = 0; i < PENDING_BUCKETS; i++) {
        Pending* p;
        for (p = pending[i]; p; p = p->next) {
            if (!p->pg_done)
                n++;
        }
    }
    return n;
}

void store_wait(int ms)
{
    time_t deadline = time(NULL) + (ms + 999) / 1000;

    /* No worker threads (before the fork, or after the shutdown): the
     * main thread writes them itself. */
    if (!worker_enabled())
        store_writer_drain_sync();
    while (store_up && store_pending() > 0 && time(NULL) < deadline &&
           worker_enabled())
        worker_wait(100);
    if (store_pending() > 0)
        log("store: %d write%s could not reach the database before exit",
            store_pending(), store_pending() == 1 ? "" : "s");
}

void store_shutdown(void)
{
    int i;

    if (sweep_timer)
        del_timeout(sweep_timer);
    if (retry_timer)
        del_timeout(retry_timer);
    sweep_timer = retry_timer = NULL;
    store_writer_stop();
    store_up = 0;
    while (types)
        store_unregister(types->type);
    for (i = 0; i < PENDING_BUCKETS; i++) {
        while (pending[i])
            pending_remove(pending[i]);
    }
    /* Fetches with a batch in flight are freed when it comes back, if it
     * does (everything they refer to is gone: fetch_done() checks). */
    {
        Fetch *f, *next;
        for (f = fetches; f; f = next) {
            next = f->next;
            f->cancelled = 1;
            f->type = NULL;
            if (f->outstanding == 0)
                fetch_free(f);
        }
        fetches_queued = 0;
    }
    if (main_redis)
        redis_client_close(main_redis);
    main_redis = NULL;
    store_conn_free(conn_template);
    conn_template = NULL;
}

void store_report(void)
{
    TypeNode* tn;

    for (tn = types; tn; tn = tn->next) {
        struct StoreTypeState_* st = tn->type->state;
        log("store: %s: %u in memory; %lu reads (%lu memory, %lu pending,"
            " %lu redis, %lu database, %lu missing), %lu prefetched,"
            " %lu writes, %lu deletes",
            tn->type->name, st->count, st->st_gets, st->st_mem,
            st->st_pending, st->st_redis, st->st_pg, st->st_missing,
            st->st_prefetched, st->st_writes, st->st_deletes);
    }
    log("store: %d pending write%s, %u queued", pending_count,
        pending_count == 1 ? "" : "s", store_writer_queued());
}
