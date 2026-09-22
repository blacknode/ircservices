/* Module support.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#undef use_module
#undef unuse_module

#if !STATIC_MODULES
# include <dlfcn.h>
#endif

/*************************************************************************/

/* Internal structure for callbacks. */
typedef struct callbackinfo_ CallbackInfo;
struct callbackinfo_ {
    char *name;
    int calling;  /* used by {call,remove}_callback() for safe callback
                   * removal from inside the callback */
    struct {
        callback_t func;
        const Module *adder;
        int pri;
    } *funcs;
    int funcs_count;
};

/* Structure for module data. */
struct Module_ {
    Module *next, *prev;
    char *name;                 /* Module name (path passed to load_module())*/
    ConfigDirective *modconfig; /* `module_config' in this module */
    Module ***this_module_pptr; /* `_this_module_ptr' in this module */
    const int32 *module_version_ptr;  /* `module_version' in this module */
    void *dllhandle;            /* Handle used by dynamic linker */
    CallbackInfo *callbacks;
    int callbacks_count;
    const Module **users;       /* Array of module's users (use_module()) */
    int users_count;
};


/* Module data for Services core. */
static Module coremodule = {.name = "core"};


/* Global list of modules. */
static Module *modulelist = &coremodule;


/* Callbacks for loading, unloading, and reconfiguring modules. */
static int cb_load_module = -1;
static int cb_unload_module = -1;
static int cb_reconfigure = -1;


/*************************************************************************/

#if STATIC_MODULES

/* Structure for a module symbol. */
struct modsym {
    const char *symname;
    void *value;
};

/* Static module information (from modules/modules.a): */
struct modinfo {
    const char *modname;
    struct modsym *modsyms;
};
extern struct modinfo modlist[];

#else  /* !STATIC_MODULES */

static void *program_handle;    /* Handle for the main program */

#endif  /* STATIC_MODULES */

/*************************************************************************/

/* Internal routine declarations: */

static Module *internal_load_module(const char *modulename);
static int internal_init_module(Module *module);
static int internal_unload_module(Module *module, int shutdown);

/*************************************************************************/

/* Translate the NULL module to &coremodule, and ensure that any given
 * module is in fact on the module list, aborting the function otherwise.
 * Call as:
 *     VALIDATE_MOD(module)
 * or
 *     VALIDATE_MOD(module, return_value)
 * where `module' is a variable holding a module handle.
 */

#define VALIDATE_MOD(__m,...)  do {         \
    if (!__m) {                             \
        __m = &coremodule;                  \
    } else {                                \
        Module *__tmp;                      \
        LIST_FOREACH (__tmp, modulelist) {  \
            if (__tmp == __m)               \
                break;                      \
        }                                   \
        if (!__tmp) {                       \
            log("%s(): module %p not on module list", __FUNCTION__, __m); \
            return __VA_ARGS__;             \
        }                                   \
    }                                       \
} while (0)

/*************************************************************************/
/********************* Initialization and cleanup ************************/
/*************************************************************************/

int modules_init(int ac, char **av)
{
#if !STATIC_MODULES
    program_handle = dlopen(NULL, 0);
#endif
    cb_load_module   = register_callback("load module");
    cb_unload_module = register_callback("unload module");
    cb_reconfigure   = register_callback("reconfigure");
    if (cb_load_module < 0 || cb_unload_module < 0 || cb_reconfigure < 0) {
        log("modules_init: register_callback() failed\n");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void modules_cleanup(void)
{
    int i;

    unload_all_modules();
    unregister_callback(cb_reconfigure);
    unregister_callback(cb_unload_module);
    unregister_callback(cb_load_module);
    ARRAY_FOREACH(i, coremodule.callbacks) {
        if (coremodule.callbacks[i].name) {
            log("modules: Core forgot to unregister callback `%s'",
                coremodule.callbacks[i].name);
            free(coremodule.callbacks[i].name);
            free(coremodule.callbacks[i].funcs);
        }
    }
    free(coremodule.callbacks);
}

/*************************************************************************/

void unload_all_modules(void)
{
    /* Normally it would be sufficient to iterate through the module list,
     * since new modules are always inserted at the front of the list, but
     * it is possible for an older module to lock a newer module via the
     * "load module" callback, resulting in unload failures if that simple
     * method was used.  Instead, we repeatedly search the module list for
     * the first unloadable module (a module that is not the core module
     * and is not locked), unload that module, and repeat until no
     * unloadable modules are left.  (In theory, since a module's locks are
     * forcibly when the module is unloaded, this should only leave the
     * core module, but we check anyway just to be safe.) */

    for (;;) {
        Module *mod;
        LIST_FOREACH (mod, modulelist) {
            if (strcmp(mod->name, "core") != 0) {
                int i;
                ARRAY_FOREACH (i, mod->users) {
                    if (mod->users[i] != mod)
                        break;
                }
                if (i >= mod->users_count) {
                    /* This module has no users (except possibly itself),
                     * so unload it */
                    break;
                }
            }
        }
        if (!mod)
            break;
        if (!internal_unload_module(mod, 1)) {
            log("modules: Failed to unload `%s' on exit", mod->name);
            /* Unlink it anyway, but don't free the structure to avoid
             * segfaults in the module.  This is an impossible case
             * anyway, so we don't worry about the leak. */
            LIST_REMOVE(mod, modulelist);
        }
    }

    if (!modulelist) {
        log("modules: BUG: core module got removed from module list during"
            " shutdown!");
    } else if (modulelist != &coremodule || modulelist->next != NULL) {
        Module *mod;
        log("modules: BUG: failed to unload some modules during shutdown"
            " (circular lock?)");
        LIST_FOREACH (mod, modulelist) {
            if (strcmp(mod->name, "core") != 0) {
                log("modules: -- module %s not unloaded", mod->name);
            }
        }
    }
}

/*************************************************************************/
/*********************** Low-level module routines ***********************/
/*************************************************************************/

/* These low-level routines take care of all changes in processing with
 * regard to dynamic vs. static modules and different platforms. */

/* Common variables: */

#if STATIC_MODULES
static const char *dl_last_error;
#endif

/*************************************************************************/

/* Low-level routine to open a module and return a handle. */

static void *my_dlopen(const char *name)
{
#if !STATIC_MODULES

    char pathname[PATH_MAX+1];
    snprintf(pathname, sizeof(pathname), "%s/modules/%s.so",
             services_dir, name);
    return dlopen(pathname, RTLD_NOW | RTLD_GLOBAL);

#else  /* STATIC_MODULES */

    int i;

    for (i = 0; modlist[i].modname; i++) {
        if (strcmp(modlist[i].modname, name) == 0)
            break;
    }
    if (!modlist[i].modname) {
        dl_last_error = "Module not found";
        return NULL;
    }
    return &modlist[i];

#endif /* STATIC_MODULES */
} /* my_dlopen() */

/*************************************************************************/

/* Low-level routine to close a module. */

static void my_dlclose(void *handle)
{
#if !STATIC_MODULES

    dlclose(handle);

#else  /* STATIC_MODULES */

    /* nothing */

#endif /* STATIC_MODULES */
} /* my_dlclose() */

/*************************************************************************/

/* Low-level routine to retrieve a symbol from a module given its handle. */

static void *my_dlsym(void *handle, const char *symname)
{
#if !STATIC_MODULES

# if SYMS_NEED_UNDERSCORES
    char buf[256];
    if (strlen(symname) > sizeof(buf)-2) {  /* too long for buffer */
        log("modules: symbol name too long in my_dlsym(): %s", symname);
        return NULL;
    }
    snprintf(buf, sizeof(buf), "_%s", symname);
    symname = buf;
# endif
    if (handle) {
        return dlsym(handle, symname);
    } else {
        Module *mod;
        void *ptr;
        LIST_FOREACH (mod, modulelist) {
            ptr = dlsym(mod->dllhandle?mod->dllhandle:program_handle, symname);
            if (ptr)
                return ptr;
        }
        return NULL;
    }

#else  /* STATIC_MODULES */

    int i;

    if (handle) {
        struct modsym *syms = ((struct modinfo *)handle)->modsyms;
        for (i = 0; syms[i].symname; i++) {
            if (strcmp(syms[i].symname, symname) == 0)
                break;
        }
        if (!syms[i].symname) {
            dl_last_error = "Symbol not found";
            return NULL;
        }
        return syms[i].value;
    } else {
        const char *save_dl_last_error = dl_last_error;
        for (i = 0; modlist[i].modname; i++) {
            void *value = my_dlsym(&modlist[i], symname);
            if (value) {
                dl_last_error = save_dl_last_error;
                return value;
            }
        }
        dl_last_error = "Symbol not found";
        return NULL;
    }

#endif /* STATIC_MODULES */
} /* my_dlsym() */

/*************************************************************************/

/* Low-level routine to return the error message (if any) from the previous
 * call. */

static const char *my_dlerror(void)
{
#if !STATIC_MODULES

    return dlerror();

#else  /* STATIC_MODULES */

    const char *str = dl_last_error;
    dl_last_error = NULL;
    return str;

#endif /* STATIC_MODULES */
} /* my_dlerror() */

/*************************************************************************/
/************************ Module-level functions *************************/
/*************************************************************************/

/* Load a new module and return the Module pointer, or NULL on error.
 * (External interface to the above functions.)
 */

Module *load_module(const char *modulename)
{
    Module *module;


    if (!modulename) {
        log("load_module(): modulename is NULL!");
        return NULL;
    }

    log_debug(1, "Loading module `%s'", modulename);

    module = internal_load_module(modulename);
    if (!module)
        return NULL;
    LIST_INSERT(module, modulelist);

    if (!configure(module->name, module->modconfig,
                   CONFIGURE_READ | CONFIGURE_SET)) {
        log("modules: configure() failed for %s", modulename);
        goto fail;
    }

    if (!internal_init_module(module)) {
        log("modules: init_module() failed for %s", modulename);
        deconfigure(module->modconfig);
        goto fail;
    }

    log_debug(1, "Successfully loaded module `%s'", modulename);
    call_callback_2(cb_load_module, module, module->name);

    /* This is a REALLY STUPID HACK to ensure protocol modules are loaded
     * first--but hey, it works! */
    if (protocol_features & PF_UNSET) {
        log("FATAL: load_module(): A protocol module must be loaded first!");
        unload_module(module);
        return NULL;
    }

    return module;

  fail:
    free(module->name);
    my_dlclose(module->dllhandle);
    LIST_REMOVE(module, modulelist);
    free(module);
    return NULL;
}

/************************************/

/* Internal routine to load a module.  Returns the module pointer or NULL
 * on error.
 */

static Module *internal_load_module(const char *modulename)
{
    void *handle;
    Module *module, *mptr;
    int32 *verptr;
    Module ***thisptr;
    ConfigDirective *confptr;

    if (strstr(modulename, "../")) {
        log("modules: Attempt to load bad module name: %s", modulename);
        goto err_return;
    }
    LIST_SEARCH(modulelist, name, modulename, strcmp, mptr);
    if (mptr) {
        log("modules: Attempt to load module `%s' twice", modulename);
        goto err_return;
    }

    handle = my_dlopen(modulename);
    if (!handle) {
        const char *error = my_dlerror();
        if (!error)
            error = "Unknown error";
        log("modules: Unable to load module `%s': %s", modulename, error);
        goto err_return;
    }

    module = scalloc(sizeof(*module), 1);
    module->dllhandle = handle;
    module->name = sstrdup(modulename);

    thisptr = NULL;
    if (check_module_symbol(module, "_this_module_ptr", (void **)&thisptr,
                            NULL)){
#if !defined(STATIC_MODULES)
        /* When using dynamic linking, the above may return the first
         * instance of `_this_module_ptr' found in _any_ module (though
         * giving priority to the given module), so we need to check if
         * we've seen this address before.  With static linking, the result
         * will be NULL if the symbol does not exist in this specific
         * module, so the extra check is unnecessary. */
        LIST_SEARCH_SCALAR(modulelist, this_module_pptr, thisptr, mptr);
        if (mptr)
            thisptr = NULL;
#endif
    }
    if (!thisptr) {
        log("modules: Unable to load module `%s': No `_this_module_ptr' symbol"
            " found", modulename);
        goto err_freemod;
    }
    module->this_module_pptr = thisptr;
    **thisptr = module;

    verptr = NULL;  /* as above */
    if (check_module_symbol(module, "module_version", (void **)&verptr, NULL)){
#if !defined(STATIC_MODULES)
        LIST_SEARCH_SCALAR(modulelist, module_version_ptr, verptr, mptr);
        if (mptr)
            verptr = NULL;
#endif
    }
    if (!verptr) {
        log("modules: Unable to load module `%s': No `module_version'"
            " symbol found", modulename);
        goto err_freemod;
    } else if (*verptr != MODULE_VERSION_CODE) {
        log("modules: Unable to load module `%s': Version mismatch"
            " (module version = %08X, core version = %08X)",
            modulename, *verptr, MODULE_VERSION_CODE);
        goto err_freemod;
    }
    module->module_version_ptr = verptr;

    confptr = NULL;  /* as above */
    if (check_module_symbol(module, "module_config", (void **)&confptr, NULL)){
#if !defined(STATIC_MODULES)
        LIST_SEARCH_SCALAR(modulelist, modconfig, confptr, mptr);
        if (mptr)
            confptr = NULL;
#endif
    }
    module->modconfig = confptr;

    return module;

  err_freemod:
    free(module->name);
    free(module);
    my_dlclose(handle);
  err_return:
    return NULL;
}

/************************************/

/* Initialize a module.  Return the module's init_module() return value, or
 * 1 if the module does not have an init_module() function.
 */

static int internal_init_module(Module *module)
{
    int (*initfunc)(void);

    initfunc = get_module_symbol(module, "init_module");
    if (initfunc)
        return initfunc();
    else
        return 1;
}

/*************************************************************************/

/* Remove a module from memory.  Return nonzero on success, zero on
 * failure.
 */

int unload_module(Module *module)
{
    return internal_unload_module(module, 0);
}

/************************************/

/* Internal implementation of unload_module(), taking an additional
 * parameter indicating whether the unload is due to Services shutting down
 * or not.
 */

static int internal_unload_module(Module *module, int shutdown)
{
    int (*exit_module)(int shutdown);
    Module *tmp;
    int i;


    if (!module) {
        log("unload_module(): module is NULL!");
        return 0;
    }

    if (module->users_count > 0) {
        log("modules: Attempt to unload in-use module `%s' (in use by %s%s)",
            module->name, module->users[0]->name,
            module->users_count>1 ? " and others" : "");
        return 0;
    }

    log_debug(1, "Unloading module `%s'", module->name);

    /* Call the module's exit routine */
    exit_module = get_module_symbol(module, "exit_module");
    if (exit_module && !(*exit_module)(shutdown)) {
        if (shutdown) {
            log("modules: exit_module() for module `%s' returned zero on"
                " shutdown, unloading module anyway", module->name);
        } else {
            return 0;
        }
    }

    /* Remove the module from the global module list, ensuring that no
     * new callbacks or the like can be added while we unload it */
    LIST_REMOVE(module, modulelist);

    /* Ensure that callbacks and use_module() calls are properly undone */
    LIST_FOREACH (tmp, modulelist) {
        ARRAY_FOREACH (i, tmp->users) {
            if (tmp->users[i] == module) {
                log("modules: Module `%s' forgot to unuse_module() for"
                    " module `%s'", module->name, tmp->name);
                ARRAY_REMOVE(tmp->users, i);
                i--;
            }
        }
        ARRAY_FOREACH (i, tmp->callbacks) {
            int j;
            /* Don't warn for callbacks that couldn't have been removed
             * because the callback was in use (e.g. for shutting down
             * after a crash) */
            if (tmp->callbacks[i].calling)
                continue;
            ARRAY_FOREACH (j, tmp->callbacks[i].funcs) {
                if (tmp->callbacks[i].funcs[j].adder == module) {
                    log("modules: Module `%s' forgot to remove callback"
                        " `%s' from module `%s'", module->name,
                        tmp->callbacks[i].name, tmp->name);
                    ARRAY_REMOVE(tmp->callbacks[i].funcs, j);
                    j--;
                }
            }
        }
    }
    ARRAY_FOREACH (i, module->callbacks) {
        if (module->callbacks[i].name) {
            log("modules: Module `%s' forgot to unregister callback `%s'",
                module->name, module->callbacks[i].name);
            free(module->callbacks[i].name);
            free(module->callbacks[i].funcs);
        }
    }
    free(module->callbacks);

    /* Clean up and free the module data */
    call_callback_1(cb_unload_module, module);
    deconfigure(module->modconfig);
    free(module->name);
    my_dlclose(module->dllhandle);
    free(module);

    return 1;
}

/*************************************************************************/

/* Return the Module pointer for the named module, or NULL if no such
 * module exists.
 */

Module *find_module(const char *modulename)
{
    Module *result;

    if (!modulename) {
        log("find_module(): modulename is NULL!");
        return NULL;
    }
    LIST_SEARCH(modulelist, name, modulename, strcmp, result);
    return result;
}

/*************************************************************************/

/* Increment the use count for the given module.  A module cannot be
 * unloaded while its use count is nonzero.
 */

static int use_module_loopcheck(const Module *module, const Module *check)
{
    /* Return whether `module' is used by `check' (self-references are
     * ignored). */

    int i;

    ARRAY_FOREACH (i, module->users) {
        if (module->users[i] != module) {
            if (module->users[i] == check
             || use_module_loopcheck(module->users[i], check))
                return 1;
        }
    }
    return 0;
}

void _use_module(Module *module, const Module *caller)
{
    VALIDATE_MOD(module);
    VALIDATE_MOD(caller);
    if (module == caller) {
        log("modules: BUG: Module `%s' called use_module() for itself!",
            module->name);
        return;
    }
    if (use_module_loopcheck(caller, module)) {
        log("modules: BUG: use_module loop detected (called by `%s' for `%s')",
            caller->name, module->name);
        return;
    }
    ARRAY_EXTEND(module->users);
    module->users[module->users_count-1] = caller;
}

/*************************************************************************/

/* Decrement the use count for the given module.  `module' may be NULL, in
 * which case this routine does nothing.
 */

void _unuse_module(Module *module, const Module *caller)
{
    int i;

    if (!module)
        return;
    VALIDATE_MOD(module);
    VALIDATE_MOD(caller);
    if (module == caller) {
        log("modules: BUG: Module `%s' called unuse_module() for itself!",
            module->name);
        return;
    }
    if (module->users_count == 0) {
        log("modules: BUG: trying to unuse module `%s' with use count 0"
            " from module `%s'", module->name, caller->name);
        return;
    }
    ARRAY_SEARCH_PLAIN_SCALAR(module->users, caller, i);
    if (i >= module->users_count) {
        log("modules: BUG: trying to unuse module `%s' from module `%s' but"
            " caller not found in user list!", module->name, caller->name);
        return;
    }
    ARRAY_REMOVE(module->users, i);
}

/*************************************************************************/

/* Reconfigure all modules.  The "reconfigure" callback is called with an
 * `int' parameter of 0 before reconfiguration and 1 after.  Returns 1 on
 * success, 0 on failure (on failure, all modules' configuration data will
 * be left alone).
 */

int reconfigure_modules(void)
{
    Module *mod;

    call_callback_1(cb_reconfigure, 0);
    LIST_FOREACH (mod, modulelist) {
        if (!configure(mod->name, mod->modconfig, CONFIGURE_READ))
            return 0;
    }
    LIST_FOREACH (mod, modulelist)
        configure(mod->name, mod->modconfig, CONFIGURE_SET);
    call_callback_1(cb_reconfigure, 1);
    return 1;
}

/*************************************************************************/
/****************** Module symbol/information retrieval ******************/
/*************************************************************************/

/* Retrieve the value of the named symbol in the given module.  Return NULL
 * if no such symbol exists.  Note that this function should not be used
 * for symbols whose value might be NULL, because there is no way to
 * distinguish a symbol value of NULL from an error return.  For such
 * symbols, or for cases where a symbol might legitimately not exist and
 * no error should be printed for nonexistence, use check_module_symbol().
 */

void *_get_module_symbol(Module *module, const char *symname,
                         const Module *caller)
{
    void *value;

    if (!check_module_symbol(module, symname, &value, NULL)) {
        if (module) {
            log("%s: Unable to resolve symbol `%s' in module `%s'",
                get_module_name(caller), symname, get_module_name(module));
        } else {
            log("%s: Unable to resolve symbol `%s'",
                get_module_name(caller), symname);
        }
        return NULL;
    }
    return value;
}

/*************************************************************************/

/* Check whether the given symbol exists in the given module; return 1 if
 * so, 0 otherwise.  If `resultptr' is non-NULL and the symbol exists, the
 * value is stored in the variable it points to.  If `errorptr' is non-NULL
 * and the symbol does not exist, a human-readable error message is stored
 * in the variable it points to.
 */

int check_module_symbol(Module *module, const char *symname,
                        void **resultptr, const char **errorptr)
{
    void *value;
    const char *error;

    (void) my_dlerror();  /* clear any previous error */
    value = my_dlsym(module ? module->dllhandle : NULL, symname);
    error = my_dlerror();
    if (error) {
        if (errorptr)
            *errorptr = error;
        return 0;
    } else {
        if (resultptr)
            *resultptr = value;
        return 1;
    }
}

/*************************************************************************/

/* Retrieve the name of the given module.  If NULL is given, returns the
 * string "core".
 */

const char *get_module_name(const Module *module)
{
    return module ? module->name : "core";
}

/*************************************************************************/
/********************** Callback-related functions ***********************/
/*************************************************************************/

/* Local function to look up a callback for a module.  Returns NULL if not
 * found.
 */

static CallbackInfo *find_callback(Module *module, const char *name)
{
    int i;

    ARRAY_FOREACH (i, module->callbacks) {
        if (module->callbacks[i].name
         && strcmp(module->callbacks[i].name,name) == 0)
            break;
    }
    if (i == module->callbacks_count)
        return NULL;
    return &module->callbacks[i];
}

/*************************************************************************/

/* Register a new callback.  "module" is the calling module's own Module
 * pointer, or NULL for core Services callbacks (this is set by the
 * register_callback() macro).  Return the callback identifier (a
 * nonnegative integer) or -1 on error.
 */

int _register_callback(Module *module, const char *name)
{
    int i;

    log_debug(2, "register_callback(%s, \"%s\")",
              module ? module->name : "core", name);
    VALIDATE_MOD(module, -1);
    if (find_callback(module, name)) {
        log("BUG: register_callback(%s,\"%s\"): callback already registered",
            module ? module->name : "core", name);
        return -1;
    }
    i = module->callbacks_count;
    ARRAY_EXTEND(module->callbacks);
    module->callbacks[i].name = sstrdup(name);
    module->callbacks[i].calling = 0;
    module->callbacks[i].funcs_count = 0;
    module->callbacks[i].funcs = NULL;
    return i;
}

/*************************************************************************/

/* Call all functions hooked into a callback.  Return 1 if a callback
 * returned nonzero, 0 if all callbacks returned zero, or -1 on error.
 */

int _call_callback_5(Module *module, int id, void *arg1, void *arg2,
                     void *arg3, void *arg4, void *arg5)
{
    CallbackInfo *cl;
    int res = 0;
    int i;

    VALIDATE_MOD(module, -1);
    if (id < 0 || id >= module->callbacks_count)
        return -1;
    cl = &module->callbacks[id];
    cl->calling = 1;
    ARRAY_FOREACH (i, cl->funcs) {
        res = cl->funcs[i].func(arg1, arg2, arg3, arg4, arg5);
        if (res != 0)
            break;
    }
    if (cl->calling == 2) {  /* flag indicating some callbacks were removed */
        ARRAY_FOREACH (i, cl->funcs) {
            if (!cl->funcs[i].func) {
                ARRAY_REMOVE(cl->funcs, i);
                i--;
            }
        }
    }
    cl->calling = 0;
    return res;
}

/*************************************************************************/

/* Delete a callback. */

int _unregister_callback(Module *module, int id)
{
    CallbackInfo *cl;

    VALIDATE_MOD(module, 0);
    log_debug(2, "unregister_callback(%s, %d)", module->name, id);
    if (id < 0 || id >= module->callbacks_count) {
        log("unregister_callback(): BUG: invalid callback ID %d for module"
            " `%s'", id, module->name);
        return 0;
    }
    cl = &module->callbacks[id];
    if (!cl->name) {
        log("unregister_callback(): BUG: callback ID %d for module `%s'"
            " is unused (double unregister?)", id, module->name);
        return 0;
    }
    free(cl->funcs);
    free(cl->name);
    cl->funcs = NULL;
    cl->name = NULL;
    return 1;
}

/*************************************************************************/

/* Add (hook) a function into to a callback with the given priority (higher
 * priority value = called sooner).  Callbacks with the same priority are
 * called in the order they were added.
 */

int _add_callback_pri(Module *module, const char *name, callback_t callback,
                      int priority, const Module *caller)
{
    CallbackInfo *cl;
    int n;

    log_debug(2, "add_callback_pri(%s, \"%s\", %p, %d)",
              module ? module->name : "core", name ? name : "(null)",
              callback, priority);
    VALIDATE_MOD(module, 0);
    VALIDATE_MOD(caller, 0);
    cl = find_callback(module, name);
    if (!cl) {
        log_debug(2, "-- callback not found");
        return 0;
    }
    if (priority < CBPRI_MIN || priority > CBPRI_MAX) {
        log("add_callback_pri(): priority (%d) out of range for callback"
            " `%s' in module `%s'",
            priority, name, module ? module->name : "core");
        return 0;
    }
    ARRAY_FOREACH (n, cl->funcs) {
        if (cl->funcs[n].pri < priority)
            break;
    }
    ARRAY_INSERT(cl->funcs, n);
    cl->funcs[n].func = callback;
    cl->funcs[n].adder = caller;
    cl->funcs[n].pri = priority;
    return 1;
}

/*************************************************************************/

/* Remove (unhook) a function from a callback. */
int _remove_callback(Module *module, const char *name, callback_t callback,
                     const Module *caller)
{
    CallbackInfo *cl;
    int index;

    log_debug(2, "remove_callback(%s, \"%s\", %p)",
              module ? module->name : "core", name, callback);
    VALIDATE_MOD(module, 0);
    VALIDATE_MOD(caller, 0);
    cl = find_callback(module, name);
    if (!cl)
        return 0;
    ARRAY_SEARCH_SCALAR(cl->funcs, func, callback, index);
    if (index == cl->funcs_count)
        return 0;
    if (cl->funcs[index].adder != caller) {
        log("remove_callback(%s, \"%s\"): BUG: caller `%s' tried to remove"
            " callback %p added by different module `%s'!",
            get_module_name(module), name, get_module_name(caller), callback,
            get_module_name(cl->funcs[index].adder));
        return 0;
    }
    if (cl->calling) {
        cl->funcs[index].func = NULL;
        cl->calling = 2;  /* flag to call_callback() indicating CB removed */
    } else {
        ARRAY_REMOVE(cl->funcs, index);
    }
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
