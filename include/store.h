/* The entity store: Services' data, in PostgreSQL, through Redis.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * PostgreSQL is the truth.  Redis is the fast copy.  Services' memory holds
 * only what is being used right now:
 *
 *     Services  =>  Redis  <=  PostgreSQL
 *
 * A module describes each kind of record it keeps (a StoreType: a nickname,
 * a nickname group, a channel) -- its tables, created by the module's
 * migrations, and how to turn the C structure into a JSON "bundle" and
 * back -- and then works with records the way Services always have:
 *
 *     NickInfo *ni = store_get(&nick_type, "alice");    // pinned
 *     ...read it, change it...
 *     store_put(&nick_type, ni);                        // unpinned
 *
 * READING.  store_get() looks in the working set first (records somebody
 * is using right now), then among the writes not yet in the database, then
 * in Redis, and only then in PostgreSQL, whose answer is copied into Redis
 * for the next time -- a missing record included ("not registered" is the
 * commonest answer, and it is cached too, for a shorter time).  Every step
 * after the first is a synchronous round trip on the main thread, with a
 * short deadline: this is for the paths that cannot go on without the
 * answer (a command).  The paths that see the whole network go by -- a user
 * connecting, a nick change, a JOIN, a burst -- use store_prefetch(), which
 * fetches in the background, in batches, and calls back when the records
 * are ready.
 *
 * THE WORKING SET.  A record stays in memory while it is pinned -- by the
 * user who is using the nick, by the channel that is registered, by a
 * timer -- and is released when the last pin goes.  Memory is bounded by
 * what is online, never by the size of the database: five million
 * registered nicknames and eighty thousand users online means at most
 * eighty thousand NickInfos in memory.
 *
 * WRITING.  A record is written when it has changed: when it is unpinned
 * for the last time, and, for records that stay pinned (the nick of a user
 * who stays online), by a sweep that looks at a few of them every second.
 * "Changed" means its bundle differs from the one last written, so no
 * caller has to remember to say so.  The new bundle becomes a pending write
 * at once -- every read of that record is served from it from then on --
 * and a writer thread, with connections of its own, writes it to
 * PostgreSQL (in transactions of many records, in order, retrying until
 * the database takes it) and then to Redis.  Only when both have it is the
 * pending write dropped, so a record never goes back in time, not even
 * when Redis or PostgreSQL were down for a while.

 * A BUNDLE is a JSON object: "main" is the row of the main table (column
 * name to value), and every child table has an array of rows under its own
 * name.  It is exactly what Redis holds, exactly what PostgreSQL builds when
 * a record is loaded (one query: json_build_object() over row_to_json()
 * and json_agg()), and exactly what is written to the tables, with
 * jsonb_populate_record(): a record has one representation.  A child marked
 * read-only is part of the bundle (and of the cached copy) but is never
 * written: it belongs to other records (the nicknames of a group, for
 * instance, which are nick records), and is only there so that loading the
 * group is one round trip.
 *
 * Everything here runs in the main thread.  Records belong to the store:
 * never free one, and never keep a pointer to one without a pin.
 *
 * See docs/readme.database.
 */

#ifndef STORE_H
#define STORE_H

#include <stddef.h>
#include <time.h>

struct json_t;
struct Module_;
struct dbfield_;

/* Longest key, and longest name of a type, table or column. */
#define STORE_KEY_MAX  255
#define STORE_NAME_MAX 63

/* A child table of a record: every row of `table' whose `fk' column is
 * the record's key, in `order' order, as an array under `name' in the
 * bundle.  Written by deleting the record's rows and inserting the
 * bundle's -- unless `readonly', for rows that belong to other records and
 * are only loaded with this one; `columns' then limits what is loaded.
 * All of these are SQL names chosen by the module, never user input. */
typedef struct {
    const char* name;    /* Member name in the bundle */
    const char* table;   /* Table (NULL: same as name) */
    const char* fk;      /* Column holding the parent's key */
    const char* order;   /* ORDER BY column(s) */
    const char* columns; /* Columns to load (NULL: all) */
    int readonly;        /* Loaded, cached, never written */
} StoreChild;

typedef struct StoreType_ StoreType;
struct StoreType_ {
    const char* name;           /* Type name; Redis key <prefix>svc:<name>:<key> */
    const char* table;          /* Main table */
    const char* key_column;     /* Its primary key column */
    const char* key_type;       /* Its SQL type ("text", "bigint") */
    const StoreChild* children; /* NULL-terminated (by name), or NULL */

    /* Build a new record from a bundle; NULL if it cannot be built. */
    void* (*decode)(struct json_t* bundle);
    /* The bundle of a record: "main" and every child (read-only children
     * may be included: they are cached, not written). */
    struct json_t* (*encode)(const void* record);
    /* Free a record the store no longer needs. */
    void (*release)(void* record);
    /* The key of a record, into `buf'. */
    void (*keyof)(const void* record, char* buf, size_t size);
    /* Normalize a key as the caller spelled it (e.g. IRC case folding),
     * into `buf'; NULL to use it as is. */
    void (*normalize)(const char* key, char* buf, size_t size);

    /* --- private to the store --- */
    struct StoreTypeState_* state;
};

/*************************************************************************/

/* Types.  A module registers its types from its `init' (after its
 * migrations, which create the tables, have been applied) and unregisters
 * them from its `fini', which writes whatever changed and drops the
 * working set of those types.  Returns nonzero on success. */
extern int store_register(StoreType* type);
extern void store_unregister(StoreType* type);

/*************************************************************************/

/* Records. */

/* The record with key `key', pinned, or NULL if there is none (or it could
 * not be read: the error is logged).  Synchronous: see READING above. */
extern void* store_get(StoreType* type, const char* key);

/* The record with key `key' if it is in the working set, pinned; NULL
 * otherwise.  Never does any I/O. */
extern void* store_peek(StoreType* type, const char* key);

/* Pin a record again (a second holder). */
extern void store_hold(StoreType* type, void* record);

/* Unpin a record; with the last pin it is written if it changed, and
 * released -- at the end of the current pass of the main loop, so that the
 * caller may still look at it until then (but must not keep it).  NULL is
 * accepted. */
extern void store_put(StoreType* type, void* record);

/* A new record, which the caller built: written at once, and returned
 * pinned.  Fails (returns 0, and the record is released) if a record with
 * the same key exists -- or might: when neither Redis nor PostgreSQL can
 * say that the key is free, nothing is created, since the new record
 * would replace the real one once the database answers again. */
extern int store_add(StoreType* type, void* record);

/* Delete a record: from the database, from Redis (which then remembers it
 * as missing), and from memory -- the record is released now, whatever its
 * pins; holders must drop their pointers, as they always had to. */
extern void store_delete(StoreType* type, void* record);

/* Write a record now if it changed, without unpinning it. */
extern void store_sync(StoreType* type, void* record);

/* How many records of `type' are in memory right now. */
extern unsigned int store_resident(StoreType* type);

/* Forget a cached copy (after a change made in the database by other
 * means): the next store_get() reads PostgreSQL. */
extern void store_invalidate(StoreType* type, const char* key);

/*************************************************************************/

/* Going through many records, for the few things that must (an operator's
 * LIST, DROPEMAIL, expiration).  Records are read from the database a page
 * at a time, bundles included, never all at once; `where' is an SQL
 * condition on the main table's columns (alias t), written by the module,
 * whose values are $2, $3... bound from `params' ($1 is the store's).
 * `fn' gets each record pinned (unpinned after it returns) and returns
 * nonzero to stop.  A record in memory or with a pending write is taken
 * from there, not from the database.  Synchronous: an operator's command,
 * not something that runs on every event.  Returns the number of records
 * seen, or -1 if the database could not be read. */
typedef int (*StoreEachFn)(void* record, void* arg);
extern int store_foreach(StoreType* type, const char* where,
                         const char* const* params, int nparams,
                         StoreEachFn fn, void* arg);

/* The number of records matching `where' (as above; NULL for all), or -1
 * if the database could not be read. */
extern long store_count(StoreType* type, const char* where,
                        const char* const* params, int nparams);

/*************************************************************************/

/* Fetching in the background, for the hot paths.  Every key of `keys' is
 * looked up (Redis in batches, PostgreSQL for the misses) by a worker
 * thread; when all are known, `done' is called in the main thread with
 * `arg', and a store_get() of any of those keys made from it does no I/O.
 * `owner' is THIS_MODULE: the call is dropped if it is unloaded.  Returns
 * nonzero if `done' will be called (possibly before this returns, when
 * every key was in memory already). */
typedef void (*StoreFetchFn)(void* arg);
extern int store_prefetch(struct Module_* owner, StoreType* type,
                          const char** keys, int nkeys, StoreFetchFn done,
                          void* arg);

/*************************************************************************/

/* Helpers for encode/decode: a C structure described by a DBField array
 * (see databases.h) as a JSON row, and back.  Column names are the field
 * names with anything that is not a letter, a digit or an underscore turned
 * into an underscore; a PASSWORD field is two columns, <name> (bytea, as
 * "\x..." text) and <name>_cipher.  Strings that are not valid UTF-8 are
 * converted from ISO-8859-1.  load_only fields are decoded, not encoded. */
extern struct json_t* store_encode_fields(const void* record,
                                          const struct dbfield_* fields);
extern void store_decode_fields(struct json_t* row, void* record,
                                const struct dbfield_* fields);

/* An IRC wildcard pattern (`*', `?') as an SQL LIKE pattern, with `%',
 * `_' and `\' escaped, into `buf'.  Returns `buf'.  Bound as a value, of
 * course: `where t.nick_key like $2'. */
extern char* store_like_pattern(const char* glob, char* buf, size_t size);

/* A JSON string from a C string (NULL gives JSON null), converting it from
 * ISO-8859-1 if it is not valid UTF-8. */
extern struct json_t* store_json_string(const char* str);

/* A new C string (smalloc'd) from a JSON value, or NULL for null/missing. */
extern char* store_dup_string(struct json_t* value);

/*************************************************************************/

/* Core interface.  Not for modules. */

extern int store_init(void);         /* after the database and Redis */
extern void store_flush_all(void);   /* write everything that changed */
extern void store_collect(void);     /* release the records unpinned in
                                      * this pass of the main loop */
extern void store_wait(int ms);      /* wait for the writer to catch up */
extern int store_pending(void);      /* writes not yet in the database */
extern void store_shutdown(void);
extern void store_drop_module(struct Module_* mod);
extern void store_report(void);      /* one line per type to the log */

/*************************************************************************/

#endif /* STORE_H */
