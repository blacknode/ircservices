/* NickServ utility routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

/* If STANDALONE_NICKSERV is defined when compiling this file, the
 * following four routines will be defined as `static' for use in other
 * modules/programs:
 *    new_nickinfo()
 *    free_nickinfo()
 *    new_nickgroupinfo()
 *    free_nickgroupinfo()
 * This file should be #include'd in the file making use of the routines.
 * Note that new_nickgroupinfo() will not check for the presence of any
 * nickgroup ID it assigns, since get_nickgroupinfo() is not assumed to be
 * available.
 */

#ifdef STANDALONE_NICKSERV

# define STANDALONE_STATIC static
# undef EXPORT_FUNC
# define EXPORT_FUNC(x) /*nothing*/

#else

# define STANDALONE_STATIC /*nothing*/

# include "services.h"
# include "modules.h"
# include "language.h"
# include "encrypt.h"

# include "nickserv.h"
# include "ns-local.h"

#endif

/*************************************************************************/

#ifndef STANDALONE_NICKSERV

static int cb_set_identified   = -1;
static int cb_cancel_user      = -1;
static int cb_check_recognized = -1;
static int cb_delete           = -1;
static int cb_groupdelete      = -1;

#endif

/*************************************************************************/
/************************ Global utility routines ************************/
/*************************************************************************/

/* Allocate and initialize a new NickInfo structure. */

EXPORT_FUNC(new_nickinfo)
STANDALONE_STATIC NickInfo *new_nickinfo(void)
{
    NickInfo *ni = scalloc(sizeof(*ni), 1);
    return ni;
}

/*************************************************************************/

/* Free a NickInfo structure and all associated data. */

EXPORT_FUNC(free_nickinfo)
STANDALONE_STATIC void free_nickinfo(NickInfo *ni)
{
    if (ni) {
        free(ni->last_usermask);
        free(ni->last_realmask);
        free(ni->last_realname);
        free(ni->last_quit);
        free(ni);
    }
}

/*************************************************************************/

/* Allocate and initialize a new NickGroupInfo structure.  If `seed' is
 * non-NULL, the group ID will be set to a value unique from any other
 * currently stored in the database (in this case `seed' is used in the
 * initial attempt to allocate the ID).  If `seed' is NULL, the ID value
 * is left at zero.
 */

EXPORT_FUNC(new_nickgroupinfo)
STANDALONE_STATIC NickGroupInfo *new_nickgroupinfo(const char *seed)
{
    NickGroupInfo *ngi = scalloc(sizeof(*ngi), 1);

    if (seed) {
        uint32 id;
        int count;

        id = 0;
        for (count = 0; seed[count] != 0; count++)
            id ^= seed[count] << ((count % 6) * 5);
        if (id == 0)
            id = 1;
#ifndef STANDALONE_NICKSERV
        count = 0;
        while (put_nickgroupinfo(get_nickgroupinfo(id)) != NULL
               && ++count < NEWNICKGROUP_TRIES
        ) {
            id = rand() + rand();       /* 32 bits of randomness, hopefully */
            if (id == 0)
                id = 1;
        }
        if (count >= NEWNICKGROUP_TRIES) {
            module_log("new_nickgroupinfo() unable to allocate unused ID"
                       " after %d tries!  Giving up.", NEWNICKGROUP_TRIES);
            free(ngi);
            return NULL;
        }
#endif
        ngi->id = id;
    }

    ngi->memos.memomax = MEMOMAX_DEFAULT;
    ngi->channelmax = CHANMAX_DEFAULT;
    ngi->language = LANG_DEFAULT;
    ngi->timezone = TIMEZONE_DEFAULT;
    return ngi;
}

/*************************************************************************/

/* Free a NickGroupInfo structure and all associated data. */

EXPORT_FUNC(free_nickgroupinfo)
STANDALONE_STATIC void free_nickgroupinfo(NickGroupInfo *ngi)
{
    int i;

    if (!ngi)
        return;
    free(ngi->nicks);
    clear_password(&ngi->pass);
    free(ngi->url);
    free(ngi->email);
    free(ngi->info);
    free(ngi->suspend_reason);
    ARRAY_FOREACH (i, ngi->access)
        free(ngi->access[i]);
    free(ngi->access);
    ARRAY_FOREACH (i, ngi->ajoin)
        free(ngi->ajoin[i]);
    free(ngi->ajoin);
    free(ngi->channels);
    ARRAY_FOREACH (i, ngi->memos.memos) {
        free(ngi->memos.memos[i].channel);
        free(ngi->memos.memos[i].text);
    }
    free(ngi->memos.memos);
    ARRAY_FOREACH (i, ngi->ignore)
        free(ngi->ignore[i]);
    free(ngi->ignore);
    ARRAY_FOREACH (i, ngi->id_users) {
        User *u = ngi->id_users[i];
        int j;
        ARRAY_SEARCH_PLAIN_SCALAR(u->id_nicks, ngi->id, j);
        if (j < u->id_nicks_count) {
            ARRAY_REMOVE(u->id_nicks, j);
#ifndef STANDALONE_NICKSERV
        } else {
            module_log("BUG: free_nickgroupinfo(): user %p (%s) listed in"
                       " id_users for nickgroup %u, but nickgroup not in"
                       " id_nicks!", u, u->nick, ngi->id);
#endif
        }
    }
    free(ngi->id_users);
    free(ngi);
}

/*************************************************************************/

#ifndef STANDALONE_NICKSERV  /* to the end of the file */

/*************************************************************************/

/* Retrieve the NickGroupInfo structure associated with the given NickInfo.
 * If it cannot be retrieved, log an error message and return NULL.
 * Note: Callers should use get_ngi[_id]() or check_ngi[_id](), which embed
 *       the current source file and line number in the log message.
 */

EXPORT_FUNC(_get_ngi)
NickGroupInfo *_get_ngi(const NickInfo *ni, const char *file, int line)
{
    NickGroupInfo *ngi;

    if (!ni) {
        module_log("BUG: ni==NULL in get_ngi() (called from %s:%d)",
                   file, line);
        return NULL;
    }
    ngi = get_nickgroupinfo(ni->nickgroup);
    if (!ngi)
        module_log("Unable to get NickGroupInfo (id %u) for %s at %s:%d",
                   ni->nickgroup, ni->nick, file, line);
    return ngi;
}

EXPORT_FUNC(_get_ngi_id)
NickGroupInfo *_get_ngi_id(uint32 id, const char *file, int line)
{
    NickGroupInfo *ngi = get_nickgroupinfo(id);
    if (!ngi)
        module_log("Unable to get NickGroupInfo (id %u) at %s:%d",
                   id, file, line);
    return ngi;
}

/*************************************************************************/

/* Return whether the user has previously identified for the given nick
 * group (1=yes, 0=no).  If the nick group does not exist or is in a
 * not-authenticated state, this function will always return 0.
 */

EXPORT_FUNC(has_identified_nick)
int has_identified_nick(const User *u, uint32 group)
{
    int i, unauthed;
    NickGroupInfo *ngi;

    ngi = get_ngi_id(group);
    unauthed = ngi_unauthed(ngi);
    put_nickgroupinfo(ngi);
    if (!ngi || unauthed)
        return 0;
    ARRAY_SEARCH_PLAIN_SCALAR(u->id_nicks, group, i);
    return i < u->id_nicks_count;
}

/*************************************************************************/
/*********************** Internal utility routines ***********************/
/*************************************************************************/

/* Check whether the given nickname is allowed to be registered (with the
 * given password and E-mail address) or linked (password/email NULL).
 * Returns nonzero if allowed, 0 if not.  A wrapper for the "REGISTER/LINK
 * check" callback.
 */

int reglink_check(User *u, const char *nick, char *password, char *email)
{
    int res = call_callback_4(cb_reglink_check, u, nick, password, email);
    if (res < 0)
        module_log("REGISTER/LINK callback returned an error");
    return res == 0;
}

/*************************************************************************/

/* Update the last_usermask, last_realmask, and last_realname nick fields
 * from the given user's data, and sets last_seen to the current time.
 * Assumes u->ni is non-NULL.
 */

void update_userinfo(const User *u)
{
    const char *host;
    NickInfo *ni = u->ni;

    ni->last_seen = time(NULL);
    free(ni->last_usermask);
    if (u->fakehost)
        host = u->fakehost;
    else
        host = u->host;
    ni->last_usermask = smalloc(strlen(u->username)+strlen(host)+2);
    sprintf(ni->last_usermask, "%s@%s", u->username, host);
    free(ni->last_realmask);
    ni->last_realmask = smalloc(strlen(u->username)+strlen(u->host)+2);
    sprintf(ni->last_realmask, "%s@%s", u->username, u->host);
    free(ni->last_realname);
    ni->last_realname = sstrdup(u->realname);
}

/*************************************************************************/

/* Check whether a user is recognized for the nick they're using, or if
 * they're the same user who last identified for the nick.  If not, send
 * warnings as appropriate.  If so (and not NI_SECURE), update last seen
 * info.  Return 1 if the user is valid and recognized, 0 otherwise (note
 * that this means an NI_SECURE nick will return 0 from here unless the
 * user has identified for the nick before).  If the user's nick is not
 * registered, 0 is returned.
 *
 * The `nick' parameter is the nick the user is using.  It is passed
 * separately to handle cases where the User structure has a different
 * nick (e.g. when changing nicks).
 */

int validate_user(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    int is_recognized;

    if (u->ni)
        put_nickinfo(u->ni);
    if (u->ngi && u->ngi != NICKGROUPINFO_INVALID)
        put_nickgroupinfo(u->ngi);
    ni = get_nickinfo(u->nick);
    if (ni && ni->nickgroup) {
        ngi = get_ngi(ni);
        if (!ngi) {
            put_nickinfo(ni);
            ni = NULL;
            ngi = NICKGROUPINFO_INVALID;
        }
    } else {
        ngi = NULL;
    }
    u->ni = ni;
    u->ngi = ngi;
    if (!ni)
        return 0;

    ni->authstat = 0;
    ni->user = u;

    if ((ni->status & NS_VERBOTEN) || (ngi->flags & NF_SUSPENDED)) {
        if (usermode_reg) {
            send_cmd(s_NickServ, "SVSMODE %s :-%s", u->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
        }
        notice_lang(s_NickServ, u, NICK_MAY_NOT_BE_USED);
        notice_lang(s_NickServ, u, DISCONNECT_IN_1_MINUTE);
        add_ns_timeout(ni, TO_SEND_433, 40);
        add_ns_timeout(ni, TO_COLLIDE, 60);
        return 0;
    }

    if (!ngi_unauthed(ngi)) {
        /* Neither of these checks should be done if the nick is awaiting
         * authentication */

        if (has_identified_nick(u, ngi->id)) {
            set_identified(u);
            return 1;
        }

        if (!NoSplitRecovery) {
            /*
             * This can be exploited to gain improper privilege if an
             * attacker has the same Services stamp, username and hostname
             * as the victim.
             *
             * Under ircd.dal 4.4.15+ (Dreamforge) and other servers
             * supporting a Services stamp, Services guarantees that the
             * first condition cannot occur unless the stamp counter rolls
             * over, which happens every 2^31-1 client connections.  This
             * is practically infeasible given present technology.  As an
             * example, on a network of 30 servers, an attack introducing
             * 50 new clients every second on every server (requiring at
             * least 10-15 megabits of bandwidth) would need to be
             * sustained for over 16 days to cause the stamp to roll over.
             * Note, however, that since the Services stamp counter is
             * initialized randomly at startup, a restart of Services will
             * change this situation unpredictably; in the worst case, the
             * stamp could be initialized to a value already in use on the
             * network.
             *
             * Under other servers, an attack is theoretically possible,
             * but would require access to either the computer the victim
             * is using for IRC or the DNS servers for the victim's domain
             * and IP address range in order to have the same hostname, and
             * would require that the attacker connect so that he has the
             * same server timestamp as the victim.  Practically, the
             * former can be accomplished either by finding a victim who
             * uses a shell account on a multiuser system and obtaining an
             * account on the same system, or through the scripting
             * capabilities of many IRC clients combined with social
             * engineering; the latter could be accomplished by finding a
             * server with a clock slower than that of the victim's server
             * and timing the connection attempt properly.
             *
             * If someone gets a hacked server into your network, all bets
             * are off.
             */
            if (ni->id_stamp != 0 && u->servicestamp == ni->id_stamp) {
                char buf[BUFSIZE];
                snprintf(buf, sizeof(buf), "%s@%s", u->username, u->host);
                if (ni->last_realmask && strcmp(buf, ni->last_realmask) == 0) {
                    set_identified(u);
                    return 1;
                }
            }
        }

    }  /* if (!ngi_unauthed(ngi)) */

    /* From here on down, the user is known to be not identified.  Clear
     * any "registered nick" mode from them. */
    if (usermode_reg) {
        send_cmd(s_NickServ, "SVSMODE %s :-%s", u->nick,
                 mode_flags_to_string(usermode_reg, MODE_USER));
    }

    is_recognized = (call_callback_1(cb_check_recognized, u) == 1);

    if (!(ngi->flags & NF_SECURE) && !ngi_unauthed(ngi) && is_recognized) {
        ni->authstat |= NA_RECOGNIZED;
        update_userinfo(u);
        return 1;
    }

    if (is_recognized || ngi_unauthed(ngi) || !NSAllowKillImmed
     || !(ngi->flags & NF_KILL_IMMED)
    ) {
        if (ngi->flags & NF_SECURE)
            notice_lang(s_NickServ, u, NICK_IS_SECURE, s_NickServ);
        else
            notice_lang(s_NickServ, u, NICK_IS_REGISTERED, s_NickServ);
    }

    if ((ngi->flags & NF_KILLPROTECT) && !is_recognized) {
        if (!ngi_unauthed(ngi)
         && NSAllowKillImmed
         && (ngi->flags & NF_KILL_IMMED)
        ) {
            collide_nick(ni, 0);
        } else if (!ngi_unauthed(ngi) && (ngi->flags & NF_KILL_QUICK)) {
            notice_lang(s_NickServ, u, DISCONNECT_IN_20_SECONDS);
            add_ns_timeout(ni, TO_COLLIDE, 20);
            add_ns_timeout(ni, TO_SEND_433, 10);
        } else {
            notice_lang(s_NickServ, u, DISCONNECT_IN_1_MINUTE);
            add_ns_timeout(ni, TO_COLLIDE, 60);
            add_ns_timeout(ni, TO_SEND_433, 40);
        }
    }

    if (!noexpire && NSExpire && NSExpireWarning
     && !(ni->status & NS_NOEXPIRE)
    ) {
        int time_left = NSExpire - (time(NULL) - ni->last_seen);
        if (time_left <= NSExpireWarning) {
            notice_lang(s_NickServ, u, NICK_EXPIRES_SOON,
                        maketime(ngi,time_left,0), s_NickServ, s_NickServ);
        }
    }

    return 0;
}

/*************************************************************************/

/* Cancel validation flags for a nick (i.e. when the user with that nick
 * signs off or changes nicks).  Also cancels any impending collide, sets
 * the nick's last seen time if the user is recognized for the nick and
 * unlocks the nick's info records.
 */

void cancel_user(User *u)
{
    if (u->ni) {
        NickInfo *ni = u->ni;
        int old_status = ni->status;
        int old_authstat = ni->authstat;

        if (nick_id_or_rec(ni))
            ni->last_seen = time(NULL);
        ni->user = NULL;
        ni->status &= ~NS_TEMPORARY;
        ni->authstat = 0;
        if (old_status & NS_GUESTED)
            introduce_enforcer(ni);
        call_callback_3(cb_cancel_user, u, old_status, old_authstat);
        rem_ns_timeout(ni, TO_COLLIDE, 1);
        put_nickinfo(u->ni);
        put_nickgroupinfo(u->ngi);
    }
    u->ni = NULL;
    u->ngi = NULL;
}

/*************************************************************************/

/* Marks a user as having identified for the given nick. */

void set_identified(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    if (!u || !(ni = u->ni) || !(ngi = u->ngi) || ngi==NICKGROUPINFO_INVALID) {
        module_log("BUG: set_identified() for unregistered nick! user%s%s",
                   u ? "=" : " is NULL", u ? u->nick : "");
        return;
    }

    int old_authstat = ni->authstat;
    ni->authstat &= ~NA_IDENT_NOMAIL;
    ni->authstat |= NA_IDENTIFIED;
    ni->id_stamp = u->servicestamp;
    if (!nick_recognized(ni)) {
        update_userinfo(u);
        ni->authstat |= NA_RECOGNIZED;
    }
    if (!has_identified_nick(u, ngi->id)) {
        ARRAY_EXTEND(u->id_nicks);
        u->id_nicks[u->id_nicks_count-1] = ngi->id;
        ARRAY_EXTEND(ngi->id_users);
        ngi->id_users[ngi->id_users_count-1] = u;
    }
    if (usermode_reg) {
        send_cmd(s_NickServ, "SVSMODE %s :+%s", u->nick,
                 mode_flags_to_string(usermode_reg, MODE_USER));
    }
    call_callback_2(cb_set_identified, u, old_authstat);
}

/*************************************************************************/
/*************************************************************************/

/* Add a nick to the database.  Returns a pointer to the new NickInfo
 * structure if the nick was successfully registered, NULL otherwise.
 * Assumes nick does not already exist.  If `nickgroup_ret' is non-NULL,
 * a new NickGroupInfo structure is created, ni->nickgroup/ngi->*nick*
 * are set appropriately, and a pointer to the NickGroupInfo is returned
 * in that variable.  Note that makenick() may return NULL if a nick group
 * is requested and new_nickgroupinfo() returns NULL.
 *
 * If `nick' is longer than NICKMAX-1 characters, it is silently truncated
 * in the new NickInfo and NickGroupInfo.
 */

NickInfo *makenick(const char *nick, NickGroupInfo **nickgroup_ret)
{
    NickInfo *ni;
    NickGroupInfo *ngi;

#if CLEAN_COMPILE
    ngi = NULL;
#endif
    if (nickgroup_ret) {
        ngi = new_nickgroupinfo(nick);
        if (!ngi)
            return NULL;
    }
    ni = new_nickinfo();
    strbcpy(ni->nick, nick);
    if (nickgroup_ret) {
        ni->nickgroup = ngi->id;
        ARRAY_EXTEND(ngi->nicks);
        strbcpy(ngi->nicks[0], nick);
        *nickgroup_ret = add_nickgroupinfo(ngi);
    }
    return add_nickinfo(ni);
}

/*************************************************************************/

/* Remove a nick from the NickServ database.  Return 1 on success, 0
 * otherwise.
 */

int delnick(NickInfo *ni)
{
    int i;
    NickGroupInfo *ngi;

    rem_ns_timeout(ni, -1, 1);
    if (ni->status & NS_KILL_HELD)
        release_nick(ni, 0);
    if (ni->user) {
        if (usermode_reg)
            send_cmd(s_NickServ, "SVSMODE %s :-%s", ni->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
        ni->user->ni = NULL;
        ni->user->ngi = NULL;
    }

    ngi = ni->nickgroup ? get_nickgroupinfo(ni->nickgroup) : NULL;
    if (ngi) {
        ARRAY_SEARCH_PLAIN(ngi->nicks, ni->nick, irc_stricmp, i);
        if (i >= ngi->nicks_count) {
            module_log("BUG: delete nick: no entry in ngi->nicks[] for"
                       " nick %s", ni->nick);
        } else {
            ARRAY_REMOVE(ngi->nicks, i);
            /* Adjust ngi->mainnick as needed; if the main nick is being
             * deleted, switch it to the next one in the array, or the
             * previous if it's currently the last one. */
            if (ngi->mainnick > i || ngi->mainnick >= ngi->nicks_count)
                ngi->mainnick--;
        }
    }
    call_callback_1(cb_delete, ni);
    if (ngi) {
        if (ngi->nicks_count == 0) {
            call_callback_2(cb_groupdelete, ngi, ni->nick);
            del_nickgroupinfo(ngi);
        } else {
            put_nickgroupinfo(ngi);
        }
    }
    del_nickinfo(ni);
    return 1;
}

/*************************************************************************/

/* Remove a nick group and all associated nicks from the NickServ database.
 * Return 1 on success, 0 otherwise.
 */

int delgroup(NickGroupInfo *ngi)
{
    int i;

    ARRAY_FOREACH (i, ngi->nicks) {
        NickInfo *ni = get_nickinfo(ngi->nicks[i]);
        if (!ni) {
            /* We might have just caused it to expire, so don't log any
             * error messages */
            continue;
        }
        rem_ns_timeout(ni, -1, 1);
        if (ni->status & NS_KILL_HELD) {
            release_nick(ni, 0);
        }
        if (ni->user) {
            if (usermode_reg)
                send_cmd(s_NickServ, "SVSMODE %s :-%s", ni->nick,
                         mode_flags_to_string(usermode_reg, MODE_USER));
            ni->user->ni = NULL;
            ni->user->ngi = NULL;
        }
        call_callback_1(cb_delete, ni);
        del_nickinfo(ni);
    }
    call_callback_2(cb_groupdelete, ngi, ngi_mainnick(ngi));
    del_nickgroupinfo(ngi);
    return 1;
}

/*************************************************************************/

/* Drop a nickgroup, logging appropriate information about it first.  `u'
 * is the user dropping the nickgroup; `dropemail' should have one of the
 * following values:
 *    - NULL for a user dropping their own nick
 *    - PTR_INVALID for a DROPNICK from a Services admin
 *    - The pattern used for a DROPEMAIL from a Services admin
 * Returns the result of delgroup(ngi).
 */

int drop_nickgroup(NickGroupInfo *ngi, const User *u, const char *dropemail)
{
    int i;

    module_log("%s!%s@%s dropped nickgroup %u%s%s%s%s%s%s%s:",
               u->nick, u->username, u->host, ngi->id,
               ngi->email ? " (E-mail " : "",
               ngi->email ? ngi->email : "",
               ngi->email ? ")" : "",
               dropemail ? " as Services admin" : "",
               (dropemail && dropemail!=PTR_INVALID) ? " (DROPEMAIL on " : "",
               (dropemail && dropemail!=PTR_INVALID) ? dropemail : "",
               (dropemail && dropemail!=PTR_INVALID) ? ")" : "");
    ARRAY_FOREACH (i, ngi->nicks) {
        module_log(" -- %s!%s@%s dropped nick %s",
                   u->nick, u->username, u->host, ngi->nicks[i]);
    }
    return delgroup(ngi);
}

/*************************************************************************/

/* Suspend the given nick group. */

void suspend_nick(NickGroupInfo *ngi, const char *reason,
                  const char *who, const time_t expires)
{
    strbcpy(ngi->suspend_who, who);
    ngi->suspend_reason  = sstrdup(reason);
    ngi->suspend_time    = time(NULL);
    ngi->suspend_expires = expires;
    ngi->flags |= NF_SUSPENDED;
}

/*************************************************************************/

/* Delete the suspension data for the given nick group.  We also alter the
 * last_seen value for all nicks in this group to ensure that they do not
 * expire within the next NSSuspendGrace seconds, giving the owner a chance
 * to reclaim them (but only if set_time is nonzero).
 */

void unsuspend_nick(NickGroupInfo *ngi, int set_time)
{
    time_t now = time(NULL);

    if (!(ngi->flags & NF_SUSPENDED)) {
        module_log("unsuspend: called on non-suspended nick group %u [%s]",
                   ngi->id, ngi->nicks[0]);
        return;
    }
    ngi->flags &= ~NF_SUSPENDED;
    free(ngi->suspend_reason);
    memset(ngi->suspend_who, 0, sizeof(ngi->suspend_who));
    ngi->suspend_reason  = NULL;
    ngi->suspend_time    = 0;
    ngi->suspend_expires = 0;
    if (set_time && NSExpire && NSSuspendGrace) {
        int i;
        if (ngi->authcode) {
            ngi->authset = now;
            module_log("unsuspend: altering authset time for %s (nickgroup"
                       " %u) to %ld", ngi_mainnick(ngi), ngi->id,
                       (long)ngi->authset);
        }
        ARRAY_FOREACH (i, ngi->nicks) {
            NickInfo *ni = get_nickinfo(ngi->nicks[i]);
            if (!ni)
                continue;
            if (now - ni->last_seen >= NSExpire - NSSuspendGrace) {
                ni->last_seen = now - NSExpire + NSSuspendGrace;
                module_log("unsuspend: altering last_seen time for %s to %ld",
                           ni->nick, (long)ni->last_seen);
            }
            put_nickinfo(ni);
        }
    }
}

/*************************************************************************/
/*************************************************************************/

/* Check the password given by the user and handle errors if it's not
 * correct (or the encryption routine fails).  `command' is the name of
 * the command calling this function, and `failure_msg' is the message to
 * send to the user if the encryption routine fails.  Returns 1 if the
 * password matches, 0 otherwise.
 */

int nick_check_password(User *u, NickInfo *ni, const char *password,
                        const char *command, int failure_msg)
{
    int res;
    NickGroupInfo *ngi = get_ngi(ni);

    if (!ngi) {
        module_log("%s: no nickgroup for %s, aborting password check",
                   command, ni->nick);
        notice_lang(s_NickServ, u, failure_msg);
        return 0;
    }

    res = check_password(password, &ngi->pass);
    put_nickgroupinfo(ngi);
    if (res == 0) {
        module_log("%s: bad password for %s from %s!%s@%s",
                   command, ni->nick, u->nick, u->username, u->host);
        bad_password(s_NickServ, u, ni->nick);
        ni->bad_passwords++;
        if (BadPassWarning && ni->bad_passwords == BadPassWarning) {
            wallops(s_NickServ, "\2Warning:\2 Repeated bad password attempts"
                    " for nick %s", ni->nick);
        }
        return 0;
    } else if (res == -1) {
        module_log("%s: check_password failed for %s",
                   command, ni->nick);
        notice_lang(s_NickServ, u, failure_msg);
        return 0;
    } else {
        ni->bad_passwords = 0;
        return 1;
    }
}

/*************************************************************************/

/* Return the number of registered nicknames with the given E-mail address.
 * If a registered nickname matches the given E-mail address but the
 * address has not been authenticated, return the negative of the number of
 * registered nicknames: for example, if there are 5 nicks with the given
 * address, -5 would be returned.
 *
 * Note that this function is O(n) in the total number of registered nick
 * groups, so do not use it lightly!
 */

int count_nicks_with_email(const char *email)
{
    int count = 0, unauthed = 0;
    NickGroupInfo *ngi;

    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        if (ngi->email && stricmp(ngi->email, email) == 0) {
            if (ngi_unauthed(ngi))
                unauthed = 1;
            count += ngi->nicks_count;
        }
    }
    return unauthed ? -count : count;
}

/*************************************************************************/
/*************************************************************************/

int init_util(void)
{
    cb_set_identified   = register_callback("set identified");
    cb_cancel_user      = register_callback("cancel user");
    cb_check_recognized = register_callback("check recognized");
    cb_delete           = register_callback("nick delete");
    cb_groupdelete      = register_callback("nickgroup delete");
    if (cb_cancel_user < 0 || cb_check_recognized < 0 || cb_delete < 0
     || cb_groupdelete < 0
    ) {
        module_log("Unable to register callbacks (util.c)");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void exit_util()
{
    unregister_callback(cb_groupdelete);
    unregister_callback(cb_delete);
    unregister_callback(cb_check_recognized);
    unregister_callback(cb_cancel_user);
    unregister_callback(cb_set_identified);
}

/*************************************************************************/

#endif  /* !STANDALONE_NICKSERV */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
