/* SOP/AOP/VOP handling for ChanServ.
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
#include "language.h"
#include "commands.h"
#include "modules/nickserv/nickserv.h"
#include "modules/operserv/operserv.h"

#include "chanserv.h"
#include "cs-local.h"

/*************************************************************************/

static Module *module_chanserv;

/*************************************************************************/

/* Return the name of the list or the syntax message number corresponding
 * to the given level.  Does no error checking. */
# define XOP_LISTNAME(level) \
    ((level)==ACCLEV_SOP ? "SOP" : (level)==ACCLEV_AOP ? "AOP" : \
     (level)==ACCLEV_HOP ? "HOP" : (level)==ACCLEV_VOP ? "VOP" : "NOP")
# define XOP_SYNTAX(level) \
    ((level)==ACCLEV_SOP ? CHAN_SOP_SYNTAX : \
     (level)==ACCLEV_AOP ? CHAN_AOP_SYNTAX : \
     (level)==ACCLEV_HOP ? CHAN_HOP_SYNTAX : \
     (level)==ACCLEV_VOP ? CHAN_VOP_SYNTAX : CHAN_NOP_SYNTAX)
# define XOP_LIST_SYNTAX(level) \
    ((level)==ACCLEV_SOP ? CHAN_SOP_LIST_SYNTAX : \
     (level)==ACCLEV_AOP ? CHAN_AOP_LIST_SYNTAX : \
     (level)==ACCLEV_HOP ? CHAN_HOP_LIST_SYNTAX : \
     (level)==ACCLEV_VOP ? CHAN_VOP_LIST_SYNTAX : CHAN_NOP_LIST_SYNTAX)

/*************************************************************************/

/* Local functions. */

static void handle_xop(User *u, int level);
static int xop_list(const User *u, int index, const ChannelInfo *ci,
                    int *sent_header);

/*************************************************************************/

static void do_sop(User *u);
static void do_aop(User *u);
static void do_hop(User *u);
static void do_vop(User *u);
static void do_nop(User *u);

static Command cmds[] = {
    { "SOP",      do_sop,      NULL,  CHAN_HELP_SOP,            -1,-1 },
    { "AOP",      do_aop,      NULL,  CHAN_HELP_AOP,            -1,-1 },
    { "VOP",      do_vop,      NULL,  CHAN_HELP_VOP,            -1,-1 },
    { "NOP",      do_nop,      NULL,  CHAN_HELP_NOP,            -1,-1 },
    { NULL }
};

static Command cmds_halfop[] = {
    { "HOP",      do_hop,      NULL,  CHAN_HELP_HOP,            -1,-1 },
    { NULL }
};

/*************************************************************************/
/*************************** The *OP commands ****************************/
/*************************************************************************/

/* SOP, VOP, AOP, and HOP wrappers.  These just call handle_xop() with the
 * appropriate level.
 */

static void do_sop(User *u)
{
    handle_xop(u, ACCLEV_SOP);
}

static void do_aop(User *u)
{
    handle_xop(u, ACCLEV_AOP);
}

static void do_hop(User *u)
{
    handle_xop(u, ACCLEV_HOP);
}

static void do_vop(User *u)
{
    handle_xop(u, ACCLEV_VOP);
}

static void do_nop(User *u)
{
    handle_xop(u, ACCLEV_NOP);
}

/*************************************************************************/

/* Central handler for all *OP commands. */

static void handle_xop_add(const User *u, int level, int is_servadmin,
                           ChannelInfo *ci, const char *nick);
static void handle_xop_del(const User *u, int level, int is_servadmin,
                           ChannelInfo *ci, const char *nick);
static void handle_xop_list(const User *u, int level, ChannelInfo *ci,
                            const char *startstr, const char *pattern);
static void handle_xop_count(const User *u, int level, ChannelInfo *ci);

static void handle_xop(User *u, int level)
{
    char *chan = strtok(NULL, " ");
    char *cmd = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    ChannelInfo *ci = NULL;
    int is_list = (cmd && (stricmp(cmd,"LIST")==0 || stricmp(cmd,"COUNT")==0));
    int is_servadmin = is_services_admin(u);

    if (!chan
     || !cmd
     || (!is_list && !nick)
     || (stricmp(cmd,"COUNT") == 0 && nick)
    ) {
        syntax_error(s_ChanServ, u, XOP_LISTNAME(level), XOP_SYNTAX(level));
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!is_servadmin
               && !check_access_cmd(u, ci, "ACCESS", is_list ? "LIST" : cmd)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "ADD") == 0) {
        handle_xop_add(u, level, is_servadmin, ci, nick);
    } else if (stricmp(cmd, "DEL") == 0) {
        handle_xop_del(u, level, is_servadmin, ci, nick);
    } else if (stricmp(cmd, "LIST") == 0) {
        int have_start = (nick && *nick=='+');
        const char *s = have_start ? strtok(NULL, " ") : nick;
        handle_xop_list(u, level, ci, have_start ? nick : NULL, s);
    } else if (stricmp(cmd, "COUNT") == 0) {
        handle_xop_count(u, level, ci);
    } else {
        syntax_error(s_ChanServ, u, XOP_LISTNAME(level), XOP_SYNTAX(level));
    }
    put_channelinfo(ci);
}


static void handle_xop_add(const User *u, int level, int is_servadmin,
                           ChannelInfo *ci, const char *nick)
{
    const char *listname = XOP_LISTNAME(level);

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
        return;
    }
    switch (access_add(ci, nick, level,
                       is_servadmin ? ACCLEV_FOUNDER : get_access(u,ci))) {
      case RET_ADDED:
        notice_lang(s_ChanServ, u, CHAN_XOP_ADDED, nick, ci->name, listname);
        break;
      case RET_CHANGED:
        notice_lang(s_ChanServ, u, CHAN_XOP_LEVEL_CHANGED,
                    nick, ci->name, listname);
        break;
      case RET_UNCHANGED:
        notice_lang(s_ChanServ, u, CHAN_XOP_LEVEL_UNCHANGED,
                    nick, ci->name, listname);
        break;
      case RET_LISTFULL:
        notice_lang(s_ChanServ, u, CHAN_XOP_REACHED_LIMIT, CSAccessMax);
        break;
      case RET_NOSUCHNICK:
        notice_lang(s_ChanServ, u, CHAN_XOP_NICKS_ONLY);
        break;
      case RET_NICKFORBID:
        notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, nick);
        break;
      case RET_NOOP:
        notice_lang(s_ChanServ, u, CHAN_XOP_NOOP, nick);
        break;
      case RET_PERMISSION:
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        break;
    }
}


static void handle_xop_del(const User *u, int level, int is_servadmin,
                           ChannelInfo *ci, const char *nick)
{
    const char *listname = XOP_LISTNAME(level);
    NickInfo *ni;
    int i;

    if (!is_servadmin && level >= get_access(u, ci)) {
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        return;
    } else if (readonly) {
        if (is_servadmin) {
            notice_lang(s_ChanServ, u, READ_ONLY_MODE);
        } else {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
            return;
        }
    }

    ni = get_nickinfo(nick);
    if (!ni) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
        return;
    }
    if (!check_ngi(ni)) {
        notice_lang(s_ChanServ, u, INTERNAL_ERROR);
        put_nickinfo(ni);
        return;
    }
    ARRAY_SEARCH_SCALAR(ci->access, nickgroup, ni->nickgroup, i);
    put_nickinfo(ni);
    if (i == ci->access_count || ci->access[i].level != level) {
        notice_lang(s_ChanServ, u, CHAN_XOP_NOT_FOUND,
                    nick, ci->name, listname);
        return;
    }
    ci->access[i].nickgroup = 0;
    notice_lang(s_ChanServ, u, CHAN_XOP_DELETED, nick, ci->name, listname);
}


static void handle_xop_list(const User *u, int level, ChannelInfo *ci,
                            const char *startstr, const char *pattern)
{
    const char *listname = XOP_LISTNAME(level);
    NickGroupInfo *ngi;
    int count = 0, sent_header = 0, skip = 0, i;

    if (startstr) {
        skip = (int)atolsafe(startstr+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_ChanServ, u, listname, XOP_LIST_SYNTAX(level));
            return;
        }
    }
    if (ci->access_count == 0) {
        notice_lang(s_ChanServ, u, CHAN_XOP_LIST_EMPTY, ci->name, listname);
        return;
    }
    ARRAY_FOREACH (i, ci->access) {
        if (!ci->access[i].nickgroup)
            continue;
        if (ci->access[i].level != level)
            continue;
        if (!(ngi = get_ngi_id(ci->access[i].nickgroup)))
            continue;
        if (pattern && !match_wild_nocase(pattern, ngi_mainnick(ngi))) {
            put_nickgroupinfo(ngi);
            continue;
        }
        put_nickgroupinfo(ngi);
        count++;
        if (count > skip && count <= skip+ListMax)
            xop_list(u, i, ci, &sent_header);
    }
    if (count) {
        int shown = count - skip;
        if (shown < 0)
            shown = 0;
        else if (count > ListMax)
            shown = ListMax;
        notice_lang(s_ChanServ, u, LIST_RESULTS, shown, count);
    } else {
        notice_lang(s_ChanServ, u, CHAN_XOP_NO_MATCH, ci->name, listname);
    }
}


static void handle_xop_count(const User *u, int level, ChannelInfo *ci)
{
    const char *listname = XOP_LISTNAME(level);
    int count = 0, i;

    if (ci->access_count == 0) {
        notice_lang(s_ChanServ, u, CHAN_XOP_LIST_EMPTY, ci->name, listname);
        return;
    }
    ARRAY_FOREACH (i, ci->access) {
        if (ci->access[i].nickgroup && ci->access[i].level == level)
            count++;
    }
    if (count)
        notice_lang(s_ChanServ, u, CHAN_XOP_COUNT, ci->name, listname, count);
    else
        notice_lang(s_ChanServ, u, CHAN_XOP_LIST_EMPTY, ci->name, listname);
}

/*************************************************************************/

static int xop_list(const User *u, int index, const ChannelInfo *ci,
                    int *sent_header)
{
    ChanAccess *access = &ci->access[index];
    NickGroupInfo *ngi;

    if (!(ngi = get_ngi_id(access->nickgroup)))
        return RET_INTERR;
    if (!*sent_header) {
        notice_lang(s_ChanServ, u, CHAN_XOP_LIST_HEADER,
                    XOP_LISTNAME(access->level), ci->name);
        *sent_header = 1;
    }
    notice(s_ChanServ, u->nick, "    %s", ngi_mainnick(ngi));
    put_nickgroupinfo(ngi);
    return RET_LISTED;
}

/*************************************************************************/
/*************************************************************************/

/* Callback to display help text for SOP and AOP. */

static int do_help(User *u, const char *param)
{
    if (stricmp(param, "SOP") == 0) {
        notice_help(s_ChanServ, u, CHAN_HELP_SOP);
        notice_help(s_ChanServ, u, CHAN_HELP_SOP_MID1);
        notice_help(s_ChanServ, u, CHAN_HELP_SOP_MID2);
        notice_help(s_ChanServ, u, CHAN_HELP_SOP_END);
        return 1;
    } else if (stricmp(param, "AOP") == 0) {
        notice_help(s_ChanServ, u, CHAN_HELP_AOP);
        notice_help(s_ChanServ, u, CHAN_HELP_AOP_MID);
        notice_help(s_ChanServ, u, CHAN_HELP_AOP_END);
        return 1;
    }
    return 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { NULL }
};

/*************************************************************************/

int init_module(void)
{
    module_chanserv = find_module("chanserv/main");
    if (!module_chanserv) {
        module_log("Main ChanServ module not loaded");
        return 0;
    }
    use_module(module_chanserv);

    if (!register_commands(module_chanserv, cmds)
        || ((protocol_features & PF_HALFOP)
            && !register_commands(module_chanserv, cmds_halfop))
    ) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    if (!add_callback(module_chanserv, "HELP", do_help)) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (module_chanserv) {
        remove_callback(module_chanserv, "HELP", do_help);
        if (protocol_features & PF_HALFOP)
            unregister_commands(module_chanserv, cmds_halfop);
        unregister_commands(module_chanserv, cmds);
        unuse_module(module_chanserv);
        module_chanserv = NULL;
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
