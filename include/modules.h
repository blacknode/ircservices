/* The loadable-module interface.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * A module is a shared object that Services open with dlopen() when a
 * `loadmodule' directive names it.  It describes itself with exactly one
 * exported object, `module_info' (a ModuleInfo), and Services reach
 * everything else through it: what it needs loaded first, its settings,
 * the pseudo-clients it provides, and the functions to start, reload and
 * stop it.  The model is ircu2's (include/module.h there).
 *
 * Everything a module attaches through this interface -- event handlers
 * (events.h), pseudo-clients (service.h), holds on other modules -- is
 * tracked against the module and undone when it is unloaded, whether or
 * not its `fini' remembers to do it.
 *
 * See docs/readme.mod_api for a guided tour and complete examples.
 */

#ifndef MODULES_H
#define MODULES_H

#include "services.h"
#include "conffile.h"

/*************************************************************************/

/* Version of this interface.  A module built against a different value is
 * refused: recompile it. */
#define MODULE_ABI 2

/* A loaded module.  Opaque: use the functions below. */
struct Module_;
typedef struct Module_ Module;

/* A pseudo-client (see service.h). */
struct Service;

/*************************************************************************/

/* Flags for ModuleInfo.flags. */

/* Apply the module's pending migrations when it is loaded, before `init'
 * (for modules that keep Services' own data; see docs/readme.migrations).
 * Without this flag, pending migrations are only reported. */
#define MODULE_APPLY_MIGRATIONS 0x0001

/*************************************************************************/

/* The description every module exports as `module_info'. */

typedef struct ModuleInfo {
    /* MODULE_ABI, as the module was compiled. */
    unsigned int abi;

    /* One line saying what the module does. */
    const char* description;

    /* Modules that must be loaded before this one, NULL-terminated (build
     * it with MODULE_REQUIRES()), or NULL.  Each is held while this module
     * is loaded, so it cannot be unloaded from under it. */
    const char* const* requires;

    /* Settings of the module's `module "<name>" { }' block, or NULL.  Read
     * before `init' and again on every REHASH. */
    ConfigDirective* config;

    /* Pseudo-clients the module provides, NULL-terminated (build it with
     * MODULE_SERVICES()), or NULL for a module without one.  Each one's
     * nick and description are read from the module block with its own
     * directive (see service.h). */
    struct Service* const* services;

    /* Bitwise combination of MODULE_* flags, or 0. */
    unsigned int flags;

    /* Start the module, after its settings have been read.  Return
     * nonzero on success; zero aborts the load (everything the module
     * attached is undone). */
    int (*init)(Module* module);

    /* Stop the module.  `shutdown' is nonzero when Services are exiting.
     * Return nonzero to allow the unload; zero refuses it (ignored on
     * shutdown).  May be NULL. */
    int (*fini)(Module* module, int shutdown);

    /* The configuration was read again (REHASH) and the new settings are
     * in place.  May be NULL. */
    void (*rehash)(Module* module);
} ModuleInfo;

/* Helpers for the NULL-terminated lists of a ModuleInfo:
 *     .requires = MODULE_REQUIRES("operserv/main", "nickserv/main"),
 *     .services = MODULE_SERVICES(&operserv_service, &global_service),
 */
#define MODULE_REQUIRES(...) ((const char* const[]){__VA_ARGS__, NULL})
#define MODULE_SERVICES(...) ((struct Service* const[]){__VA_ARGS__, NULL})

/*************************************************************************/

/* THIS_MODULE is the calling module's own handle (NULL in the core), and
 * MODULE_NAME its name.  The handle is filled in by the loader; see the
 * internals at the end of this file. */

#define RENAME_SYMBOL(symbol)       RENAME_SYMBOL_2(symbol, MODULE_ID)
#define RENAME_SYMBOL_2(symbol, id) RENAME_SYMBOL_3(symbol, id)
#define RENAME_SYMBOL_3(symbol, id) symbol##_##id

#ifdef MODULE
#define THIS_MODULE RENAME_SYMBOL(module_self)
#else
#define THIS_MODULE NULL
#endif

#define MODULE_NAME (module_name(THIS_MODULE))

/*************************************************************************/
/*************************************************************************/

/* Finding out about modules. */

/* The loaded module called `name' ("nickserv/main"), or NULL. */
extern Module* module_find(const char* name);

/* The name a module was loaded by; "core" for NULL. */
extern const char* module_name(const Module* module);

/* The MODULE_* flags of its ModuleInfo. */
extern unsigned int module_flags(const Module* module);

/* The migrations the module ships, validated when it was loaded, or NULL
 * if it ships none (see migration.h). */
struct MigrationSet;
extern const struct MigrationSet* module_migrations(const Module* module);

/* Look up an exported symbol of `module' (of every loaded module and the
 * core if `module' is NULL).  module_symbol() logs a missing symbol and
 * returns NULL; module_has_symbol() stays quiet, returns nonzero if the
 * symbol exists, and stores its value in `*value' if `value' is not NULL.
 * A module listed in `requires' may simply use the other module's symbols
 * directly (they are resolved when the module is loaded). */
extern void* module_symbol(Module* module, const char* symbol);
extern int module_has_symbol(Module* module, const char* symbol,
                             void** value);

/* Hold `held' on behalf of `holder': a held module cannot be unloaded.
 * For dependencies found at run time; the ones in `requires' are held by
 * the loader.  Every hold is released when the holder is unloaded. */
extern int module_hold(Module* holder, Module* held);
extern void module_release(Module* holder, Module* held);

/*************************************************************************/

/* Loading and unloading (the core, and OperServ). */

/* Load the module `name' ("<type>/<name>", as loadmodule gives it) and
 * start it.  Returns its handle, or NULL on error (logged). */
extern Module* module_load(const char* name);

/* Stop and unload a module.  Fails (returns zero) if another module holds
 * it or its `fini' refuses. */
extern int module_unload(Module* module);

/* Unload every module, most recently loaded first. */
extern void module_unload_all(void);

/* Read every loaded module's settings again (REHASH).  Returns nonzero on
 * success; on failure no module's settings have changed. */
extern int module_reconfigure_all(void);

/* Start and stop the module system (init.c). */
extern int module_system_init(void);
extern void module_system_cleanup(void);

/*************************************************************************/
/*************************************************************************/

/* Internals: the handle slot the loader fills in. */

#ifdef MODULE
#ifndef MODULE_MAIN_FILE
extern
#endif
    Module* RENAME_SYMBOL(module_self);
#ifdef MODULE_MAIN_FILE
Module** module_self_slot = &RENAME_SYMBOL(module_self);
#endif
#endif

/*************************************************************************/

#include "events.h"
#include "service.h"

#endif /* MODULES_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
