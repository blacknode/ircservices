/* Module support include file (interface definition).
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MODULES_H
#define MODULES_H

/*************************************************************************/

/* Version code macro.  Parameters are:
 *      major: Major version number (the "5" in 5.1a0).
 *      minor: Minor version number (the "1" in 5.1a0).
 *     status: Release status (alpha, prerelease, or final).  Use one of
 *             the MODULE_VERSION_{ALPHA,PRE,FINAL} macros:
 *                5.1a0   -> MODULE_VERSION_ALPHA
 *                5.1pre0 -> MODULE_VERSION_PRE
 *                5.1.0   -> MODULE_VERSION_FINAL
 *    release: Release number (the "0" in 5.1a0).
 */
#define MODULE_VERSION(major,minor,status,release) (            \
    ((major) > 5 || ((major) == 5 && (minor) >= 1))             \
        ? ((major)<<24 | (minor)<<16 | (status)<<12 | (release))\
        : (0x050000 | (release))                                \
)
#define MODULE_VERSION_ALPHA    0xA
#define MODULE_VERSION_PRE      0xB
#define MODULE_VERSION_FINAL    0xF

/* Version code for modules.  This will be updated whenever a change to the
 * program (structures, callbacks, etc.) makes existing binary modules
 * incompatible.  This is stored in the compiled modules and used to let
 * Services determine whether a given module is compatible with the current
 * binary; it can also be used with #if to make source code compatible with
 * multiple versions of Services.
 */
#define MODULE_VERSION_CODE     MODULE_VERSION(5,1,MODULE_VERSION_FINAL,0)

/*************************************************************************/

/* Module information.  The actual structure is defined in module.c, and is
 * opaque to the caller. */
struct Module_;
typedef struct Module_ Module;

/* Callback function prototype. */
typedef int (*callback_t)();

/* Callback priority limits. */
#define CBPRI_MIN       -10000
#define CBPRI_MAX       10000

/*************************************************************************/

/* Macro to rename a symbol based on the module's ID (MODULE_ID) in order
 * to avoid symbol clashes.  This roundabout construction is necessary to
 * force the preprocessor to substitute the value of MODULE_ID instead of
 * appending "MODULE_ID" literally. */

#define RENAME_SYMBOL(symbol) RENAME_SYMBOL_2(symbol,MODULE_ID)
#define RENAME_SYMBOL_2(symbol,id) RENAME_SYMBOL_3(symbol,id)
#define RENAME_SYMBOL_3(symbol,id) symbol##_##id

/*************************************************************************/

/* Macros to retrieve a pointer to the current module or its name. */

#ifdef MODULE
# define THIS_MODULE RENAME_SYMBOL(_this_module)
#else
# define THIS_MODULE NULL
#endif

#define MODULE_NAME (get_module_name(THIS_MODULE))

/*************************************************************************/
/*************************************************************************/

/* External variable/function declarations.  Note that many of the
 * functions below are macroized to automatically pass a "current-module"
 * parameter; functions whose names begin with "_" should not be called
 * directly. */

/*************************************************************************/

/* Initialization and cleanup: */

extern int modules_init(int ac, char **av);
extern void modules_cleanup(void);
extern void unload_all_modules(void);

/*************************************************************************/

/* Module-level functions: */

/* Load a new module and return the Module pointer, or NULL on error. */
extern Module *load_module(const char *modulename);

/* Remove a module from memory.  Return nonzero on success, zero on
 * failure. */
extern int unload_module(Module *module);

/* Return the Module pointer for the named module, or NULL if no such
 * module exists. */
extern Module *find_module(const char *modulename);

/* Increment or decrement the use count for the given module.  A module
 * cannot be unloaded while its use count is nonzero. */
#define use_module(mod)   _use_module(mod,THIS_MODULE)
#define unuse_module(mod) _unuse_module(mod,THIS_MODULE)
extern void _use_module(Module *module, const Module *caller);
extern void _unuse_module(Module *module, const Module *caller);

/* Re-read configuration files for all modules.  Return nonzero on success,
 * zero on failure. */
int reconfigure_modules(void);

/*************************************************************************/

/* Module symbol/information retrieval: */

/* Retrieve the value of the named symbol in the given module.  Return NULL
 * if no such symbol exists.  Note that this function should not be used
 * for symbols whose value might be NULL, because there is no way to
 * distinguish a symbol value of NULL from an error return.  For such
 * symbols, or for cases where a symbol might legitimately not exist and
 * no error should be printed for nonexistence, use check_module_symbol(). */
#define get_module_symbol(mod,symname) \
    _get_module_symbol(mod,symname,THIS_MODULE)
extern void *_get_module_symbol(Module *module, const char *symname,
                                const Module *caller);

/* Check whether the given symbol exists in the given module; return 1 if
 * so, 0 otherwise.  If `resultptr' is non-NULL and the symbol exists, the
 * value is stored in the variable it points to.  If `errorptr' is non-NULL
 * and the symbol does not exist, a human-readable error message is stored
 * in the variable it points to. */
extern int check_module_symbol(Module *module, const char *symname,
                               void **resultptr, const char **errorptr);

/* Retrieve the name of the given module. */
extern const char *get_module_name(const Module *module);

/*************************************************************************/

/* Callback-related functions: (all functions except register_callback()
 * and call_callback() return nonzero on success and zero on error)
 */

/* Register a new callback list. */
#define register_callback(name) _register_callback(THIS_MODULE,name)
extern int _register_callback(Module *module, const char *name);

/* Call all functions in a callback list.  Return 1 if a callback returned
 * nonzero, 0 if all callbacks returned zero, or -1 on error.  The _N
 * formats allow passing parameters. */
#define call_callback(id) \
    call_callback_1(id, NULL)
#define call_callback_1(id,arg1) \
    call_callback_2(id, arg1, NULL)
#define call_callback_2(id,arg1,arg2) \
    call_callback_3(id, arg1, arg2, NULL)
#define call_callback_3(id,arg1,arg2,arg3) \
    call_callback_4(id, arg1, arg2, arg3, NULL)
#define call_callback_4(id,arg1,arg2,arg3,arg4) \
    call_callback_5(id, arg1, arg2, arg3, arg4, NULL)
#define call_callback_5(id,arg1,arg2,arg3,arg4,arg5) \
    _call_callback_5(THIS_MODULE, id, (void *)(long)(arg1), \
                     (void *)(long)(arg2), (void *)(long)(arg3), \
                     (void *)(long)(arg4), (void *)(long)(arg5))
extern int _call_callback_5(Module *module, int id, void *arg1, void *arg2,
                            void *arg3, void *arg4, void *arg5);

/* Delete a callback list. */
#define unregister_callback(name) _unregister_callback(THIS_MODULE,name)
extern int _unregister_callback(Module *module, int id);

/* Add a function to a callback list with the given priority (higher
 * priority value = called sooner).  Callbacks with the same priority are
 * called in the order they were added. */
#define add_callback_pri(module,name,callback,priority) \
    _add_callback_pri(module,name,callback,priority,THIS_MODULE)
int _add_callback_pri(Module *module, const char *name, callback_t callback,
                      int priority, const Module *caller);

/* Add a function to a callback list with priority 0. */
#define add_callback(module,name,callback) \
    add_callback_pri(module,name,callback,0)

/* Remove a function from a callback list. */
#define remove_callback(module,name,callback) \
    _remove_callback(module,name,callback,THIS_MODULE)
extern int _remove_callback(Module *module, const char *name,
                           callback_t callback, const Module *caller);

/*************************************************************************/
/*************************************************************************/

/* Module functions: */

int init_module(void);
int exit_module(int shutdown);

/*************************************************************************/

/* Macros to declare a symbol to be exported.  Only one may be used per
 * line, and it must be placed at the beginning of the line and be the only
 * thing on the line (no semicolon at the end).  This does not have any
 * actual effect on compilation, but such lines are extracted to create
 * module symbol lists.  Note that it is not necessary to explicitly list
 * the init_module, exit_module, and module_config symbols (and in fact,
 * doing so will cause an error when linking the final executable).
 *
 * Also note that typedefs cannot be used here; use struct tags instead.
 *
 * Examples:
 *     EXPORT_VAR(const char *,s_NickServ)
 *     EXPORT_ARRAY(some_array)
 *     EXPORT_FUNC(create_akill)
 */

#define EXPORT_VAR(type,symbol)
#define EXPORT_ARRAY(symbol)
#define EXPORT_FUNC(symbol)

/*************************************************************************/
/*************************************************************************/

/* Internal-use stuff. */

/*************************************************************************/

/* Pointer to the current module.  This is defined in each module's main
 * source file, and automatically initialized by load_module().  Modules
 * should use the THIS_MODULE macro to retrieve their own module pointer
 * rather than accessing this variable directly.
 */
#ifdef MODULE
# ifndef MODULE_MAIN_FILE
extern
# endif
Module *RENAME_SYMBOL(_this_module);
# ifdef MODULE_MAIN_FILE
Module **_this_module_ptr = &RENAME_SYMBOL(_this_module);  /* used by loader */
# endif
#endif

/*************************************************************************/

/* If the preprocessor symbol MODULE_MAIN_FILE is defined, the following
 * required variable is automatically defined.  This symbol should be
 * defined for one (and only one) file per module.  (Normally, this symbol
 * is defined automatically by modules/Makerules for the main source file
 * for each module, and does not need to be defined manually.)
 */
#if defined(MODULE_MAIN_FILE)
const int32 module_version = MODULE_VERSION_CODE;
#endif

/*************************************************************************/

#endif  /* MODULES_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
