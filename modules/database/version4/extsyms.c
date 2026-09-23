/* Interface to external module (*Serv) symbols for the database/version4
 * module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * database/version4 reads and writes the data of NickServ, ChanServ,
 * MemoServ and StatServ, which may be loaded after it (or not at all), so
 * it cannot link against their symbols.  Each imported function is called
 * through a pointer, __dblocal_<func>, that starts out pointing at a stub
 * with the function's own signature: the first call looks the symbol up
 * in its module, rebinds the pointer and forwards the call.  When the
 * module is unloaded the pointer goes back to the stub.
 */

#include "services.h"
#include "modules.h"

#define IN_EXTSYMS_C
#include "extsyms.h"

/*************************************************************************/

static Module *module_nickserv;
static Module *module_chanserv;
static Module *module_memoserv;
static Module *module_statserv;

static const char *this_module_name = "(unknown-module)";

/*************************************************************************/

/* Look up `symbol' in module `modname' (cached in `*module'); abort if it
 * is not there, since the caller has no way to continue without it. */
static void *resolve(Module **module, const char *modname, const char *symbol)
{
    void *ptr = NULL;

    if (!*module)
        *module = find_module(modname);
    if (*module)
        ptr = get_module_symbol(*module, symbol);
    if (!ptr)
        fatal("%s: undefined symbol `%s'", this_module_name, symbol);
    return ptr;
}

/* Import a function returning a value / returning nothing.  `params' is
 * the parenthesized parameter list, `args' the matching argument list. */
#define IMPORT_FUNC(modname, module, ret, func, params, args)          \
    static ret __dblocal_##func##_stub params;                          \
    typeof(func) *__dblocal_##func = __dblocal_##func##_stub;           \
    static ret __dblocal_##func##_stub params                           \
    {                                                                   \
        __dblocal_##func = resolve(&module, modname, #func);            \
        return __dblocal_##func args;                                   \
    }
#define IMPORT_PROC(modname, module, func, params, args)               \
    static void __dblocal_##func##_stub params;                         \
    typeof(func) *__dblocal_##func = __dblocal_##func##_stub;           \
    static void __dblocal_##func##_stub params                          \
    {                                                                   \
        __dblocal_##func = resolve(&module, modname, #func);            \
        __dblocal_##func args;                                          \
    }

/* Import a variable, falling back to `default' while its module is not
 * loaded. */
#define IMPORT_VAR_MAYBE(modname, module, var, default)                \
    static typeof(var) *__dblocal_##var##_ptr;                          \
    typeof(var) __dblocal_get_##var(void)                               \
    {                                                                   \
        if (!__dblocal_##var##_ptr) {                                   \
            if (!module)                                                \
                module = find_module(modname);                          \
            if (module)                                                 \
                __dblocal_##var##_ptr = get_module_symbol(module, #var);\
        }                                                               \
        return __dblocal_##var##_ptr ? *__dblocal_##var##_ptr : default;\
    }

/*************************************************************************/

IMPORT_VAR_MAYBE("chanserv/main", module_chanserv, CSMaxReg, CHANMAX_DEFAULT)
IMPORT_VAR_MAYBE("memoserv/main", module_memoserv, MSMaxMemos, MEMOMAX_DEFAULT)

IMPORT_PROC("nickserv/main", module_nickserv, del_nickinfo,
            (NickInfo *ni), (ni))
IMPORT_FUNC("nickserv/main", module_nickserv, NickInfo *, get_nickinfo,
            (const char *nick), (nick))
IMPORT_FUNC("nickserv/main", module_nickserv, NickInfo *, put_nickinfo,
            (NickInfo *ni), (ni))
IMPORT_PROC("nickserv/main", module_nickserv, del_nickgroupinfo,
            (NickGroupInfo *ngi), (ngi))
IMPORT_FUNC("nickserv/main", module_nickserv, NickGroupInfo *,
            get_nickgroupinfo, (uint32 id), (id))
IMPORT_FUNC("nickserv/main", module_nickserv, NickGroupInfo *,
            put_nickgroupinfo, (NickGroupInfo *ngi), (ngi))
IMPORT_FUNC("nickserv/main", module_nickserv, NickGroupInfo *,
            first_nickgroupinfo, (void), ())
IMPORT_FUNC("nickserv/main", module_nickserv, NickGroupInfo *,
            next_nickgroupinfo, (void), ())
IMPORT_FUNC("nickserv/main", module_nickserv, NickGroupInfo *, _get_ngi,
            (const NickInfo *ni, const char *file, int line),
            (ni, file, line))
IMPORT_FUNC("nickserv/main", module_nickserv, NickGroupInfo *, _get_ngi_id,
            (uint32 id, const char *file, int line), (id, file, line))
IMPORT_FUNC("chanserv/main", module_chanserv, ChannelInfo *, get_channelinfo,
            (const char *chan), (chan))
IMPORT_FUNC("chanserv/main", module_chanserv, ChannelInfo *, put_channelinfo,
            (ChannelInfo *ci), (ci))
IMPORT_PROC("chanserv/main", module_chanserv, reset_levels,
            (ChannelInfo *ci), (ci))
IMPORT_FUNC("statserv/main", module_statserv, ServerStats *, get_serverstats,
            (const char *servername), (servername))
IMPORT_FUNC("statserv/main", module_statserv, ServerStats *, put_serverstats,
            (ServerStats *ss), (ss))

/*************************************************************************/
/*************************************************************************/

/* Forget the symbols of a module that goes away. */
static int do_unload_module(Module *mod)
{
    if (mod == module_nickserv) {
        module_nickserv = NULL;
        __dblocal_del_nickinfo        = __dblocal_del_nickinfo_stub;
        __dblocal_get_nickinfo        = __dblocal_get_nickinfo_stub;
        __dblocal_put_nickinfo        = __dblocal_put_nickinfo_stub;
        __dblocal_del_nickgroupinfo   = __dblocal_del_nickgroupinfo_stub;
        __dblocal_get_nickgroupinfo   = __dblocal_get_nickgroupinfo_stub;
        __dblocal_put_nickgroupinfo   = __dblocal_put_nickgroupinfo_stub;
        __dblocal_first_nickgroupinfo = __dblocal_first_nickgroupinfo_stub;
        __dblocal_next_nickgroupinfo  = __dblocal_next_nickgroupinfo_stub;
        __dblocal__get_ngi            = __dblocal__get_ngi_stub;
        __dblocal__get_ngi_id         = __dblocal__get_ngi_id_stub;
    } else if (mod == module_chanserv) {
        module_chanserv = NULL;
        __dblocal_CSMaxReg_ptr    = NULL;
        __dblocal_get_channelinfo = __dblocal_get_channelinfo_stub;
        __dblocal_put_channelinfo = __dblocal_put_channelinfo_stub;
        __dblocal_reset_levels    = __dblocal_reset_levels_stub;
    } else if (mod == module_memoserv) {
        module_memoserv = NULL;
        __dblocal_MSMaxMemos_ptr = NULL;
    } else if (mod == module_statserv) {
        module_statserv = NULL;
        __dblocal_get_serverstats = __dblocal_get_serverstats_stub;
        __dblocal_put_serverstats = __dblocal_put_serverstats_stub;
    }
    return 0;
}

/*************************************************************************/

/* `name' is the name of the calling module (used for errors) */

int init_extsyms(const char *name)
{
    if (!add_callback(NULL, "unload module", do_unload_module))
        return 0;
    this_module_name = name;
    return 1;
}

void exit_extsyms(void)
{
    remove_callback(NULL, "unload module", do_unload_module);
}
