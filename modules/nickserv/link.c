/* Nickname linking module.
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
#include "modules/operserv/operserv.h"
#include "modules/chanserv/chanserv.h"

#include "nickserv.h"
#include "ns-local.h"

/*************************************************************************/

static Module *module_nickserv;

static int32 NSLinkMax;

/*************************************************************************/

static void do_link(User *u);
static void do_unlink(User *u);
static void do_listlinks(User *u);

static Command cmds[] = {
    { "LINK",     do_link,     NULL,  NICK_HELP_LINK,         -1,-1 },
    { "UNLINK",   do_unlink,   NULL,  -1,
                NICK_HELP_UNLINK, NICK_OPER_HELP_UNLINK, },
    { "LISTLINKS",do_listlinks,NULL,  -1,
                NICK_HELP_LISTLINKS, NICK_OPER_HELP_LISTLINKS },
    { "SET MAINNICK", NULL,    NULL,  NICK_HELP_SET_MAINNICK, -1,-1 },
    { NULL }
};

/*************************************************************************/
/*************************** Command functions ***************************/
/*************************************************************************/

static void do_link(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni = u->ni, *ni2;
    NickGroupInfo *ngi = u->ngi;
    int n;

    if (readonly && !is_services_admin(u)) {
        notice_lang(s_NickServ, u, NICK_LINK_DISABLED);
    } else if (!nick) {
        syntax_error(s_NickServ, u, "LINK", NICK_LINK_SYNTAX);
    } else if (strlen(nick) > protocol_nickmax) {
        notice_lang(s_NickServ, u, NICK_TOO_LONG, protocol_nickmax);
    } else if (!valid_nick(nick)) {
        notice_lang(s_NickServ, u, NICK_INVALID, nick);
    } else if (!reglink_check(u, nick, NULL, NULL)) {
        notice_lang(s_NickServ, u, NICK_CANNOT_BE_LINKED, nick);
        return;
    } else if (!ni || !ngi || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (!user_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (irc_stricmp(u->nick, nick) == 0) {
        notice_lang(s_NickServ, u, NICK_LINK_SAME);
    } else if ((ni2 = get_nickinfo(nick)) != NULL) {
        int i;
        ARRAY_SEARCH_PLAIN(ngi->nicks, nick, irc_stricmp, i);
        if (i < ngi->nicks_count)
            notice_lang(s_NickServ, u, NICK_LINK_ALREADY_LINKED, nick);
        else if (ni2->status & NS_VERBOTEN)
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, nick);
        else
            notice_lang(s_NickServ, u, NICK_X_ALREADY_REGISTERED, nick);
        put_nickinfo(ni2);
    } else if (get_user(nick)) {
        notice_lang(s_NickServ, u, NICK_LINK_IN_USE, nick);
    } else if (ngi->nicks_count >= NSLinkMax) {
        notice_lang(s_NickServ, u, NICK_LINK_TOO_MANY, NSLinkMax);
    } else if (NSRegEmailMax && ngi->email && !is_services_admin(u)
            && (n=abs(count_nicks_with_email(ngi->email))) >= NSRegEmailMax) {
        notice_lang(s_NickServ, u, NICK_LINK_TOO_MANY_NICKS, n,
                    NSRegEmailMax);
    } else {
        ni2 = makenick(nick, NULL);
        if (ni->last_usermask)
            ni2->last_usermask = sstrdup(ni->last_usermask);
        if (ni->last_realmask)
            ni2->last_realmask = sstrdup(ni->last_realmask);
        if (ni->last_realname)
            ni2->last_realname = sstrdup(ni->last_realname);
        if (ni->last_quit)
            ni2->last_quit = sstrdup(ni->last_quit);
        ni2->time_registered = ni2->last_seen = time(NULL);
        ni2->nickgroup = ni->nickgroup;
        put_nickinfo(ni2);
        ARRAY_EXTEND(ngi->nicks);
        strbcpy(ngi->nicks[ngi->nicks_count-1], nick);
        module_log("%s!%s@%s linked nick %s to %s",
                   u->nick, u->username, u->host, nick, u->nick);
        notice_lang(s_NickServ, u, NICK_LINKED, nick);
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);
    }
} /* do_link() */

/*************************************************************************/

static void do_unlink(User *u)
{
    NickInfo *ni = u->ni, *ni2;
    NickGroupInfo *ngi = u->ngi, *ngi2 = NULL;
    char *nick = strtok(NULL, " ");
    char *extra = strtok(NULL, " ");
    int is_servadmin = is_services_admin(u);
    int force = (extra != NULL && stricmp(extra,"FORCE") == 0);

    if (readonly && !is_servadmin) {
        notice_lang(s_NickServ, u, NICK_LINK_DISABLED);
    } else if (!nick || (extra && (!is_oper(u) || !force))) {
        syntax_error(s_NickServ, u, "UNLINK",
                     is_oper(u) ? NICK_UNLINK_OPER_SYNTAX
                                : NICK_UNLINK_SYNTAX);
    } else if (force && !is_servadmin) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
    } else if (!ni || !ngi || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (!user_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (irc_stricmp(nick, u->nick) == 0) {
        notice_lang(s_NickServ, u, NICK_UNLINK_SAME);
    } else if (!(ni2 = get_nickinfo(nick)) || !ni2->nickgroup
               || !(ngi2 = get_ngi(ni2)) || ngi2->nicks_count == 1) {
        notice_lang(s_NickServ, u, force ? NICK_UNLINK_NOT_LINKED
                                         : NICK_UNLINK_NOT_LINKED_YOURS, nick);
    } else if (!force && ni2->nickgroup != ni->nickgroup) {
        notice_lang(s_NickServ, u, NICK_UNLINK_NOT_LINKED_YOURS, nick);
    } else {
        int msg;
        char *param1;
        if (ni2->nickgroup != ni->nickgroup) {
            delnick(ni2);
            msg = NICK_X_UNLINKED;
            param1 = ngi_mainnick(ngi2);
        } else {
            delnick(ni2);
            msg = NICK_UNLINKED;
            param1 = ngi_mainnick(ngi);  /* Not used, but for completeness */
        }
        if (force && WallAdminPrivs) {
            wallops(s_NickServ, "\2%s\2 used UNLINK FORCE on \2%s\2",
                    u->nick, nick);
        }
        notice_lang(s_NickServ, u, msg, nick, param1);
        module_log("%s!%s@%s unlinked nick %s from %s", u->nick,
                   u->username, u->host, nick, param1);
        if (readonly)
            notice_lang(s_NickServ, u, READ_ONLY_MODE);
    }
    put_nickgroupinfo(ngi2);
}

/*************************************************************************/

static void do_listlinks(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi;
    int i;

    if (nick) {
        if (!is_services_oper(u)) {
            syntax_error(s_NickServ, u, "LISTLINKS", NICK_LISTLINKS_SYNTAX);
            return;
        } else if (!(ni = get_nickinfo(nick))) {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, nick);
            return;
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, ni->nick);
            put_nickinfo(ni);
            return;
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(s_NickServ, u, INTERNAL_ERROR);
            put_nickinfo(ni);
            return;
        }
    } else {
        if (!(ni = u->ni) || !(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
            notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
            return;
        } else if (!user_identified(u)) {
            notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
            return;
        }
        ni->usecount++;
        ngi->usecount++;
    }

    notice_lang(s_NickServ, u, NICK_LISTLINKS_HEADER, ni->nick);
    ARRAY_FOREACH (i, ngi->nicks) {
        notice(s_NickServ, u->nick, "    %c%s",
               i==ngi->mainnick ? '*' : ' ', ngi->nicks[i]);
    }
    notice_lang(s_NickServ, u, NICK_LISTLINKS_FOOTER, ngi->nicks_count);
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

/* SET MAINNICK handler */

static int do_set_mainnick(User *u, NickInfo *ni, NickGroupInfo *ngi,
                           const char *cmd, const char *param)
{
    int i;

    if (stricmp(cmd, "MAINNICK") != 0)
        return 0;
    ARRAY_SEARCH_PLAIN(ngi->nicks, param, irc_stricmp, i);
    if (i >= ngi->nicks_count) {
        notice_lang(s_NickServ, u, NICK_SET_MAINNICK_NOT_FOUND, param);
    } else {
        module_log("%s!%s@%s set main nick of %s (group %u) to %s",
                   u->nick, u->username, u->host, ngi_mainnick(ngi),
                   ngi->id, ngi->nicks[i]);
        ngi->mainnick = i;
        notice_lang(s_NickServ, u, NICK_SET_MAINNICK_CHANGED, param);
    }
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "NSLinkMax",        { { CD_POSINT, CF_DIRREQ, &NSLinkMax } } },
    { NULL }
};

static int old_NICK_DROPPED   = -1;
static int old_NICK_X_DROPPED = -1;

/*************************************************************************/

int init_module(void)
{
    if (NSLinkMax > MAX_NICKCOUNT) {
        module_log("NSLinkMax upper-bounded at MAX_NICKCOUNT (%d)",
                   MAX_NICKCOUNT);
        NSLinkMax = MAX_NICKCOUNT;
    }

    module_nickserv = find_module("nickserv/main");
    if (!module_nickserv) {
        module_log("Main NickServ module not loaded");
        return 0;
    }
    use_module(module_nickserv);

    if (!register_commands(module_nickserv, cmds)) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    if (!add_callback(module_nickserv, "SET", do_set_mainnick)) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    old_NICK_DROPPED = mapstring(NICK_DROPPED, NICK_DROPPED_LINKS);
    old_NICK_X_DROPPED = mapstring(NICK_X_DROPPED, NICK_X_DROPPED_LINKS);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (old_NICK_DROPPED >= 0) {
        mapstring(NICK_DROPPED, old_NICK_DROPPED);
        old_NICK_DROPPED = -1;
    }
    if (old_NICK_X_DROPPED >= 0) {
        mapstring(NICK_X_DROPPED, old_NICK_X_DROPPED);
        old_NICK_X_DROPPED = -1;
    }

    if (module_nickserv) {
        remove_callback(module_nickserv, "SET", do_set_mainnick);
        unregister_commands(module_nickserv, cmds);
        unuse_module(module_nickserv);
        module_nickserv = NULL;
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
