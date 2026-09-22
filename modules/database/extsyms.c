/* Interface to external module (*Serv) symbols for the database/version4
 * module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"

#define IN_EXTSYMS_C
#include "extsyms.h"

/*************************************************************************/

#ifdef __INTEL_COMPILER  /* icc hides the symbols from the inline asm */
# define static
#endif
static Module *module_nickserv;
static Module *module_chanserv;
static Module *module_memoserv;
static Module *module_statserv;
#ifdef __INTEL_COMPILER
# undef static
#endif

static const char *this_module_name = "(unknown-module)";

/*************************************************************************/

static void fatal_no_symbol(const char *symbol)
{
    fatal("%s: undefined symbol `%s'", this_module_name, symbol);
}

/*************************************************************************/
/*************************************************************************/

#if GCC3_HACK

# if defined(__sparc__)

#  define IMPORT_FUNC(modulename, module, func)                         \
static void __dblocal_##func##_stub0(void) {                            \
    void *ptr = NULL;                                                   \
    if (!module)                                                        \
        module = find_module(modulename);                               \
    if (module)                                                         \
        ptr = get_module_symbol(module, #func);                         \
    if (!ptr)                                                           \
        fatal_no_symbol(#func);                                         \
    __dblocal_##func = ptr;                                             \
}                                                                       \
static void *__dblocal_##func##_stub1(void) {                           \
    __dblocal_##func##_stub0();                                         \
    __builtin_return(__builtin_apply((void *)__dblocal_##func,          \
                     __builtin_apply_args(), 64));                      \
}                                                                       \
static void __dblocal_##func##_stub(void) {                             \
    (void) __builtin_apply((void *)__dblocal_##func##_stub1,            \
                           __builtin_apply_args(), 64);                 \
    asm("ld [%sp-128],%i0");                                            \
}                                                                       \
typeof(func) *__dblocal_##func = (typeof(func) *) __dblocal_##func##_stub;

#elif defined(__POWERPC__)

#  define IMPORT_FUNC(modulename, module, func)                         \
static void __dblocal_##func##_stub0(void) {                            \
    void *ptr = NULL;                                                   \
    if (!module)                                                        \
        module = find_module(modulename);                               \
    if (module)                                                         \
        ptr = get_module_symbol(module, #func);                         \
    if (!ptr)                                                           \
        fatal_no_symbol(#func);                                         \
    __dblocal_##func = ptr;                                             \
}                                                                       \
static void __dblocal_##func##_stub(void) {                             \
    asm("lwz r0,0(r1)\n                                                 \
         stwu r0,-104(r1)\n                                             \
         stw r3,64(r1)\n                                                \
         stw r4,68(r1)\n                                                \
         stw r5,72(r1)\n                                                \
         stw r6,76(r1)\n                                                \
         stw r7,80(r1)\n                                                \
         stw r8,84(r1)\n                                                \
         stw r9,88(r1)\n                                                \
         stw r10,92(r1)\n                                               \
         mflr r0\n                                                      \
         stw r0,100(r1)");                                              \
    asm("mtctr %0\n                                                     \
         bctrl\n                                                        \
         lwz r10,92(r1)" : : "r" (__dblocal_##func##_stub0));           \
    asm("stw %0,96(r1)" : : "r" (__dblocal_##func));                    \
    asm("lwz r3,64(r1)\n                                                \
         lwz r4,68(r1)\n                                                \
         lwz r5,72(r1)\n                                                \
         lwz r6,76(r1)\n                                                \
         lwz r7,80(r1)\n                                                \
         lwz r8,84(r1)\n                                                \
         lwz r9,88(r1)\n                                                \
         lwz r10,92(r1)\n                                               \
         lwz r11,96(r1)\n                                               \
         mtctr r11\n                                                    \
         bctrl\n                                                        \
         lwz r0,100(r1)\n                                               \
         mtlr r0\n                                                      \
         mr r0,r3");                                                    \
}                                                                       \
typeof(func) *__dblocal_##func = (typeof(func) *) __dblocal_##func##_stub;

#else  /* not SPARC and not PowerPC... must be our good old friend */

#  define IMPORT_FUNC(modulename, module, func)                         \
static void __dblocal_##func##_stub0(void) {                            \
    void *ptr = NULL;                                                   \
    if (!module)                                                        \
        module = find_module(modulename);                               \
    if (module)                                                         \
        ptr = get_module_symbol(module, #func);                         \
    if (!ptr)                                                           \
        fatal_no_symbol(#func);                                         \
    __dblocal_##func = ptr;                                             \
}                                                                       \
static void *__dblocal_##func##_stub(void) {                            \
    __dblocal_##func##_stub0();                                         \
    __builtin_return(__builtin_apply((void *)__dblocal_##func,          \
                     __builtin_apply_args(), 64));                      \
}                                                                       \
typeof(func) *__dblocal_##func = (typeof(func) *) __dblocal_##func##_stub;

#endif /* to SPARC or not to SPARC */

#elif defined(__GNUC__)

#define IMPORT_FUNC(modulename, module, func)                           \
static void *__dblocal_##func##_stub(void) {                            \
    void *ptr = NULL;                                                   \
    if (!module)                                                        \
        module = find_module(modulename);                               \
    if (module)                                                         \
        ptr = get_module_symbol(module, #func);                         \
    if (!ptr)                                                           \
        fatal_no_symbol(#func);                                         \
    __dblocal_##func = ptr;                                             \
    __builtin_return(__builtin_apply(ptr, __builtin_apply_args(), 64)); \
}                                                                       \
typeof(func) *__dblocal_##func = (typeof(func) *) __dblocal_##func##_stub;

#elif defined(__INTEL_COMPILER)

volatile void *_fatal_no_symbol = (void *)fatal_no_symbol;

#define IMPORT_FUNC(modulename, module, func)   \
static void __dblocal_##func##_stub(void) {     \
asm("   movl    "#module",%eax;                 \
        testl   %eax,%eax;                      \
        jnz     1f;                             \
        leal    8f,%eax;                        \
        pushl   %eax;                           \
        call    find_module;                    \
        leal    4(%esp),%esp;                   \
        movl    %eax,"#module";                 \
1:      testl   %eax,%eax;                      \
        jz      2f;                             \
        leal    9f,%edx;                        \
        pushl   %edx;                           \
        pushl   %eax;                           \
        call    get_module_symbol;              \
        leal    8(%esp),%esp;                   \
2:      testl   %eax,%eax;                      \
        jnz     3f;                             \
        leal    9f,%eax;                        \
        pushl   %eax;                           \
        call    *_fatal_no_symbol;              \
        leal    4(%esp),%esp;                   \
3:      movl    %eax,__dblocal_"#func";         \
        movl    %ebp,%esp;                      \
        popl    %ebp;                           \
        jmp     *%eax;                          \
8:      .ascii  \"" modulename "\\0\";          \
9:      .ascii  \""#func"\\0\"                  \
");                                             \
}                                               \
typeof(func) *__dblocal_##func = (typeof(func) *) __dblocal_##func##_stub;

#else

#error IMPORT_FUNC not implemented for this compiler

#endif  /* GCC3_HACK etc */


#define IMPORT_VAR(modulename, module, var)                             \
static typeof(var) *__dblocal_##var##_ptr;                              \
typeof(var) __dblocal_get_##var(void) {                                 \
    if (!__dblocal_##var##_ptr) {                                       \
        if (!module)                                                    \
            module = find_module(modulename);                           \
        if (module)                                                     \
            __dblocal_##var##_ptr = get_module_symbol(module, #var);    \
        if (!__dblocal_##var##_ptr)                                     \
            fatal_no_symbol(#var);                                      \
    }                                                                   \
    return *__dblocal_##var;                                            \
}


#define IMPORT_VAR_MAYBE(modulename, module, var, default)              \
static typeof(var) *__dblocal_##var##_ptr;                              \
typeof(var) __dblocal_get_##var(void) {                                 \
    if (!__dblocal_##var##_ptr) {                                       \
        if (!module)                                                    \
            module = find_module(modulename);                           \
        if (module)                                                     \
            __dblocal_##var##_ptr = get_module_symbol(module, #var);    \
    }                                                                   \
    return __dblocal_##var##_ptr ? *__dblocal_##var##_ptr : default;    \
}

/*************************************************************************/

IMPORT_VAR_MAYBE("chanserv/main", module_chanserv, CSMaxReg, CHANMAX_DEFAULT);
IMPORT_VAR_MAYBE("memoserv/main", module_memoserv, MSMaxMemos,MEMOMAX_DEFAULT);

IMPORT_FUNC("nickserv/main", module_nickserv, del_nickinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, get_nickinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, put_nickinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, del_nickgroupinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, get_nickgroupinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, put_nickgroupinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, first_nickgroupinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, next_nickgroupinfo);
IMPORT_FUNC("nickserv/main", module_nickserv, _get_ngi);
IMPORT_FUNC("nickserv/main", module_nickserv, _get_ngi_id);
IMPORT_FUNC("chanserv/main", module_chanserv, get_channelinfo);
IMPORT_FUNC("chanserv/main", module_chanserv, put_channelinfo);
IMPORT_FUNC("chanserv/main", module_chanserv, reset_levels);
IMPORT_FUNC("statserv/main", module_statserv, get_serverstats);
IMPORT_FUNC("statserv/main", module_statserv, put_serverstats);

/*************************************************************************/
/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_nickserv) {
        module_nickserv = NULL;
        __dblocal_del_nickinfo = (void *) __dblocal_del_nickinfo_stub;
        __dblocal_get_nickinfo = (void *) __dblocal_get_nickinfo_stub;
        __dblocal_put_nickinfo = (void *) __dblocal_put_nickinfo_stub;
        __dblocal_del_nickgroupinfo =
            (void *) __dblocal_del_nickgroupinfo_stub;
        __dblocal_get_nickgroupinfo =
            (void *) __dblocal_get_nickgroupinfo_stub;
        __dblocal_put_nickgroupinfo =
            (void *) __dblocal_put_nickgroupinfo_stub;
        __dblocal_first_nickgroupinfo =
            (void *) __dblocal_first_nickgroupinfo_stub;
        __dblocal_next_nickgroupinfo =
            (void *) __dblocal_next_nickgroupinfo_stub;
        __dblocal__get_ngi = (void *) __dblocal__get_ngi_stub;
        __dblocal__get_ngi_id = (void *) __dblocal__get_ngi_id_stub;
    } else if (mod == module_chanserv) {
        module_chanserv = NULL;
        __dblocal_CSMaxReg_ptr = NULL;
        __dblocal_get_channelinfo = (void *) __dblocal_get_channelinfo_stub;
        __dblocal_put_channelinfo = (void *) __dblocal_put_channelinfo_stub;
        __dblocal_reset_levels = (void *) __dblocal_reset_levels_stub;
    } else if (mod == module_memoserv) {
        module_memoserv = NULL;
        __dblocal_MSMaxMemos_ptr = NULL;
    } else if (mod == module_statserv) {
        module_statserv = NULL;
        __dblocal_get_serverstats = (void *) __dblocal_get_serverstats_stub;
        __dblocal_put_serverstats = (void *) __dblocal_put_serverstats_stub;
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

/*************************************************************************/

void exit_extsyms()
{
    remove_callback(NULL, "unload module", do_unload_module);
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
