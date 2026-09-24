/* Versioned SQL migrations, per module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/migration.h).
 *
 * A module that needs tables of its own ships the SQL that makes them.  It
 * puts the files in a migrations/ subdirectory, the build compiles them
 * into the shared object (as the array `module_migrations'), Services check
 * them when the module is loaded, and they are applied -- automatically at
 * start-up for the modules Services cannot work without, by an operator
 * with OperServ MIGRATION for every other one.
 *
 * WHAT A MODULE SHIPS
 *
 * A module with migrations is a module built from a directory:
 *
 *     modules/<type>/<name>/migrations/v1_create_accounts.up.sql
 *     modules/<type>/<name>/migrations/v1_create_accounts.down.sql
 *     modules/<type>/<name>/migrations/v2_add_score.up.sql
 *     modules/<type>/<name>/migrations/v2_add_score.down.sql
 *
 * The rules are checked at load time, and a module that breaks any of them
 * does not load; the log says which file is wrong:
 *
 *   - the name is v<N>_<name>.<up|down>.sql, and nothing else;
 *   - <name> is letters, digits and underscores, and nothing else;
 *   - <N> starts at 1 and runs without gaps or repeats (no leading zeros);
 *   - every up has a down with the same version and the same name.
 *
 * The last one is not bureaucracy.  A migration nobody can undo is a
 * migration nobody can safely apply to a live network.
 *
 * WHO APPLIES THEM
 *
 * The core's own set (src/migrations/, recorded as module "core") is
 * applied when Services start: it creates the tables everything else is
 * recorded in.  A module that declares MODULE_MIGRATIONS_AUTO -- the ones
 * that keep Services' own data (NickServ, ChanServ, OperServ, ...) -- has
 * its pending migrations applied when it is loaded, before init_module():
 * without them there would not even be an identified operator to apply
 * them by hand.  Every other module follows ircu2's rule: its migrations
 * never run by themselves.  Loading it tells Services what SQL exists; an
 * operator decides whether the database should run it, and when, with
 * OperServ MIGRATION LIST, STATUS, APPLY and REVERT.
 *
 * THE RECORD
 *
 * Every applied migration is a row in the `migrations' table, and reverting
 * one deletes its row.  What is in the table is what is applied: Services
 * keep no second opinion anywhere.
 *
 * See docs/readme.migrations.
 */

#ifndef MIGRATION_H
#define MIGRATION_H

#include <sys/types.h>

struct Module_;

/* Longest <name> of a migration, and longest module name a migration may
 * be recorded under, without the NUL. */
#define MIGRATION_NAME_LEN   64
#define MIGRATION_MODULE_LEN 64

/* The module name the core's own migrations are recorded under.  No module
 * may be called this. */
#define MIGRATION_CORE "core"

/* Most migrations one module may ship. */
#define MIGRATION_MAX 256

/* One .sql file, exactly as the build found it. */
struct MigrationFile {
    const char* mf_name; /* File name, e.g. "v1_create_accounts.up.sql" */
    const char* mf_sql;  /* Its entire contents */
};

/* One version, with both of its halves. */
struct Migration {
    unsigned int mg_version;               /* The N in vN */
    char mg_name[MIGRATION_NAME_LEN + 1];  /* The name after it */
    const char* mg_up;                     /* SQL that applies it */
    const char* mg_down;                   /* SQL that reverts it */
};

/* Everything one module ships, once checked; index i is version i+1. */
struct MigrationSet {
    char ms_module[MIGRATION_MODULE_LEN + 1];
    unsigned int ms_count;
    struct Migration* ms_list;
};

/* In a module's main source file, declares that its migrations are applied
 * when it is loaded (see WHO APPLIES THEM above). */
#define MODULE_MIGRATIONS_AUTO const int module_migrations_auto = 1

/*************************************************************************/

/* Validation (src/migration.c). */

/* Check an embedded set of files and turn it into a MigrationSet.  Returns
 * the set, or NULL: NULL with `*errstr' NULL means the module simply has no
 * migrations, which is not an error; otherwise `*errstr' names the file
 * that broke a rule (a static buffer, good until the next call). */
extern struct MigrationSet* migration_build(const char* module,
                                            const struct MigrationFile* files,
                                            const char** errstr);

/* Release a set from migration_build(). */
extern void migration_free(struct MigrationSet* set);

/* Nonzero if `name' is one no module may use. */
extern int migration_reserved_name(const char* name);

/*************************************************************************/

/* Applying them (src/migration_run.c). */

/* Apply the core's own migrations.  Blocks; called by database_init() at
 * start-up.  Returns nonzero on success (errors are logged). */
extern int migration_core_start(void);

/* Apply every pending migration of `set', in order, now.  Blocks; used for
 * modules declaring MODULE_MIGRATIONS_AUTO, when they are loaded.  Returns
 * nonzero if the schema is up to date afterwards. */
extern int migration_apply_now(const struct MigrationSet* set);

/* Log a warning if `set' has migrations that are not applied.  Blocks
 * (one query).  Used when a module without MODULE_MIGRATIONS_AUTO is
 * loaded. */
extern void migration_check_pending(const struct MigrationSet* set);

/* The OperServ commands.  Each answers asynchronously, through `reply',
 * which is called with `nick' (the operator, who may have left by then:
 * look them up again) and one line of text; `owner' is the module `reply'
 * belongs to, so that pending replies are dropped if it is unloaded. */
typedef void (*MigrationReplyFn)(const char* nick, const char* text);

extern void migration_cmd_list(struct Module_* owner, MigrationReplyFn reply,
                               const char* nick);
extern void migration_cmd_status(struct Module_* owner,
                                 MigrationReplyFn reply, const char* nick,
                                 const char* module);
extern void migration_cmd_apply(struct Module_* owner, MigrationReplyFn reply,
                                const char* nick, const char* module,
                                unsigned int upto);
extern void migration_cmd_revert(struct Module_* owner,
                                 MigrationReplyFn reply, const char* nick,
                                 const char* module, unsigned int downto);

/* Forget the replies owed through a module that is going away. */
extern void migration_drop_module(struct Module_* mod);

/* Release everything, at exit. */
extern void migration_shutdown(void);

/*************************************************************************/

#endif /* MIGRATION_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
