/* ChanServ internal utility routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

/* If STANDALONE_CHANSERV is defined when compiling this file, the
 * following routines will be defined as `static' for use in other
 * modules/programs:
 *    new_channelinfo()
 *    free_channelinfo()
 *    reset_levels()
 * This file should be #include'd in the file making use of the routines.
 */

#ifdef STANDALONE_CHANSERV

# define STANDALONE_STATIC static
# undef EXPORT_FUNC
# define EXPORT_FUNC(x) /*nothing*/

#else

# define STANDALONE_STATIC /*nothing*/

# include "services.h"
# include "modules.h"
# include "language.h"
# include "modules/nickserv/nickserv.h"

# include "chanserv.h"
# include "cs-local.h"

#endif

/*************************************************************************/
/************************ Global utility routines ************************/
/*************************************************************************/

/* Allocate and initialize a new ChannelInfo structure. */

EXPORT_FUNC(new_channelinfo)
STANDALONE_STATIC ChannelInfo *new_channelinfo(void)
{
    ChannelInfo *ci = scalloc(sizeof(ChannelInfo), 1);
    reset_levels(ci);
    return ci;
}

/*************************************************************************/

/* Free a ChannelInfo structure and all associated data. */

EXPORT_FUNC(free_channelinfo)
STANDALONE_STATIC void free_channelinfo(ChannelInfo *ci)
{
    int i;

    if (ci) {
        clear_password(&ci->founderpass);
        free(ci->desc);
        free(ci->url);
        free(ci->email);
        free(ci->last_topic);
        free(ci->suspend_reason);
        free(ci->access);
        ARRAY_FOREACH (i, ci->akick) {
            free(ci->akick[i].mask);
            free(ci->akick[i].reason);
        }
        free(ci->akick);
        free(ci->mlock.key);
        free(ci->mlock.link);
        free(ci->mlock.flood);
        free(ci->entry_message);
        free(ci);
    }  /* if (ci) */
}

/*************************************************************************/

/* Reset channel access level values to their default state. */

EXPORT_FUNC(reset_levels)
STANDALONE_STATIC void reset_levels(ChannelInfo *ci)
{
    int i;
    for (i = 0; i < CA_SIZE; i++)
        ci->levels[i] = ACCLEV_DEFAULT;
}

/*************************************************************************/

#ifndef STANDALONE_CHANSERV  /* to the end of the file */

/*************************************************************************/

/* Check the nick group's number of registered channels against its limit,
 * and return -1 if below the limit, 0 if at it exactly, and 1 if over it.
 * If `max_ret' is non-NULL, store the nick group's registered channel
 * limit there.
 */

EXPORT_FUNC(check_channel_limit)
int check_channel_limit(const NickGroupInfo *ngi, int *max_ret)
{
    register int max, count;

    max = ngi->channelmax;
    if (max == CHANMAX_DEFAULT)
        max = CSMaxReg;
    else if (max == CHANMAX_UNLIMITED)
        max = MAX_CHANNELCOUNT;
    count = ngi->channels_count;
    if (max_ret)
        *max_ret = max;
    return count<max ? -1 : count==max ? 0 : 1;
}

/*************************************************************************/
/*********************** Internal utility routines ***********************/
/*************************************************************************/

/* Add a channel to the database.  Returns a pointer to the new ChannelInfo
 * structure if the channel was successfully registered, NULL otherwise.
 * Assumes channel does not already exist. */

ChannelInfo *makechan(const char *chan)
{
    ChannelInfo *ci;

    ci = scalloc(sizeof(ChannelInfo), 1);
    strbcpy(ci->name, chan);
    ci->time_registered = time(NULL);
    reset_levels(ci);
    return add_channelinfo(ci);
}

/*************************************************************************/

/* Remove a channel from the ChanServ database.  Return 1 on success, 0
 * otherwise. */

int delchan(ChannelInfo *ci)
{
    User *u;
    Channel *c;

    /* Remove channel from founder's owned-channel count */
    uncount_chan(ci);

    /* Clear link (and "registered" mode) from channel record if it exists */
    c = ci->c;
    if (c) {
        c->ci = NULL;
        if (chanmode_reg) {
            c->mode &= ~chanmode_reg;
            /* Send this out immediately, no set_cmode() delay */
            send_cmode_cmd(s_ChanServ, ci->name, "-%s",
                           mode_flags_to_string(chanmode_reg, MODE_CHANNEL));
        }
    }

    /* Clear channel from users' identified-channel lists */
    for (u = first_user(); u; u = next_user()) {
        struct u_chaninfolist *uc, *next;
        LIST_FOREACH_SAFE (uc, u->id_chans, next) {
            if (irc_stricmp(uc->chan, ci->name) == 0) {
                LIST_REMOVE(uc, u->id_chans);
                free(uc);
            }
        }
    }

    /* Now actually delete record */
    del_channelinfo(ci);

    return 1;
}

/*************************************************************************/

/* Mark the given channel as owned by its founder.  This updates the
 * founder's list of owned channels (ngi->channels).
 */

void count_chan(const ChannelInfo *ci)
{
    NickGroupInfo *ngi = ci->founder ? get_ngi_id(ci->founder) : NULL;

    if (!ngi)
        return;
    /* Be paranoid--this could overflow in extreme cases, though we check
     * for that elsewhere as well. */
    if (ngi->channels_count >= MAX_CHANNELCOUNT) {
        module_log("count BUG: overflow in ngi->channels_count for %u (%s)"
                   " on %s", ngi->id, ngi_mainnick(ngi), ci->name);
        return;
    }
    ARRAY_EXTEND(ngi->channels);
    strbcpy(ngi->channels[ngi->channels_count-1], ci->name);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

/* Mark the given channel as no longer owned by its founder. */

void uncount_chan(const ChannelInfo *ci)
{
    NickGroupInfo *ngi = ci->founder ? get_ngi_id(ci->founder) : NULL;
    int i;

    if (!ngi)
        return;
    ARRAY_SEARCH_PLAIN(ngi->channels, ci->name, irc_stricmp, i);
    if (i >= ngi->channels_count) {
        module_log("uncount BUG: channel not found in channels[] for %u (%s)"
                   " on %s", ngi->id,
                   ngi->nicks_count ? ngi_mainnick(ngi) : "<unknown>",
                   ci->name);
        return;
    }
    ARRAY_REMOVE(ngi->channels, i);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

/* Does the given user have founder access to the channel? */

int is_founder(const User *user, const ChannelInfo *ci)
{
    if (!ci || (ci->flags & (CF_VERBOTEN | CF_SUSPENDED)) || !valid_ngi(user))
        return 0;
    if (user->ngi->id == ci->founder) {
        if ((user_identified(user) ||
                 (user_recognized(user) && !(ci->flags & CF_SECURE))))
            return 1;
    }
    if (is_identified(user, ci))
        return 1;
    return 0;
}

/*************************************************************************/

/* Has the given user password-identified as founder for the channel? */

int is_identified(const User *user, const ChannelInfo *ci)
{
    struct u_chaninfolist *c;

    if (!ci)
        return 0;
    LIST_FOREACH (c, user->id_chans) {
        if (irc_stricmp(c->chan, ci->name) == 0)
            return 1;
    }
    return 0;
}

/*************************************************************************/

/* Restore the topic in a channel when it's created, if we should. */

void restore_topic(Channel *c)
{
    ChannelInfo *ci = c->ci;

    if (ci && (ci->flags & CF_KEEPTOPIC) && ci->last_topic && *ci->last_topic){
        set_topic(s_ChanServ, c, ci->last_topic,
                  *ci->last_topic_setter ? ci->last_topic_setter : s_ChanServ,
                  ci->last_topic_time);
    }
}

/*************************************************************************/

/* Record a channel's topic in its ChannelInfo structure. */

void record_topic(ChannelInfo *ci, const char *topic,
                  const char *setter, time_t topic_time)
{
    if (!ci)
        return;
    free(ci->last_topic);
    if (*topic)
        ci->last_topic = sstrdup(topic);
    else
        ci->last_topic = NULL;
    strbcpy(ci->last_topic_setter, setter);
    ci->last_topic_time = topic_time;
}

/*************************************************************************/

/* Mark the given channel as suspended. */

void suspend_channel(ChannelInfo *ci, const char *reason,
                     const char *who, const time_t expires)
{
    strbcpy(ci->suspend_who, who);
    ci->suspend_reason  = sstrdup(reason);
    ci->suspend_time    = time(NULL);
    ci->suspend_expires = expires;
    ci->flags |= CF_SUSPENDED;
}

/*************************************************************************/

/* Delete the suspension data for the given channel.  We also alter the
 * last_seen value to ensure that it does not expire within the next
 * CSSuspendGrace seconds, giving its users a chance to reclaim it
 * (but only if set_time is non-zero).
 */

void unsuspend_channel(ChannelInfo *ci, int set_time)
{
    time_t now = time(NULL);

    if (!(ci->flags & CF_SUSPENDED)) {
        module_log("unsuspend_channel() called on non-suspended channel %s",
                   ci->name);
        return;
    }
    ci->flags &= ~CF_SUSPENDED;
    free(ci->suspend_reason);
    memset(ci->suspend_who, 0, sizeof(ci->suspend_who));
    ci->suspend_reason  = NULL;
    ci->suspend_time    = 0;
    ci->suspend_expires = 0;
    if (set_time && CSExpire && CSSuspendGrace
     && (now - ci->last_used >= CSExpire - CSSuspendGrace)
    ) {
        ci->last_used = now - CSExpire + CSSuspendGrace;
        module_log("unsuspend: Altering last_used time for %s to %ld",
                   ci->name, (long)ci->last_used);
    }
}

/*************************************************************************/

/* Register a bad password attempt for a channel. */

void chan_bad_password(User *u, ChannelInfo *ci)
{
    bad_password(s_ChanServ, u, ci->name);
    ci->bad_passwords++;
    if (BadPassWarning && ci->bad_passwords == BadPassWarning) {
        wallops(s_ChanServ, "\2Warning:\2 Repeated bad password attempts"
                            " for channel %s (last attempt by user %s)",
                ci->name, u->nick);
    }
}

/*************************************************************************/

/* Return the ChanOpt structure for the given option name.  If not found,
 * return NULL.
 */

ChanOpt *chanopt_from_name(const char *optname)
{
    int i;
    for (i = 0; chanopts[i].name; i++) {
        if (stricmp(chanopts[i].name, optname) == 0)
            return &chanopts[i];
    }
    return NULL;
}

/*************************************************************************/

/* Return a string listing the options (those given in chanopts[]) set on
 * the given channel.  Uses the given NickGroupInfo for language
 * information.  The returned string is stored in a static buffer which
 * will be overwritten on the next call.
 */

char *chanopts_to_string(const ChannelInfo *ci, const NickGroupInfo *ngi)
{
    static char buf[BUFSIZE];
    char *end = buf;
    const char *commastr = getstring(ngi, COMMA_SPACE);
    const char *s;
    int need_comma = 0;
    int i;

    for (i = 0; chanopts[i].name && end-buf < sizeof(buf)-1; i++) {
        if (chanopts[i].namestr < 0)
            continue;
        if (ci->flags & chanopts[i].flag) {
            s = getstring(ngi, chanopts[i].namestr);
            if (!s)
                continue;
            if (need_comma)
                end += snprintf(end, sizeof(buf)-(end-buf), "%s", commastr);
            end += snprintf(end, sizeof(buf)-(end-buf), "%s", s);
            need_comma = 1;
        }
    }
    return buf;
}

/*************************************************************************/

#endif  /* !STANDALONE_CHANSERV */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
