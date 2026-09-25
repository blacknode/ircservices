/* The module loader (modules.h).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "cache.h"
#include "conffile.h"
#include "db.h"
#include "encrypt.h"
#include "migration.h"
#include "store.h"
#include "worker.h"

#include <dlfcn.h>

/*************************************************************************/

struct Module_ {
    Module *next, *prev;
    char* name;              /* As loadmodule gave it: "nickserv/main" */
    void* dll_handle;        /* From dlopen() */
    const ModuleInfo* info;  /* Its `module_info' */
    ConfigDirective* config; /* ModuleInfo.config plus one directive per
                              * pseudo-client, or NULL (allocated) */
    Module** holders;        /* Modules holding this one, one entry per
                              * hold (module_hold()) */
    int holders_count;
    struct MigrationSet* migrations; /* Its `module_migrations', checked */
};

/* Loaded modules, the most recently loaded first. */
static Module* module_list;

/* Handle of the main program, for symbol lookups. */
static void* program_handle;

static Event* module_loaded_event;
static Event* module_unloaded_event;

/*************************************************************************/
/*************************************************************************/

int module_system_init(void)
{
    program_handle = dlopen(NULL, RTLD_NOW);
    module_loaded_event = event_declare(NULL, EVENT_MODULE_LOADED);
    module_unloaded_event = event_declare(NULL, EVENT_MODULE_UNLOADED);
    if (!module_loaded_event || !module_unloaded_event) {
        log("modules: unable to declare the module events");
        return 0;
    }
    return 1;
}

void module_system_cleanup(void)
{
    module_unload_all();
    event_retract(module_unloaded_event);
    event_retract(module_loaded_event);
    module_unloaded_event = module_loaded_event = NULL;
}

/*************************************************************************/
/*************************************************************************/

Module* module_find(const char* name)
{
    Module* module;

    if (!name)
        return NULL;
    LIST_SEARCH(module_list, name, name, strcmp, module);
    return module;
}

const char* module_name(const Module* module)
{
    return module ? module->name : "core";
}

unsigned int module_flags(const Module* module)
{
    return module ? module->info->flags : 0;
}

const struct MigrationSet* module_migrations(const Module* module)
{
    return module ? module->migrations : NULL;
}

/*************************************************************************/

int module_has_symbol(Module* module, const char* symbol, void** value)
{
    void* found = NULL;

    (void)dlerror(); /* Clear any previous error */
    if (module) {
        found = dlsym(module->dll_handle, symbol);
    }
    else {
        Module* other;
        LIST_FOREACH(other, module_list)
        {
            found = dlsym(other->dll_handle, symbol);
            if (found)
                break;
        }
        if (!found)
            found = dlsym(program_handle, symbol);
    }
    if (dlerror() != NULL || !found)
        return 0;
    if (value)
        *value = found;
    return 1;
}

void* module_symbol(Module* module, const char* symbol)
{
    void* value;

    if (!module_has_symbol(module, symbol, &value)) {
        log("modules: symbol `%s' not found in %s", symbol,
            module ? module->name : "any module");
        return NULL;
    }
    return value;
}

/*************************************************************************/

/* Nonzero if `module' holds `other', directly or through the modules it
 * holds. */
static int holds_through(const Module* module, const Module* other)
{
    const Module* held;
    int i;

    LIST_FOREACH(held, module_list)
    {
        ARRAY_FOREACH(i, held->holders)
        {
            if (held->holders[i] == module &&
                (held == other || holds_through(held, other)))
                return 1;
        }
    }
    return 0;
}

int module_hold(Module* holder, Module* held)
{
    if (!held) {
        log("BUG: module_hold() from %s without a module",
            module_name(holder));
        return 0;
    }
    if (holder == held) {
        log("BUG: module_hold(): %s tried to hold itself", held->name);
        return 0;
    }
    if (holder && holds_through(held, holder)) {
        log("BUG: module_hold(): %s and %s would hold each other",
            module_name(holder), held->name);
        return 0;
    }
    ARRAY_EXTEND(held->holders);
    held->holders[held->holders_count - 1] = holder;
    return 1;
}

void module_release(Module* holder, Module* held)
{
    int i;

    if (!held)
        return;
    ARRAY_SEARCH_PLAIN_SCALAR(held->holders, holder, i);
    if (i >= held->holders_count) {
        log("BUG: module_release(): %s does not hold %s",
            module_name(holder), held->name);
        return;
    }
    ARRAY_REMOVE(held->holders, i);
}

/* Release every hold `module' has on other modules. */
static void release_all_holds(Module* module)
{
    Module* held;
    int i;

    LIST_FOREACH(held, module_list)
    {
        ARRAY_FOREACH(i, held->holders)
        {
            if (held->holders[i] == module) {
                ARRAY_REMOVE(held->holders, i);
                i--;
            }
        }
    }
}

/*************************************************************************/
/*************************************************************************/

/* Build the directive table the module's block is read with: the
 * module's own directives, then `<directive> = <nick>, <description>;'
 * for each of its pseudo-clients.  NULL if there are none. */
static ConfigDirective* build_config(const ModuleInfo* info)
{
    ConfigDirective* table;
    int own = 0, services = 0, i;

    while (info->config && info->config[own].name)
        own++;
    while (info->services && info->services[services])
        services++;
    if (!own && !services)
        return NULL;

    table = scalloc(own + services + 1, sizeof(*table));
    for (i = 0; i < own; i++)
        table[i] = info->config[i];
    for (i = 0; i < services; i++) {
        struct Service* service = info->services[i];
        ConfigDirective* directive = &table[own + i];
        directive->name = service->directive;
        directive->params[0].type = CD_STRING;
        directive->params[0].flags = CF_DIRREQ;
        directive->params[0].ptr = &service->nick;
        directive->params[1].type = CD_STRING;
        directive->params[1].ptr = &service->description;
    }
    return table;
}

/*************************************************************************/

/* Check the migrations a module ships (the array `module_migrations',
 * embedded by the build from its migrations/ directory), and apply the
 * pending ones if it has MODULE_APPLY_MIGRATIONS; otherwise only say that
 * some are pending.  Returns zero if the module must not load. */
static int load_migrations(Module* module)
{
    const struct MigrationFile* files = NULL;
    const char* error = NULL;

    if (migration_reserved_name(module->name)) {
        log("modules: `%s' is a reserved name", module->name);
        return 0;
    }
    if (!module_has_symbol(module, "module_migrations", (void**)&files) ||
        !files)
        return 1;
    module->migrations = migration_build(module->name, files, &error);
    if (!module->migrations) {
        if (!error)
            return 1; /* No migrations at all */
        log("modules: Could not load %s: %s", module->name, error);
        return 0;
    }
    if (module->info->flags & MODULE_APPLY_MIGRATIONS) {
        if (!migration_apply_now(module->migrations)) {
            log("modules: Could not load %s: its migrations could not be"
                " applied",
                module->name);
            return 0;
        }
    }
    else {
        migration_check_pending(module->migrations);
    }
    return 1;
}

/*************************************************************************/

/* Drop everything a module has in flight -- worker tasks and threads,
 * database queries, cache calls -- so that nothing calls into its code once
 * it is unmapped.  Waits for a task of the module's that a worker thread is
 * running right now. */
static void drop_module_work(Module* module)
{
    worker_cancel_module(module);
    db_drop_module(module);
    cache_drop_module(module);
    migration_drop_module(module);
    store_drop_module(module);
    password_drop_module(module);
}

/*************************************************************************/

/* Undo whatever module_load() or a module had set up, and free it.  The
 * module must already be off the module list. */
static void free_module(Module* module, int services_introduced)
{
    event_forget_module(module);
    service_detach_module(module, services_introduced);
    drop_module_work(module);
    release_all_holds(module);
    deconfigure(module->config);
    free(module->config);
    migration_free(module->migrations);
    if (module->dll_handle)
        dlclose(module->dll_handle);
    free(module->holders);
    free(module->name);
    free(module);
}

/*************************************************************************/

/* Open the shared object and check its description.  Returns the module,
 * not yet on the module list, or NULL. */
static Module* open_module(const char* name)
{
    char path[PATH_MAX + 1];
    Module *module, *other;
    const ModuleInfo* info = NULL;
    Module*** self_slot = NULL; /* Address of its `module_self_slot' */
    void* handle;

    snprintf(path, sizeof(path), "%s/modules/%s.so", services_dir, name);
    handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        const char* error = dlerror();
        /* A module using the symbols of one it requires cannot even be
         * opened before that one is loaded, so its `requires' is never
         * read: say what it most likely means. */
        log("modules: Unable to load module `%s': %s%s", name,
            error ? error : "unknown error",
            error && strstr(error, "undefined symbol")
                ? " (is a module it requires loaded after it? check the"
                  " order of the loadmodule lines)"
                : "");
        return NULL;
    }
    module = scalloc(sizeof(*module), 1);
    module->name = sstrdup(name);
    module->dll_handle = handle;

    /* A symbol found at the same address as one of an already loaded
     * module's is that module's, not this one's. */
    if (module_has_symbol(module, "module_info", (void**)&info)) {
        LIST_SEARCH_SCALAR(module_list, info, info, other);
        if (other)
            info = NULL;
    }
    if (!info) {
        log("modules: Unable to load module `%s': it has no `module_info'",
            name);
        goto fail;
    }
    if (info->abi != MODULE_ABI) {
        log("modules: Unable to load module `%s': built for module interface"
            " %u, Services have %u; recompile it",
            name, info->abi, MODULE_ABI);
        goto fail;
    }
    if (!module_has_symbol(module, "module_self_slot", (void**)&self_slot) ||
        !self_slot || !*self_slot) {
        log("modules: Unable to load module `%s': no `module_self_slot'"
            " (was it built with the Services build system?)",
            name);
        goto fail;
    }
    module->info = info;
    **self_slot = module; /* The module's THIS_MODULE */
    return module;

fail:
    dlclose(handle);
    free(module->name);
    free(module);
    return NULL;
}

/*************************************************************************/

Module* module_load(const char* name)
{
    Module* module;
    const ModuleInfo* info;
    int i;

    if (!name || !*name || strstr(name, "../")) {
        log("modules: Attempt to load bad module name: %s",
            name ? name : "(null)");
        return NULL;
    }
    if (module_find(name)) {
        log("modules: Attempt to load module `%s' twice", name);
        return NULL;
    }
    log_debug(1, "Loading module `%s'", name);

    module = open_module(name);
    if (!module)
        return NULL;
    info = module->info;
    LIST_INSERT(module, module_list);

    for (i = 0; info->requires && info->requires[i]; i++) {
        Module* required = module_find(info->requires[i]);
        if (!required) {
            log("modules: %s requires %s: load it first (loadmodule \"%s\""
                " before loadmodule \"%s\")",
                name, info->requires[i], info->requires[i], name);
            goto fail;
        }
        if (!module_hold(module, required))
            goto fail;
    }

    /* Its migrations: checked now, and applied now if the module cannot
     * work without them (see migration.h). */
    if (!load_migrations(module))
        goto fail;

    module->config = build_config(info);
    if (!configure(name, module->config, CONFIGURE_READ | CONFIGURE_SET)) {
        log("modules: Could not load %s: its configuration has errors",
            name);
        goto fail;
    }
    if (!service_attach_module(module, info->services))
        goto fail;

    if (info->init && !info->init(module)) {
        log("modules: Could not load %s: it failed to start", name);
        /* Let it undo whatever it did before failing. */
        if (info->fini)
            info->fini(module, 0);
        goto fail;
    }

    service_introduce_module(module);
    log_debug(1, "Successfully loaded module `%s'", name);
    event_emit(module_loaded_event, module, module->name);
    return module;

fail:
    LIST_REMOVE(module, module_list);
    free_module(module, 0);
    return NULL;
}

/*************************************************************************/

/* Unload a module.  `shutdown' is nonzero when Services are exiting, in
 * which case its `fini' cannot refuse. */
static int unload_module(Module* module, int shutdown)
{
    if (!module) {
        log("BUG: module_unload() with a NULL module");
        return 0;
    }
    if (module->holders_count > 0) {
        log("modules: Cannot unload %s: %s needs it%s", module->name,
            module_name(module->holders[0]),
            module->holders_count > 1 ? " (and others)" : "");
        return 0;
    }
    log_debug(1, "Unloading module `%s'", module->name);

    if (module->info->fini && !module->info->fini(module, shutdown)) {
        if (!shutdown) {
            log("modules: %s refused to be unloaded", module->name);
            return 0;
        }
        log("modules: %s failed to stop cleanly on shutdown; unloading it"
            " anyway",
            module->name);
    }

    /* Off the list first, so that nothing new reaches it. */
    LIST_REMOVE(module, module_list);
    event_emit(module_unloaded_event, module);
    free_module(module, 1);
    return 1;
}

int module_unload(Module* module)
{
    return unload_module(module, 0);
}

/*************************************************************************/

void module_unload_all(void)
{
    /* The most recently loaded module that nothing holds, again and again:
     * a module is always loaded after the ones it requires, but a hold
     * taken at run time can point the other way. */
    for (;;) {
        Module* module;
        LIST_FOREACH(module, module_list)
        {
            if (module->holders_count == 0)
                break;
        }
        if (!module)
            break;
        unload_module(module, 1);
    }
    if (module_list) {
        Module* module;
        log("modules: BUG: modules still holding each other at shutdown:");
        LIST_FOREACH(module, module_list)
        log("modules: -- %s (held by %s)", module->name,
            module_name(module->holders[0]));
    }
}

/*************************************************************************/

int module_reconfigure_all(void)
{
    Module *module, *oldest = NULL;

    service_reconfigure_begin();
    LIST_FOREACH(module, module_list)
    {
        if (!configure(module->name, module->config, CONFIGURE_READ)) {
            service_reconfigure_end();
            return 0;
        }
    }
    LIST_FOREACH(module, module_list)
    {
        configure(module->name, module->config, CONFIGURE_SET);
        oldest = module;
    }
    service_reconfigure_end();

    /* Tell the modules, in the order they were loaded. */
    for (module = oldest; module; module = module->prev) {
        if (module->info->rehash)
            module->info->rehash(module);
    }
    event_report_undeclared();
    return 1;
}

/*************************************************************************/

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
