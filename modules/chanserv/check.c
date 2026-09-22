/* Routines to check validity of JOINs and mode changes.
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
#include "timeout.h"
#include "modules/nickserv/nickserv.h"
#include "modules/operserv/operserv.h"

#include "chanserv.h"
#include "cs-local.h"

/*************************************************************************/

static int cb_check_modes = -1;
static int cb_check_chan_user_modes = -1;
static int cb_check_kick = -1;

/*************************************************************************/
/*************************************************************************/

/* Check the current modes on a channel; if they conflict with a mode lock,
 * fix them. */

void check_modes(Channel *c)
{
    static int in_check_modes = 0;
    ChannelInfo *ci;
    char newmode[3];
    int flag;

    if (!c || c->bouncy_modes)
        return;

    if (!NoBouncyModes) {
        /* Check for mode bouncing */
        if (c->server_modecount >= 3 && c->chanserv_modecount >= 3) {
            wallops(NULL, "Warning: unable to set modes on channel %s.  "
                    "Are your servers configured correctly?", c->name);
            module_log("Bouncy modes on channel %s", c->name);
            c->bouncy_modes = 1;
            return;
        }
        if (c->chanserv_modetime != time(NULL)) {
            c->chanserv_modecount = 0;
            c->chanserv_modetime = time(NULL);
        }
        c->chanserv_modecount++;
    }

    ci = c->ci;
    if (!ci) {
        /* Services _always_ knows who should be +r. If a channel tries to be
         * +r and is not registered, send mode -r. This will compensate for
         * servers that are split when mode -r is initially sent and then try
         * to set +r when they rejoin. -TheShadow */
        if (c->mode & chanmode_reg) {
            char buf[BUFSIZE];
            snprintf(buf, sizeof(buf), "-%s",
                             mode_flags_to_string(chanmode_reg, MODE_CHANNEL));
            set_cmode(s_ChanServ, c, buf);
            /* Flush it out immediately.  Note that this won't cause
             * infinite recursion because we're clearing the mode that
             * got us here in the first place. */
            set_cmode(NULL, c);
        }
        return;
    }

    /* Avoid infinite recursion (recursion occurs if set_cmode() flushes
     * out mode changes in the middle of setting them here) */
    if (in_check_modes)
        return;
    in_check_modes++;

    newmode[2] = 0;
    /* Note that MODE_INVALID == 1<<31, so we can just stop there */
    for (flag = 1; flag != MODE_INVALID; flag <<= 1) {
        int add;
        if ((ci->mlock.on | chanmode_reg) & flag)
            add = 1;
        else if (ci->mlock.off & flag)
            add = 0;
        else
            continue;
        if (call_callback_4(cb_check_modes, c, ci, add, flag) > 0) {
            continue;
        } else if (flag == CMODE_k) {
            if (c->key && (!add || (add && c->key && ci->mlock.key
                                    && strcmp(c->key, ci->mlock.key) != 0))) {
                set_cmode(s_ChanServ, c, "-k", c->key);
                set_cmode(NULL, c);  /* flush it out */
            }
            if (add && !c->key)
                set_cmode(s_ChanServ, c, "+k", ci->mlock.key);
        } else if (flag == CMODE_l) {
            if (add && ci->mlock.limit != c->limit) {
                char limitbuf[16];
                snprintf(limitbuf, sizeof(limitbuf), "%d", ci->mlock.limit);
                set_cmode(s_ChanServ, c, "+l", limitbuf);
            } else if (!add && c->limit != 0) {
                set_cmode(s_ChanServ, c, "-l");
            }
        } else if (add ^ !!(c->mode & flag)) {
            newmode[0] = add ? '+' : '-';
            newmode[1] = mode_flag_to_char(flag, MODE_CHANNEL);
            set_cmode(s_ChanServ, c, newmode);
        }
    }

    in_check_modes--;
}

/*************************************************************************/

/* Check whether a user should be opped or voiced on a channel, and if so,
 * do it.  Updates the channel's last used time if the user is opped.
 * `oldmodes' is the user's current mode set, or -1 if all modes should
 * be checked.  `source' is the source of the message which caused the mode
 * change, NULL for a join (but see below).
 *
 * Note that this function may be called with an empty `source' (i.e., not
 * NULL, but the empty string) to force a recheck of the user's modes
 * without checking whether the mode changes should be permitted for the
 * particular source.
 */

/* Local helper routine */
static void local_set_cumodes(Channel *c, char plusminus, int32 modes,
                              struct c_userlist *cu);

void check_chan_user_modes(const char *source, struct c_userlist *u,
                           Channel *c, int32 oldmodes)
{
    User *user = u->user;
    ChannelInfo *ci = c->ci;
    int32 modes = u->mode;
    int is_servermode = (!source || strchr(source, '.') != NULL);
    int32 res;  /* result from check_access_cumode() */

    /* Don't change modes on unregistered or forbidden channels */
    if (!ci || (ci->flags & CF_VERBOTEN))
        return;

    /* Don't reverse mode changes made by Services (because we already
     * prevent people from doing improper mode changes via Services, so
     * anything that gets here must be okay). */
    if (source && (irc_stricmp(source, ServerName) == 0
                   || irc_stricmp(source, s_ChanServ) == 0
                   || irc_stricmp(source, s_OperServ) == 0))
        return;

    /* Also don't reverse mode changes by the user themselves, unless the
     * user is -o now (this could happen if we've sent out a -o already but
     * the user got in a +v or such before the -o reached their server), or
     * the user is going to be deopped soon but the -o is held up by
     * MergeChannelModes (the CUFLAG_DEOPPED flag).
     *
     * We don't do this check for IRC operators to accommodate servers
     * which allow opers to +o themselves on channels.  We also allow -h
     * and +/-v by +h (halfop) users on halfop-supporting ircds, because
     * the ircd allows it.
     */
    if (source && !is_oper(user) && irc_stricmp(source, user->nick) == 0) {
        if (!(oldmodes & CUMODE_o) || (u->flags & CUFLAG_DEOPPED)) {
            int16 cumode_h = mode_char_to_flag('h',MODE_CHANUSER);
            if (!((oldmodes & cumode_h)
                  && !((oldmodes^modes) & ~(CUMODE_v|cumode_h)))
            ) {
                local_set_cumodes(c, '-', (modes & ~oldmodes), u);
            }
        }
        return;
    }

    /* Check early for server auto-ops */
    if ((modes & CUMODE_o)
     && !(ci->flags & CF_LEAVEOPS)
     && is_servermode
    ) {
        if (!is_oper(user)  /* Here, too, don't subtract modes from opers */
         && (time(NULL)-start_time >= CSRestrictDelay
             || !check_access_if_idented(user, ci, CA_AUTOOP))
         && !check_access(user, ci, CA_AUTOOP)
        ) {
            notice_lang(s_ChanServ, user, CHAN_IS_REGISTERED, s_ChanServ);
            u->flags |= CUFLAG_DEOPPED;
            set_cmode(s_ChanServ, c, "-o", user->nick);
            modes &= ~CUMODE_o;
        } else if (check_access(user, ci, CA_AUTOOP)) {
            /* The user's an autoop user; update the last-used time here,
             * because it won't get updated below (they're already opped) */
            ci->last_used = time(NULL);
        }
    }

    /* Let the protocol module have a hack at it */
    if (call_callback_4(cb_check_chan_user_modes, source, user, c, modes) > 0)
        return;

    /* Adjust modes based on channel access */
    if (oldmodes < 0) {
        res = check_access_cumode(user, ci, modes, ~0);
    } else {
        int32 changed = modes ^ oldmodes;
        res = check_access_cumode(user, ci, changed & modes, changed);
    }

    /* Check for mode additions.  Only check if join or server mode change,
     * unless ENFORCE is set */
    /* Note: modes to add = changed modes & off new-modes = res & ~modes */
    if ((res & ~modes)
     && (oldmodes < 0 || is_servermode || (ci->flags & CF_ENFORCE))
    ) {
        local_set_cumodes(c, '+', res & ~modes, u);
        if ((res & ~modes) & CUMODE_o)
            ci->last_used = time(NULL);
    }

    /* Don't subtract modes from opers */
    if (is_oper(user))
        return;

    /* Check for mode subtractions */
    if (res & modes)
        local_set_cumodes(c, '-', res & modes, u);
}

/************************************/

/* Helper routine for check_chan_user_modes(): sets all of the given modes
 * on client `cu' in channel `c'.
 */

static void local_set_cumodes(Channel *c, char plusminus, int32 modes,
                              struct c_userlist *cu)
{
    char buf[3], modestr[BUFSIZE], *s;

    buf[0] = plusminus;
    buf[2] = 0;
    strbcpy(modestr, mode_flags_to_string(modes, MODE_CHANUSER));
    s = modestr;
    while (*s) {
        buf[1] = *s++;
        set_cmode(s_ChanServ, c, buf, cu->user->nick);
    }
    /* Set user's modes now, so check_chan_user_modes() can properly
     * determine whether subsequent modes should be set or not */
    if (plusminus == '+')
        cu->mode |= modes;
    else if (plusminus == '-')
        cu->mode &= ~modes;
}

/*************************************************************************/

/* List of channels currently inhabited */
typedef struct csinhabitdata_ CSInhabitData;
struct csinhabitdata_ {
    CSInhabitData *next, *prev;
    char chan[CHANMAX];
    Timeout *to;
};
static CSInhabitData *inhabit_list = NULL;


/* Tiny helper routine to get ChanServ out of a channel after it went in. */
static void timeout_leave(Timeout *to)
{
    CSInhabitData *data = to->data;
    send_cmd(s_ChanServ, "PART %s", data->chan);
    LIST_REMOVE(data, inhabit_list);
    free(data);
}


/* Check whether a user is permitted to be on a channel.  If so, return 0;
 * else, kickban the user with an appropriate message (could be either
 * AKICK or restricted access) and return 1.  When `on_join' is nonzero,
 * this routine will _not_ call do_kick(), assuming that the user is not
 * yet on the internal channel list.
 */

int check_kick(User *user, const char *chan, int on_join)
{
    Channel *c = get_channel(chan);
    ChannelInfo *ci = get_channelinfo(chan);
    int i;
    char *mask, *s;
    const char *reason;
    char reasonbuf[BUFSIZE];
    int stay;


    if (CSForbidShortChannel && strcmp(chan, "#") == 0) {
        mask = sstrdup("*!*@*");
        reason = getstring(user->ngi, CHAN_MAY_NOT_BE_USED);
        goto kick;
    }

    if (is_services_admin(user)) {
        put_channelinfo(ci);
        return 0;
    }

    i = call_callback_5(cb_check_kick, user, chan, ci, &mask, &reason);
    if (i == 2) {
        put_channelinfo(ci);
        return 0;
    } else if (i == 1) {
        goto kick;
    }

    /* Nothing else here affects IRC operators */
    if (is_oper(user)) {
        put_channelinfo(ci);
        return 0;
    }

    /* Check join against channel's modes--this is properly the domain of
     * the IRC server, but you never know... */
    if (c) {
        if (c->mode & chanmode_opersonly) {
            /* We know from above that they're not an oper */
            mask = create_mask(user, 1);
            reason = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
            goto kick;
        }
    }

    if (!ci) {
        if (CSRegisteredOnly) {
            mask = sstrdup("*!*@*");
            reason = getstring(user->ngi, CHAN_MAY_NOT_BE_USED);
            goto kick;
        } else {
            put_channelinfo(ci);
            return 0;
        }
    }

    if (ci->flags & (CF_VERBOTEN | CF_SUSPENDED)) {
        mask = sstrdup("*!*@*");
        reason = getstring(user->ngi, CHAN_MAY_NOT_BE_USED);
        goto kick;
    }

    if (ci->mlock.on & chanmode_opersonly) {
        /* We already know they're not an oper, so kick them off */
        mask = create_mask(user, 1);
        reason = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        goto kick;
    }

    if (!CSSkipModeRCheck && (ci->mlock.on & chanmode_regonly)
     && !user_identified(user)
    ) {
        /* User must have usermode_reg flags, i.e. be using a registered
         * nick and have identified, in order to join a chanmode_regonly
         * channel */
        mask = create_mask(user, 1);
        reason = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        goto kick;
    }

    ARRAY_FOREACH (i, ci->akick) {
        if (!ci->akick[i].mask) {
            log_debug(1, "%s autokick %d has NULL mask, deleting", ci->name,i);
            ARRAY_REMOVE(ci->akick, i);
            i--;
            continue;
        }
        if (match_usermask(ci->akick[i].mask, user)) {
            module_log_debug(2, "%s matched akick %s",
                             user->nick, ci->akick[i].mask);
            mask = sstrdup(ci->akick[i].mask);
            snprintf(reasonbuf, sizeof(reasonbuf), "AKICK by %s%s%s%s",
                     ci->akick[i].who,
                     ci->akick[i].reason ? " (" : "",
                     ci->akick[i].reason ? ci->akick[i].reason : "",
                     ci->akick[i].reason ? ")" : "");
            reason = reasonbuf;
            time(&ci->akick[i].lastused);
            goto kick;
        }
    }

    if ((time(NULL)-start_time >= CSRestrictDelay
         || check_access_if_idented(user, ci, CA_NOJOIN))
     && check_access(user, ci, CA_NOJOIN)
    ) {
        mask = create_mask(user, 1);
        reason = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        goto kick;
    }

    put_channelinfo(ci);
    return 0;

  kick:
    module_log_debug(1, "check_kick() kicking %s!%s@%s from %s",
                     user->nick, user->username, user->host, chan);
    /* When called on join, the user has not been added to our channel user
     * list yet, so we check whether the channel does not exist rather than
     * whether the channel has only one user in it.  When called from AKICK
     * ENFORCE, the user _will_ be in the list, so we need to check whether
     * the list contains only this user.  Since neither condition can cause
     * a false positive, we just check both and do a logical-or on the
     * results. */
    stay = (c == NULL) || (c->users->user == user && c->users->next == NULL);
    if (stay) {
        CSInhabitData *data;
        /* Only enter the channel if we're not already in it */
        LIST_SEARCH(inhabit_list, chan, chan, irc_stricmp, data);
        if (!data) {
            Timeout *to;
            send_cmd(s_ChanServ, "JOIN %s", chan);
            to = add_timeout(CSInhabit, timeout_leave, 0);
            to->data = data = smalloc(sizeof(*data));
            LIST_INSERT(data, inhabit_list);
            strbcpy(data->chan, chan);
            data->to = to;
        }
    }
    /* Make sure the mask has a ! in it */
    if (!(s = strchr(mask, '!')) || s > strchr(mask, '@')) {
        int len = strlen(mask);
        mask = srealloc(mask, len+3);
        memmove(mask+2, mask, len+1);
        mask[0] = '*';
        mask[1] = '!';
    }
    /* Clear any exceptions matching the user, to ensure that the ban takes
     * effect */
    if (c)
        clear_channel(c, CLEAR_EXCEPTS, user);
    /* Apparently invites can get around bans, so check for ban before adding*/
    if (!chan_has_ban(chan, mask)) {
        send_cmode_cmd(s_ChanServ, chan, "+b %s", mask);
        if (c) {
            char *av[3];
            av[0] = (char *)chan;
            av[1] = (char *)"+b";
            av[2] = mask;
            do_cmode(s_ChanServ, 3, av);
        }
    }
    free(mask);
    send_channel_cmd(s_ChanServ, "KICK %s %s :%s", chan, user->nick, reason);
    if (!on_join) {
        /* The user is already in the channel userlist, so get them out */
        char *av[3];
        av[0] = (char *)chan;
        av[1] = user->nick;
        av[2] = (char *)"check_kick";  /* dummy value */
        do_kick(s_ChanServ, 3, av);
    }
    put_channelinfo(ci);
    return 1;
}

/*************************************************************************/

/* See if the topic is locked on the given channel, and return 1 (and fix
 * the topic) if so, 0 if not. */

int check_topiclock(Channel *c, time_t topic_time)
{
    ChannelInfo *ci = c->ci;

    if (!ci || !(ci->flags & CF_TOPICLOCK))
        return 0;
    c->topic_time = topic_time;  /* because set_topic() may need it */
    set_topic(s_ChanServ, c, ci->last_topic,
              *ci->last_topic_setter ? ci->last_topic_setter : s_ChanServ,
              ci->last_topic_time);
    return 1;
}

/*************************************************************************/
/*************************************************************************/

int init_check(void)
{
    cb_check_modes           = register_callback("check_modes");
    cb_check_chan_user_modes = register_callback("check_chan_user_modes");
    cb_check_kick            = register_callback("check_kick");
    if (cb_check_modes < 0 || cb_check_chan_user_modes < 0
     || cb_check_kick < 0
    ) {
        module_log("check: Unable to register callbacks");
        exit_check();
        return 0;
    }
    return 1;
}

/*************************************************************************/

void exit_check()
{
    CSInhabitData *inhabit, *tmp;

    LIST_FOREACH_SAFE (inhabit, inhabit_list, tmp) {
        del_timeout(inhabit->to);
        LIST_REMOVE(inhabit, inhabit_list);
        free(inhabit);
    }
    unregister_callback(cb_check_kick);
    unregister_callback(cb_check_chan_user_modes);
    unregister_callback(cb_check_modes);
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
