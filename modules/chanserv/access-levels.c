/* Access list and level modification handling for ChanServ.
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
#include "access.h"
#include "cs-local.h"

/*************************************************************************/

static Module *module_chanserv;

/*************************************************************************/

/* Local functions. */

static void do_levels(User *u);
static void do_access(User *u);
static int access_list(const User *u, int index, const ChannelInfo *ci,
                       int *sent_header);

/*************************************************************************/

static Command cmds[] = {
    { "ACCESS",   do_access,   NULL,  -1,                       -1,-1 },
    { "LEVELS",   do_levels,   NULL,  -1,                       -1,-1 },
    { NULL }
};

/*************************************************************************/
/***************************** Help display ******************************/
/*************************************************************************/

/* Callback to display help text for ACCESS, ACCESS LEVELS, LEVELS, and
 * LEVELS DESC.
 */

static int do_help(User *u, const char *param)
{
    int i;
    const char *s;

    if (stricmp(param, "ACCESS") == 0) {
        notice_help(s_ChanServ, u, CHAN_HELP_ACCESS);
        if (find_module("chanserv/access-xop")) {
            if (protocol_features & PF_HALFOP) {
                notice_help(s_ChanServ, u, CHAN_HELP_ACCESS_XOP_HALFOP,
                            ACCLEV_SOP, ACCLEV_AOP, ACCLEV_HOP, ACCLEV_VOP,
                            ACCLEV_NOP);
            } else {
                notice_help(s_ChanServ, u, CHAN_HELP_ACCESS_XOP,
                            ACCLEV_SOP, ACCLEV_AOP, ACCLEV_VOP, ACCLEV_NOP);
            }
        }
        return 1;
    } else if (strnicmp(param, "ACCESS", 6) == 0
            && isspace(param[6])
            && stricmp(param+7+strspn(param+7," \t"), "LEVELS") == 0
    ) {
        notice_help(s_ChanServ, u, CHAN_HELP_ACCESS_LEVELS,
                    ACCLEV_SOP, ACCLEV_AOP);
        if (protocol_features & PF_HALFOP) {
            notice_help(s_ChanServ, u, CHAN_HELP_ACCESS_LEVELS_HALFOP,
                        ACCLEV_HOP);
        }
        notice_help(s_ChanServ, u, CHAN_HELP_ACCESS_LEVELS_END,
                    ACCLEV_VOP);
        return 1;
    } else if (strnicmp(param, "LEVELS", 6) == 0) {
        s = (param+6) + strspn(param+6," \t");
        if (!*s) {
            notice_help(s_ChanServ, u, CHAN_HELP_LEVELS);
            if (find_module("chanserv/access-xop")) {
                if (protocol_features & PF_HALFOP)
                    notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_XOP_HOP);
                else
                    notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_XOP);
            }
            notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_END);
            return 1;
        } else if (stricmp(s, "DESC") == 0) {
            int levelinfo_maxwidth = 0;
            notice_help(s_ChanServ, u, CHAN_HELP_LEVELS_DESC);
            for (i = 0; levelinfo[i].what >= 0; i++) {
                int len = strlen(levelinfo[i].name);
                if (len > levelinfo_maxwidth)
                    levelinfo_maxwidth = len;
            }
            for (i = 0; levelinfo[i].what >= 0; i++) {
                if (!*levelinfo[i].name)
                    continue;
                notice(s_ChanServ, u->nick, "    %-*s  %s",
                       levelinfo_maxwidth, levelinfo[i].name,
                       getstring(u->ngi, levelinfo[i].desc));
            }
            return 1;
        }
    }
    return 0;
}

/*************************************************************************/
/************************** The ACCESS command ***************************/
/*************************************************************************/

static void do_access_add(const User *u, int is_servadmin, ChannelInfo *ci,
                          const char *nick, const char *levelstr);
static void do_access_del(const User *u, int is_servadmin, ChannelInfo *ci,
                          const char *nick);
static void do_access_list(const User *u, const ChannelInfo *ci,
                           const char *startstr, const char *pattern);
static void do_access_listlevel(const User *u, const ChannelInfo *ci,
                                const char *startstr, const char *levels);
static void do_access_count(const User *u, const ChannelInfo *ci);

static void do_access(User *u)
{
    char *chan = strtok(NULL, " ");
    char *cmd  = strtok(NULL, " ");
    char *nick = strtok(NULL, " ");
    char *s    = strtok(NULL, " ");
    ChannelInfo *ci = NULL;
    int is_list;  /* True when command is a LIST-like command */
    int is_servadmin = is_services_admin(u);

    is_list = (cmd && (stricmp(cmd,"LIST")==0 || stricmp(cmd,"LISTLEVEL")==0
                    || stricmp(cmd,"COUNT")==0));

    if (!chan || !cmd) {
        syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!is_servadmin
               && !check_access_cmd(u, ci, "ACCESS", is_list ? "LIST" : cmd)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (stricmp(cmd, "ADD") == 0) {
        if (!nick || !s)
            syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_ADD_SYNTAX);
        else
            do_access_add(u, is_servadmin, ci, nick, s);
    } else if (stricmp(cmd, "DEL") == 0) {
        if (!nick || s)
            syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_DEL_SYNTAX);
        else
            do_access_del(u, is_servadmin, ci, nick);
    } else if (stricmp(cmd, "LIST") == 0) {
        int have_start = (nick && *nick=='+');
        do_access_list(u, ci, have_start ? nick : NULL,
                       have_start ? s : nick);
    } else if (stricmp(cmd, "LISTLEVEL") == 0) {
        const char *startstr, *levelstr;
        int have_start = (nick && *nick=='+');
        if (have_start) {
            startstr = nick;
            levelstr = s;
        } else {
            startstr = NULL;
            levelstr = nick;
        }
        if (!levelstr)
            syntax_error(s_ChanServ, u, "ACCESS",CHAN_ACCESS_LISTLEVEL_SYNTAX);
        else
            do_access_listlevel(u, ci, startstr, levelstr);
    } else if (stricmp(cmd, "COUNT") == 0) {
        if (nick || s)
            syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_COUNT_SYNTAX);
        else
            do_access_count(u, ci);
    } else { /* Unknown command */
        syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_SYNTAX);
    }
    put_channelinfo(ci);
}


static void do_access_add(const User *u, int is_servadmin, ChannelInfo *ci,
                          const char *nick, const char *levelstr)
{
    int level;

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
        return;
    }
    level = (int)atolsafe(levelstr, ACCLEV_MIN, ACCLEV_MAX);
    if (level == ACCLEV_INVALID) {
        if (errno == EINVAL) {
            syntax_error(s_ChanServ, u, "ADD", CHAN_ACCESS_ADD_SYNTAX);
        } else {
            notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_RANGE,
                        ACCLEV_MIN, ACCLEV_MAX);
        }
        return;
    } else if (level == 0) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_NONZERO);
        return;
    }
    switch (access_add(ci, nick, level,
                       is_servadmin ? ACCLEV_FOUNDER : get_access(u,ci))) {
      case RET_ADDED:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_ADDED, nick, ci->name, level);
        break;
      case RET_CHANGED:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_CHANGED,
                    nick, ci->name, level);
        break;
      case RET_UNCHANGED:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_UNCHANGED,
                    nick, ci->name, level);
        break;
      case RET_LISTFULL:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_REACHED_LIMIT, CSAccessMax);
        break;
      case RET_NOSUCHNICK:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_NICKS_ONLY);
        break;
      case RET_NICKFORBID:
        notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, nick);
        break;
      case RET_NOOP:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_NOOP, nick);
        break;
      case RET_PERMISSION:
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        break;
    }
}


static void do_access_del(const User *u, int is_servadmin, ChannelInfo *ci,
                          const char *nick)
{
    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_DISABLED);
        return;
    }
    switch (access_del(ci, nick,
                       is_servadmin ? ACCLEV_FOUNDER : get_access(u,ci))) {
      case RET_DELETED:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_DELETED, nick, ci->name);
        break;
      case RET_NOENTRY:
        notice_lang(s_ChanServ, u, CHAN_ACCESS_NOT_FOUND, nick, ci->name);
        break;
      case RET_NOSUCHNICK:
        notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, nick);
        break;
      case RET_NICKFORBID:
        notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, nick);
        break;
      case RET_PERMISSION:
        notice_lang(s_ChanServ, u, PERMISSION_DENIED);
        break;
      case RET_INTERR:
        notice_lang(s_ChanServ, u, INTERNAL_ERROR);
        break;
    }
}

static void do_access_list(const User *u, const ChannelInfo *ci,
                           const char *startstr, const char *pattern)
{
    NickGroupInfo *ngi;
    int count = 0, sent_header = 0, skip = 0, i;

    if (startstr) {
        skip = (int)atolsafe(startstr+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_ChanServ, u, "ACCESS", CHAN_ACCESS_LIST_SYNTAX);
            return;
        }
    }
    if (ci->access_count == 0) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, ci->name);
        return;
    }
    ARRAY_FOREACH (i, ci->access) {
        if (!ci->access[i].nickgroup)
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
            access_list(u, i, ci, &sent_header);
    }
    if (count) {
        int shown = count - skip;
        if (shown < 0)
            shown = 0;
        else if (count > ListMax)
            shown = ListMax;
        notice_lang(s_ChanServ, u, LIST_RESULTS, shown, count);
    } else {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name);
    }
}

static void do_access_listlevel(const User *u, const ChannelInfo *ci,
                                const char *startstr, const char *levels)
{
    int low, high, dir;
    int count = 0, sent_header = 0, skip = 0, i;
    long v, v2;
    char *s;

    if (startstr) {
        skip = (int)atolsafe(startstr+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_ChanServ, u, "ACCESS",CHAN_ACCESS_LISTLEVEL_SYNTAX);
            return;
        }
    }

    v = strtol(levels, &s, 10);
    if (s > levels && *s == '-')
        v2 = strtol(s+1, &s, 10);
    else
        v2 = v;
    if (s == levels || *s != 0) {
        syntax_error(s_ChanServ, u, "ACCESS",CHAN_ACCESS_LISTLEVEL_SYNTAX);
        return;
    } else if (v  < ACCLEV_MIN || v  > ACCLEV_MAX
            || v2 < ACCLEV_MIN || v2 > ACCLEV_MAX) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LEVEL_RANGE,
                    ACCLEV_MIN, ACCLEV_MAX);
        return;
    }
    if (v2 < v) {
        low = v2;
        high = v;
        dir = -1;
    } else {
        low = v;
        high = v2;
        dir = 1;
    }

    if (ci->access_count == 0) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_EMPTY, ci->name);
        return;
    }

    ARRAY_FOREACH (i, ci->access) {
        if (ci->access[i].nickgroup
         && ci->access[i].level >= low
         && ci->access[i].level <= high
        ) {
            count++;
            if (count > skip && count <= skip+ListMax)
                access_list(u, i, ci, &sent_header);
        }
    }
    if (count) {
        int shown = count - skip;
        if (shown < 0)
            shown = 0;
        else if (count > ListMax)
            shown = ListMax;
        notice_lang(s_ChanServ, u, LIST_RESULTS, shown, count);
    } else {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_NO_MATCH, ci->name);
    }
}

static void do_access_count(const User *u, const ChannelInfo *ci)
{
    int count = 0, i;

    ARRAY_FOREACH (i, ci->access) {
        if (ci->access[i].nickgroup)
            count++;
    }
    notice_lang(s_ChanServ, u, CHAN_ACCESS_COUNT, ci->name, count);
}

/*************************************************************************/

static int access_list(const User *u, int index, const ChannelInfo *ci,
                       int *sent_header)
{
    ChanAccess *access = &ci->access[index];
    NickGroupInfo *ngi;

    if (!access->nickgroup)
        return RET_NOENTRY;
    if (!(ngi = get_ngi_id(access->nickgroup)))
        return RET_INTERR;
    if (!*sent_header) {
        notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_HEADER, ci->name);
        *sent_header = 1;
    }
    notice_lang(s_ChanServ, u, CHAN_ACCESS_LIST_FORMAT,
                access->level, ngi_mainnick(ngi));
    put_nickgroupinfo(ngi);
    return RET_LISTED;
}

/*************************************************************************/
/************************** The LEVELS command ***************************/
/*************************************************************************/

static void do_levels(User *u)
{
    char *chan = strtok(NULL, " ");
    char *cmd  = strtok(NULL, " ");
    char *what = strtok(NULL, " ");
    char *s    = strtok(NULL, " ");
    ChannelInfo *ci = NULL;
    int16 level;
    int i;

    /* If SET, we want two extra parameters; if DIS[ABLE], we want only
     * one; else, we want none.
     */
    if (!chan || !cmd || ((stricmp(cmd,"SET")==0) ? !s :
                        (strnicmp(cmd,"DIS",3)==0) ? (!what || s) : !!what)) {
        syntax_error(s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!is_founder(u, ci) && !is_services_admin(u)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);

    } else if (stricmp(cmd, "SET") == 0) {
        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_LEVELS_READONLY);
            put_channelinfo(ci);
            return;
        }
        level = atoi(s);
        if (level < ACCLEV_MIN || level > ACCLEV_MAX) {
            notice_lang(s_ChanServ, u, CHAN_LEVELS_RANGE,
                        ACCLEV_MIN, ACCLEV_MAX);
            put_channelinfo(ci);
            return;
        }
        for (i = 0; levelinfo[i].what >= 0; i++) {
            if (stricmp(levelinfo[i].name, what) == 0) {
                ci->levels[levelinfo[i].what] = level;
                notice_lang(s_ChanServ, u, CHAN_LEVELS_CHANGED,
                            levelinfo[i].name, chan, level);
                put_channelinfo(ci);
                return;
            }
        }
        notice_lang(s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what, s_ChanServ);

    } else if (stricmp(cmd, "DIS") == 0 || stricmp(cmd, "DISABLE") == 0) {
        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_LEVELS_READONLY);
            put_channelinfo(ci);
            return;
        }
        for (i = 0; levelinfo[i].what >= 0; i++) {
            if (stricmp(levelinfo[i].name, what) == 0) {
                ci->levels[levelinfo[i].what] = ACCLEV_INVALID;
                notice_lang(s_ChanServ, u, CHAN_LEVELS_DISABLED,
                            levelinfo[i].name, chan);
                put_channelinfo(ci);
                return;
            }
        }
        notice_lang(s_ChanServ, u, CHAN_LEVELS_UNKNOWN, what, s_ChanServ);

    } else if (stricmp(cmd, "LIST") == 0) {
        int levelinfo_maxwidth = 0, i;
        notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_HEADER, chan);
        for (i = 0; levelinfo[i].what >= 0; i++) {
            int len = strlen(levelinfo[i].name);
            if (len > levelinfo_maxwidth)
                levelinfo_maxwidth = len;
        }
        for (i = 0; levelinfo[i].what >= 0; i++) {
            int lev;
            if (!*levelinfo[i].name)
                continue;
            lev = get_ci_level(ci, levelinfo[i].what);
            if (lev == ACCLEV_INVALID)
                notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_DISABLED,
                            levelinfo_maxwidth, levelinfo[i].name);
            else if (lev == ACCLEV_FOUNDER)
                notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_FOUNDER,
                            levelinfo_maxwidth, levelinfo[i].name);
            else
                notice_lang(s_ChanServ, u, CHAN_LEVELS_LIST_NORMAL,
                            levelinfo_maxwidth, levelinfo[i].name, lev);
        }

    } else if (stricmp(cmd, "RESET") == 0) {
        if (readonly) {
            notice_lang(s_ChanServ, u, CHAN_LEVELS_READONLY);
            put_channelinfo(ci);
            return;
        }
        reset_levels(ci);
        notice_lang(s_ChanServ, u, CHAN_LEVELS_RESET, chan);

    } else {
        syntax_error(s_ChanServ, u, "LEVELS", CHAN_LEVELS_SYNTAX);
    }

    put_channelinfo(ci);
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

    if (!register_commands(module_chanserv, cmds)) {
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
