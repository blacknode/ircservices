/* SET command handling.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "language.h"
#include "encrypt.h"
#include "modules/nickserv/nickserv.h"
#include "modules/operserv/operserv.h"

#include "chanserv.h"
#include "cs-local.h"

/*************************************************************************/

static int cb_set = -1;
static int cb_set_mlock = -1;
static int cb_unset = -1;

/*************************************************************************/

static void do_set_founder(User *u, ChannelInfo *ci, char *param);
static void do_set_successor(User *u, ChannelInfo *ci, char *param);
static void do_set_password(User *u, ChannelInfo *ci, char *param);
static void do_set_desc(User *u, ChannelInfo *ci, char *param);
static void do_set_url(User *u, ChannelInfo *ci, char *param);
static void do_set_email(User *u, ChannelInfo *ci, char *param);
static void do_set_entrymsg(User *u, ChannelInfo *ci, char *param);
static void do_set_mlock(User *u, ChannelInfo *ci, char *param);
static void do_set_hide(User *u, ChannelInfo *ci, char *param, char *extra);
static void do_set_boolean(User *u, ChannelInfo *ci, ChanOpt *co, char *param);

/*************************************************************************/
/*************************************************************************/

/* Main SET routine.  Calls other routines as follows:
 *      do_set_command(User *command_sender, ChannelInfo *ci, char *param);
 * The parameter passed is the first space-delimited parameter after the
 * option name, except in the case of DESC, TOPIC, and ENTRYMSG, in which
 * it is the entire remainder of the line.  Additional parameters beyond
 * the first passed in the function call can be retrieved using
 * strtok(NULL, toks).
 *
 * do_set_boolean, the default handler, is an exception to this in that it
 * also takes the ChanOpt structure for the selected option as a parameter.
 */

void do_set(User *u)
{
    char *chan = strtok(NULL, " ");
    char *cmd  = strtok(NULL, " ");
    char *param = NULL, *extra = NULL;
    ChannelInfo *ci = NULL;
    int is_servadmin = is_services_admin(u);
    int used_privs = 0;

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_SET_DISABLED);
        return;
    }

    if (chan && cmd) {
        if (stricmp(cmd, "DESC") == 0 || stricmp(cmd, "TOPIC") == 0
         || stricmp(cmd, "ENTRYMSG") == 0)
            param = strtok_remaining();
        else
            param = strtok(NULL, " ");
        if (stricmp(cmd, "HIDE") == 0)
            extra = strtok(NULL, " ");
    } else {
        param = NULL;
    }

    if (!param || (stricmp(cmd,"MLOCK") != 0 && strtok_remaining())) {
        syntax_error(s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access_cmd(u, ci, "SET", cmd)
               && (used_privs = 1, !is_servadmin)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (call_callback_4(cb_set, u, ci, cmd, param) > 0) {
        return;
    } else if (stricmp(cmd, "FOUNDER") == 0) {
        used_privs = 0;
        if (!is_founder(u, ci) && (used_privs = 1, !is_servadmin)) {
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_REQUIRED,s_ChanServ,chan);
        } else {
            if (WallAdminPrivs && used_privs) {
                wallops(s_ChanServ, "\2%s\2 used SET FOUNDER as Services"
                        " admin on \2%s\2", u->nick, ci->name);
            }
            do_set_founder(u, ci, param);
        }
    } else if (stricmp(cmd, "SUCCESSOR") == 0) {
        used_privs = 0;
        if (!is_founder(u, ci) && (used_privs = 1, !is_servadmin)) {
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_REQUIRED,s_ChanServ,chan);
        } else {
            if (WallAdminPrivs && used_privs) {
                wallops(s_ChanServ, "\2%s\2 used SET SUCCESSOR as Services"
                        " admin on \2%s\2", u->nick, ci->name);
            }
            do_set_successor(u, ci, param);
        }
    } else if (stricmp(cmd, "PASSWORD") == 0) {
        if (!is_servadmin && !is_founder(u, ci)) {
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_REQUIRED,s_ChanServ,chan);
        } else {
            if (WallAdminPrivs && used_privs) {
                wallops(s_ChanServ, "\2%s\2 used SET PASSWORD as Services"
                        " admin on \2%s\2", u->nick, ci->name);
            }
            do_set_password(u, ci, param);
        }
    } else if (stricmp(cmd, "DESC") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used SET DESC as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_desc(u, ci, param);
    } else if (stricmp(cmd, "URL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used SET URL as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_url(u, ci, param);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used SET EMAIL as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_email(u, ci, param);
    } else if (stricmp(cmd, "ENTRYMSG") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used SET ENTRYMSG as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_entrymsg(u, ci, param);
    } else if (stricmp(cmd, "MLOCK") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used SET MLOCK as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_mlock(u, ci, param);
    } else if (stricmp(cmd, "HIDE") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used SET HIDE as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_hide(u, ci, param, extra);
    } else {
        ChanOpt *co = chanopt_from_name(cmd);
        if (co && co->flag == CF_NOEXPIRE && (used_privs = 1, !is_servadmin))
            co = NULL;
        if (co) {
            if (WallAdminPrivs && used_privs) {
                wallops(s_ChanServ, "\2%s\2 used SET %s as Services admin"
                        " on \2%s\2", u->nick, co->name, ci->name);
            }
            do_set_boolean(u, ci, co, param);
        } else {
            notice_lang(s_ChanServ, u, CHAN_SET_UNKNOWN_OPTION, strupper(cmd));
            notice_lang(s_ChanServ, u, MORE_INFO, s_ChanServ, "SET");
        }
    }
    put_channelinfo(ci);
}

/*************************************************************************/

/* Handler for UNSET. */

void do_unset(User *u)
{
    char *chan = strtok(NULL, " ");
    char *cmd  = strtok(NULL, " ");
    ChannelInfo *ci = NULL;
    int is_servadmin = is_services_admin(u);
    int used_privs = 0;

    if (readonly) {
        notice_lang(s_ChanServ, u, CHAN_SET_DISABLED);
        return;
    }

    if (!chan || !cmd) {
        syntax_error(s_ChanServ, u, "UNSET", CHAN_UNSET_SYNTAX);
    } else if (!(ci = get_channelinfo(chan))) {
        notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
    } else if (ci->flags & CF_VERBOTEN) {
        notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
    } else if (!check_access_cmd(u, ci, "SET", cmd)
               && (used_privs = 1, !is_servadmin)) {
        notice_lang(s_ChanServ, u, ACCESS_DENIED);
    } else if (call_callback_3(cb_unset, u, ci, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "SUCCESSOR") == 0) {
        if (!is_servadmin && !is_founder(u, ci)) {
            notice_lang(s_ChanServ, u, CHAN_IDENTIFY_REQUIRED,s_ChanServ,chan);
        } else {
            if (WallAdminPrivs && !is_founder(u, ci)) {
                wallops(s_ChanServ, "\2%s\2 used UNSET SUCCESSOR as"
                        " Services admin on \2%s\2", u->nick, ci->name);
            }
            do_set_successor(u, ci, NULL);
        }
    } else if (stricmp(cmd, "URL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used UNSET URL as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_url(u, ci, NULL);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used UNSET EMAIL as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_email(u, ci, NULL);
    } else if (stricmp(cmd, "ENTRYMSG") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_ChanServ, "\2%s\2 used UNSET ENTRYMSG as Services admin"
                    " on \2%s\2", u->nick, ci->name);
        }
        do_set_entrymsg(u, ci, NULL);
    } else {
        syntax_error(s_ChanServ, u, "UNSET", CHAN_UNSET_SYNTAX);
    }
    put_channelinfo(ci);
}

/*************************************************************************/
/*************************************************************************/

static void do_set_founder(User *u, ChannelInfo *ci, char *param)
{
    NickInfo *ni = get_nickinfo(param);
    NickGroupInfo *ngi, *oldngi;

    if (!ni) {
        notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, param);
        return;
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, param);
        put_nickinfo(ni);
        return;
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_ChanServ, u, INTERNAL_ERROR);
        put_nickinfo(ni);
        return;
    }
    put_nickinfo(ni);
    if ((!is_services_admin(u) && check_channel_limit(ngi, NULL) >= 0)
     || ngi->channels_count >= MAX_CHANNELCOUNT
    ) {
        notice_lang(s_ChanServ, u, CHAN_SET_FOUNDER_TOO_MANY_CHANS, param);
        return;
    }
    uncount_chan(ci);
    oldngi = get_ngi_id(ci->founder);
    module_log("Changing founder of %s from %s to %s by %s!%s@%s", ci->name,
               oldngi ? ngi_mainnick(oldngi) : "<unknown>", param, u->nick,
               u->username, u->host);
    put_nickgroupinfo(oldngi);
    ci->founder = ngi->id;
    put_nickgroupinfo(ngi);
    count_chan(ci);
    if (ci->successor == ci->founder) {
        module_log("Successor for %s is same as new founder, clearing",
                   ci->name);
        ci->successor = 0;
    }
    notice_lang(s_ChanServ, u, CHAN_FOUNDER_CHANGED, ci->name, param);
}

/*************************************************************************/

static void do_set_successor(User *u, ChannelInfo *ci, char *param)
{
    if (param) {
        NickInfo *ni;
        NickGroupInfo *ngi;

        ni = get_nickinfo(param);
        if (!ni) {
            notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, param);
            return;
        } else if (ni->status & NS_VERBOTEN) {
            notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, param);
            put_nickinfo(ni);
            return;
        } else if (!(ngi = get_ngi(ni))) {
            notice_lang(s_ChanServ, u, INTERNAL_ERROR);
            put_nickinfo(ni);
            return;
        }
        put_nickinfo(ni);
        if (ngi->id == ci->founder) {
            notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_IS_FOUNDER);
            put_nickgroupinfo(ngi);
            return;
        }
        if (ci->successor) {
            NickGroupInfo *oldngi = get_ngi_id(ci->successor);
            module_log("Changing successor of %s from %s to %s by %s!%s@%s",
                       ci->name,
                       oldngi ? ngi_mainnick(oldngi) : "<unknown>", param,
                       u->nick, u->username, u->host);
            put_nickgroupinfo(oldngi);
        } else {
            module_log("Setting successor of %s to %s by %s!%s@%s",
                       ci->name, param, u->nick, u->username, u->host);
        }
        ci->successor = ngi->id;
        put_nickgroupinfo(ngi);
        notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_CHANGED, ci->name, param);
    } else {
        module_log("Clearing successor of %s by %s!%s@%s",
                   ci->name, u->nick, u->username, u->host);
        ci->successor = 0;
        notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_UNSET, ci->name);
    }
}

/*************************************************************************/

static void do_set_password(User *u, ChannelInfo *ci, char *param)
{
    Password passbuf;
    User *u2;

    if (!(NoAdminPasswordCheck && is_services_admin(u))
        && (stricmp(param, ci->name) == 0
         || stricmp(param, ci->name+1) == 0
         || stricmp(param, u->nick) == 0
         || (StrictPasswords && strlen(param) < 5))
    ) {
        notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);
        return;
    }

    init_password(&passbuf);
    if (encrypt_password(param, strlen(param), &passbuf) != 0) {
        clear_password(&passbuf);
        memset(param, 0, strlen(param));
        module_log("Failed to encrypt password for %s (set)", ci->name);
        notice_lang(s_ChanServ, u, CHAN_SET_PASSWORD_FAILED);
        return;
    }
    copy_password(&ci->founderpass, &passbuf);
    clear_password(&passbuf);
    if (CSShowPassword)
        notice_lang(s_ChanServ, u, CHAN_PASSWORD_CHANGED_TO, ci->name, param);
    else
        notice_lang(s_ChanServ, u, CHAN_PASSWORD_CHANGED, ci->name);
    memset(param, 0, strlen(param));
    if (!is_founder(u, ci)) {
        module_log("%s!%s@%s set password as Services admin for %s",
                   u->nick, u->username, u->host, ci->name);
    }
    /* Clear founder privileges from all other users who might have
     * identified earlier. */
    for (u2 = first_user(); u2; u2 = next_user()) {
        if (u2 != u) {
            struct u_chaninfolist *c, *c2;
            LIST_FOREACH_SAFE (c, u2->id_chans, c2) {
                if (irc_stricmp(c->chan, ci->name) == 0) {
                    LIST_REMOVE(c, u2->id_chans);
                    free(c);
                }
            }
        }
    }
}

/*************************************************************************/

static void do_set_desc(User *u, ChannelInfo *ci, char *param)
{
    free(ci->desc);
    ci->desc = sstrdup(param);
    notice_lang(s_ChanServ, u, CHAN_DESC_CHANGED, ci->name, param);
}

/*************************************************************************/

static void do_set_url(User *u, ChannelInfo *ci, char *param)
{
    if (param && !valid_url(param)) {
        notice_lang(s_ChanServ, u, BAD_URL);
        return;
    }

    free(ci->url);
    if (param) {
        ci->url = sstrdup(param);
        notice_lang(s_ChanServ, u, CHAN_URL_CHANGED, ci->name, param);
    } else {
        ci->url = NULL;
        notice_lang(s_ChanServ, u, CHAN_URL_UNSET, ci->name);
    }
}

/*************************************************************************/

static void do_set_email(User *u, ChannelInfo *ci, char *param)
{
    if (param && !valid_email(param)) {
        notice_lang(s_ChanServ, u, BAD_EMAIL);
        return;
    }
    if (param && rejected_email(param)) {
        notice_lang(s_NickServ, u, REJECTED_EMAIL);
        return;
    }

    free(ci->email);
    if (param) {
        ci->email = sstrdup(param);
        notice_lang(s_ChanServ, u, CHAN_EMAIL_CHANGED, ci->name, param);
    } else {
        ci->email = NULL;
        notice_lang(s_ChanServ, u, CHAN_EMAIL_UNSET, ci->name);
    }
}

/*************************************************************************/

static void do_set_entrymsg(User *u, ChannelInfo *ci, char *param)
{
    free(ci->entry_message);
    if (param) {
        ci->entry_message = sstrdup(param);
        notice_lang(s_ChanServ, u, CHAN_ENTRY_MSG_CHANGED, ci->name);
    } else {
        ci->entry_message = NULL;
        notice_lang(s_ChanServ, u, CHAN_ENTRY_MSG_UNSET, ci->name);
    }
}

/*************************************************************************/

static void do_set_mlock(User *u, ChannelInfo *ci, char *param)
{
    char *s, modebuf[40], *end, c;
    int ac = 0;
    char *av[MAX_MLOCK_PARAMS];
    char **avptr = &av[0];
    int add = -1;       /* 1 if adding, 0 if deleting, -1 if neither */
    int32 flag;
    int params;
    ModeLock oldlock;

    oldlock = ci->mlock;
    memset(&ci->mlock, 0, sizeof(ci->mlock));

    while (ac < MAX_MLOCK_PARAMS && (s = strtok(NULL, " ")) != NULL)
        av[ac++] = s;

    while (*param) {
        if (*param == '+') {
            add = 1;
            param++;
            continue;
        } else if (*param == '-') {
            add = 0;
            param++;
            continue;
        } else if (add < 0) {
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_NEED_PLUS_MINUS);
            goto fail;
        }
        c = *param++;
        flag = mode_char_to_flag(c, MODE_CHANNEL);
        params = mode_char_to_params(c, MODE_CHANNEL);
        if (!flag) {
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_UNKNOWN_CHAR, c);
            continue;
        } else if (flag == MODE_INVALID) {
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_CANNOT_LOCK, c);
            continue;
        }
        /* "Off" locks never take parameters (they prevent the mode from
         * ever being set in the first place) */
        params = add ? (params>>8) & 0xFF : 0;
        if (params > ac) {
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_NEED_PARAM, c);
            goto fail;
        }
        if (flag & chanmode_reg)
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_MODE_REG_BAD, c);
        else if (add)
            ci->mlock.on |= flag, ci->mlock.off &= ~flag;
        else
            ci->mlock.off |= flag, ci->mlock.on &= ~flag;
        switch (c) {
          case 'k':
            if (add) {
                free(ci->mlock.key);
                ci->mlock.key = sstrdup(avptr[0]);
            } else {
                free(ci->mlock.key);
                ci->mlock.key = NULL;
            }
            break;
          case 'l':
            if (add) {
                ci->mlock.limit = atol(avptr[0]);
                if (ci->mlock.limit <= 0) {
                    notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_NEED_POSITIVE,
                                'l');
                    goto fail;
                }
            } else {
                ci->mlock.limit = 0;
            }
            break;
        } /* switch */
        if (call_callback_5(cb_set_mlock, u, ci, c, add, avptr) > 0)
            goto fail;
        ac -= params;
        avptr += params;
    } /* while (*param) */

    /* Make sure there are no problems. */
    if (call_callback_5(cb_set_mlock, u, ci, 0, 0, NULL) > 0)
        goto fail;

    /* Tell the user about the new mode lock. */
    end = modebuf;
    *end = 0;
    if (ci->mlock.on || ci->mlock.key || ci->mlock.limit)
        end += snprintf(end, sizeof(modebuf)-(end-modebuf), "+%s",
                        mode_flags_to_string(ci->mlock.on, MODE_CHANNEL));
    if (ci->mlock.off)
        end += snprintf(end, sizeof(modebuf)-(end-modebuf), "-%s",
                        mode_flags_to_string(ci->mlock.off, MODE_CHANNEL));
    if (*modebuf) {
        notice_lang(s_ChanServ, u, CHAN_MLOCK_CHANGED, ci->name, modebuf);
    } else {
        notice_lang(s_ChanServ, u, CHAN_MLOCK_REMOVED, ci->name);
    }

    /* Clean up, implement the new lock and return. */
    free(oldlock.key);
    free(oldlock.link);
    free(oldlock.flood);
    check_modes(ci->c);
    return;

  fail:
    /* Failure; restore the old mode lock. */
    free(ci->mlock.key);
    free(ci->mlock.link);
    free(ci->mlock.flood);
    ci->mlock = oldlock;
}

/*************************************************************************/

#define HIDE(x) do {                    \
    flag = CF_HIDE_##x;         \
    onmsg = CHAN_SET_HIDE_##x##_ON;     \
    offmsg = CHAN_SET_HIDE_##x##_OFF;   \
} while (0)

static void do_set_hide(User *u, ChannelInfo *ci, char *param, char *extra)
{
    int32 flag;
    int onmsg, offmsg;

    if (!extra) {
        syntax_error(s_ChanServ, u, "SET HIDE", CHAN_SET_HIDE_SYNTAX);
        return;
    }
    if (stricmp(param, "EMAIL") == 0) {
        HIDE(EMAIL);
    } else if (stricmp(param, "TOPIC") == 0) {
        HIDE(TOPIC);
    } else if (stricmp(param, "MLOCK") == 0) {
        HIDE(MLOCK);
    } else {
        syntax_error(s_ChanServ, u, "SET HIDE", CHAN_SET_HIDE_SYNTAX);
        return;
    }
    if (stricmp(extra, "ON") == 0) {
        ci->flags |= flag;
        notice_lang(s_ChanServ, u, onmsg, ci->name, s_ChanServ);
    } else if (stricmp(extra, "OFF") == 0) {
        ci->flags &= ~flag;
        notice_lang(s_ChanServ, u, offmsg, ci->name, s_ChanServ);
    } else {
        syntax_error(s_ChanServ, u, "SET HIDE", CHAN_SET_HIDE_SYNTAX);
    }
}

#undef HIDE

/*************************************************************************/

static void do_set_boolean(User *u, ChannelInfo *ci, ChanOpt *co, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ci->flags |= co->flag;
        notice_lang(s_ChanServ, u, co->onstr, ci->name);
    } else if (stricmp(param, "OFF") == 0) {
        ci->flags &= ~co->flag;
        notice_lang(s_ChanServ, u, co->offstr, ci->name);
    } else {
        char buf[BUFSIZE];
        snprintf(buf, sizeof(buf), "SET %s", co->name);
        syntax_error(s_ChanServ, u, buf, co->syntaxstr);
        return;
    }
}

/*************************************************************************/
/*************************************************************************/

int init_set(void)
{
    cb_set = register_callback("SET");
    cb_set_mlock = register_callback("SET MLOCK");
    cb_unset = register_callback("UNSET");
    if (cb_set < 0 || cb_set_mlock < 0 || cb_unset < 0) {
        module_log("set: Unable to register callbacks");
        exit_set();
        return 0;
    }
    return 1;
}

/*************************************************************************/

void exit_set()
{
    unregister_callback(cb_unset);
    unregister_callback(cb_set_mlock);
    unregister_callback(cb_set);
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
