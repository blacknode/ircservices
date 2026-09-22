/* Header for extsyms.c.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef EXTSYMS_H
#define EXTSYMS_H

/*************************************************************************/

#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/memoserv/memoserv.h"
#include "modules/statserv/statserv.h"

#define DEFINE_LOCAL_VAR(sym)  extern typeof(sym) __dblocal_get_##sym(void)
#define DEFINE_LOCAL_FUNC(sym)  extern typeof(sym) *__dblocal_##sym

DEFINE_LOCAL_VAR(CSMaxReg);
DEFINE_LOCAL_VAR(MSMaxMemos);
DEFINE_LOCAL_FUNC(del_nickinfo);
DEFINE_LOCAL_FUNC(get_nickinfo);
DEFINE_LOCAL_FUNC(put_nickinfo);
DEFINE_LOCAL_FUNC(del_nickgroupinfo);
DEFINE_LOCAL_FUNC(get_nickgroupinfo);
DEFINE_LOCAL_FUNC(put_nickgroupinfo);
DEFINE_LOCAL_FUNC(first_nickgroupinfo);
DEFINE_LOCAL_FUNC(next_nickgroupinfo);
DEFINE_LOCAL_FUNC(_get_ngi);
DEFINE_LOCAL_FUNC(_get_ngi_id);
DEFINE_LOCAL_FUNC(get_channelinfo);
DEFINE_LOCAL_FUNC(put_channelinfo);
DEFINE_LOCAL_FUNC(reset_levels);
DEFINE_LOCAL_FUNC(get_serverstats);
DEFINE_LOCAL_FUNC(put_serverstats);

#ifndef IN_EXTSYMS_C
# define CSMaxReg               ((*__dblocal_get_CSMaxReg)())
# define MSMaxMemos             ((*__dblocal_get_MSMaxMemos)())
# define del_nickinfo           (*__dblocal_del_nickinfo)
# define get_nickinfo           (*__dblocal_get_nickinfo)
# define put_nickinfo           (*__dblocal_put_nickinfo)
# define del_nickgroupinfo      (*__dblocal_del_nickgroupinfo)
# define get_nickgroupinfo      (*__dblocal_get_nickgroupinfo)
# define put_nickgroupinfo      (*__dblocal_put_nickgroupinfo)
# define first_nickgroupinfo    (*__dblocal_first_nickgroupinfo)
# define next_nickgroupinfo     (*__dblocal_next_nickgroupinfo)
# define _get_ngi               (*__dblocal__get_ngi)
# define _get_ngi_id            (*__dblocal__get_ngi_id)
# define get_channelinfo        (*__dblocal_get_channelinfo)
# define put_channelinfo        (*__dblocal_put_channelinfo)
# define reset_levels           (*__dblocal_reset_levels)
# define get_serverstats        (*__dblocal_get_serverstats)
# define put_serverstats        (*__dblocal_put_serverstats)
#endif

extern int init_extsyms(const char *name);
extern void exit_extsyms(void);

/*************************************************************************/

#endif /* EXTSYMS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
