/* Various routines to perform simple actions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "language.h"
#include "modules.h"
#include "timeout.h"

/*************************************************************************/

static int cb_clear_channel = -1;
static int cb_set_topic = -1;

/* Sender to be used with clear_channel() (empty string: use server name) */
static char clear_channel_sender[NICKMAX] = {0};

/*************************************************************************/
/*************************************************************************/

int actions_init(int ac, char **av)
{
    cb_clear_channel = register_callback("clear channel");
    cb_set_topic     = register_callback("set topic");
    if (cb_clear_channel < 0 || cb_set_topic < 0) {
        log("actions_init: register_callback() failed\n");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void actions_cleanup(void)
{
    unregister_callback(cb_set_topic);
    unregister_callback(cb_clear_channel);
}

/*************************************************************************/
/*************************************************************************/

/* Note a bad password attempt for the given user from the given service.
 * If they've used up their limit, toss them off.  `service' is used only
 * for the sender of the bad-password and warning messages; these messages
 * are not sent if `service' is NULL.  `what' describes what the password
 * was for, and is used in the kill message if the user is killed.  The
 * function's return value is 1 if the user was warned, 2 if the user was
 * killed, and 0 otherwise.
 */

int bad_password(const char *service, User *u, const char *what)
{
    time_t now = time(NULL);

    if (service)
        notice_lang(service, u, PASSWORD_INCORRECT);

    if (!BadPassLimit)
        return 0;

    if (BadPassTimeout > 0 && u->bad_pw_time > 0
                        && now >= u->bad_pw_time + BadPassTimeout)
        u->bad_pw_count = 0;
    u->bad_pw_count++;
    u->bad_pw_time = now;
    if (u->bad_pw_count >= BadPassLimit) {
        char buf[BUFSIZE];
        snprintf(buf, sizeof(buf), "Too many invalid passwords (%s)", what);
        kill_user(NULL, u->nick, buf);
        return 2;
    } else if (u->bad_pw_count == BadPassLimit-1) {
        if (service)
            notice_lang(service, u, PASSWORD_WARNING);
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Clear modes/users from a channel.  The "what" parameter is one or more
 * of the CLEAR_* constants defined in services.h.  "param" is:
 *     - for CLEAR_USERS, a kick message (const char *)
 *     - for CLEAR_UMODES, a bitmask of modes to clear (int32)
 *     - for CLEAR_BANS and CLEAR_EXCEPTS, a User * to match against, or
 *          NULL for all bans/exceptions
 * Note that CLEAR_EXCEPTS must be handled via callback for protocols which
 * support it.
 */

static void clear_modes(const char *sender, Channel *chan);
static void clear_bans(const char *sender, Channel *chan, User *u);
static void clear_umodes(const char *sender, Channel *chan, int32 modes);
static void clear_users(const char *sender, Channel *chan, const char *reason);

void clear_channel(Channel *chan, int what, const void *param)
{
    const char *sender =
        *clear_channel_sender ? clear_channel_sender : ServerName;

    if (call_callback_4(cb_clear_channel, sender, chan, what, param) > 0) {
        set_cmode(NULL, chan);
        return;
    }

    if (what & CLEAR_USERS) {
        clear_users(sender, chan, (const char *)param);
        /* Once we kick all the users, nothing else will matter */
        return;
    }

    if (what & CLEAR_MODES)
        clear_modes(sender, chan);
    if (what & CLEAR_BANS)
        clear_bans(sender, chan, (User *)param);
    if (what & CLEAR_UMODES)
        clear_umodes(sender, chan, (int32)(long)param);
    set_cmode(NULL, chan);      /* Flush modes out */
}

static void clear_modes(const char *sender, Channel *chan)
{
    char buf[BUFSIZE];
    snprintf(buf, sizeof(buf), "-%s",
             mode_flags_to_string(chan->mode & ~chanmode_reg, MODE_CHANNEL));
    set_cmode(sender, chan, buf, chan->key);
}

static void clear_bans(const char *sender, Channel *chan, User *u)
{
    int i, count;
    char **bans;

    if (!chan->bans_count)
        return;

    /* Save original ban info */
    count = chan->bans_count;
    bans = smalloc(sizeof(char *) * count);
    memcpy(bans, chan->bans, sizeof(char *) * count);

    for (i = 0; i < count; i++) {
        if (!u || match_usermask(bans[i], u))
            set_cmode(sender, chan, "-b", bans[i]);
    }
    free(bans);
}

static void clear_umodes(const char *sender, Channel *chan, int32 modes)
{
    struct c_userlist *cu;

    LIST_FOREACH (cu, chan->users) {
        int32 to_clear = cu->mode & modes;  /* modes we need to clear */
        int32 flag = 1;                     /* mode we're clearing now */
        while (to_clear) {
            if (flag == MODE_INVALID) {
                log("BUG: hit invalid flag in clear_umodes!"
                    "  modes to clear = %08X, user modes = %08X",
                    to_clear, cu->mode);
                break;
            }
            if (to_clear & flag) {
                char buf[3] = "-x";
                buf[1] = mode_flag_to_char(flag, MODE_CHANUSER);
                set_cmode(sender, chan, buf, cu->user->nick);
                to_clear &= ~flag;
            }
            flag <<= 1;
        }
        cu->mode &= ~modes;
    }
}

static void clear_users(const char *sender, Channel *chan, const char *reason)
{
    char *av[3];
    struct c_userlist *cu, *next;

    /* Prevent anyone from coming back in.  The ban will disappear
     * once everyone's gone. */
    set_cmode(sender, chan, "+b", "*!*@*");
    set_cmode(NULL, chan);      /* Flush modes out */

    av[0] = chan->name;
    av[2] = (char *)reason;
    LIST_FOREACH_SAFE (cu, chan->users, next) {
        av[1] = cu->user->nick;
        send_channel_cmd(sender, "KICK %s %s :%s",
                         av[0], av[1], av[2]);
        do_kick(sender, 3, av);
    }
}

/*************************************************************************/

/* Set the nickname to be used to send commands in clear_channel() calls.
 * If NULL, the server name is used; if PTR_INVALID, the name is not
 * changed.  Returns the old value of the sender, or the empty string if no
 * nickname was set, in a static buffer.
 */

const char *set_clear_channel_sender(const char *newsender)
{
    static char oldsender[NICKMAX];
    strbcpy(oldsender, clear_channel_sender);
    if (newsender != PTR_INVALID) {
        if (newsender) {
            strbcpy(clear_channel_sender, newsender);
        } else {
            *clear_channel_sender = 0;
        }
    }
    return oldsender;
}

/*************************************************************************/

/* Remove a user from the IRC network.  `source' is the nick which should
 * generate the kill, or NULL for a server-generated kill.
 */

void kill_user(const char *source, const char *user, const char *reason)
{
    char *av[2];
    char buf[BUFSIZE];

    if (!user || !*user)
        return;
    if (!source || !*source)
        source = ServerName;
    if (!reason)
        reason = "";
    snprintf(buf, sizeof(buf), "%s (%s)", source, reason);
    av[0] = (char *)user;
    av[1] = buf;
    send_cmd(source, "KILL %s :%s", user, av[1]);
    do_kill(source, 2, av);
}

/*************************************************************************/

/* Set the topic on a channel.  `setter' must not be NULL.  `source' is the
 * nick to use to send the TOPIC message; if NULL, the server name is used.
 */

void set_topic(const char *source, Channel *c, const char *topic,
               const char *setter, time_t t)
{
    if (!source)
        source = ServerName;
    call_callback_5(cb_set_topic, source, c, topic, setter, t);
    free(c->topic);
    if (topic && *topic)
        c->topic = sstrdup(topic);
    else
        c->topic = NULL;
    strbcpy(c->topic_setter, setter);
    if (call_callback_5(cb_set_topic, source, c, NULL, NULL, t) > 0)
        return;
}

/*************************************************************************/
/*************************************************************************/

/* set_cmode(): Set modes for a channel and send those modes to remote
 * servers.  Using this routine eliminates the necessity to modify the
 * internal Channel structure and send the command out separately, and also
 * allows the modes for a channel to be collected up over several calls and
 * sent out in a single command, decreasing network traffic (and scroll).
 * This function should be called as either:
 *      set_cmode(sender, channel, modes, param1, param2...)
 * to send one or more channel modes, or
 *      set_cmode(NULL, channel)
 * to flush buffered modes for a channel (if `channel' is NULL, flushes
 * buffered modes for all channels).
 *
 * NOTE: When setting modes with parameters, all parameters MUST be
 *       strings.  Numeric parameters must be converted to strings (with
 *       snprintf() or the like) before being passed.
 */

#define MAXMODES 6  /* Maximum number of mode parameters */

#define MAXPARAMSLEN \
    (510- NICKMAX -    6   - CHANMAX  -(3+31+2*MAXMODES)-MAXMODES)
/*       |:sender| | MODE | |#channel|    | -...+...|    { param}*
 * Note that "-...+..." can contain at most 31 (binary) plus 2*MAXMODES
 * (MAXMODES modes with parameters plus +/-) characters. */

static struct modedata {
    time_t used;
    Channel *channel;
    char sender[NICKMAX];
    int32 binmodes_on;
    int32 binmodes_off;
    char opmodes[MAXMODES*2+1];
    char params[MAXMODES][MAXPARAMSLEN+1];
    int nopmodes, nparams, paramslen;
    Timeout *timeout;   /* For timely flushing */
} modedata[MERGE_CHANMODES_MAX];

static void possibly_remove_mode(struct modedata *md, char mode,
                                 const char *user);
static void add_mode_with_params(struct modedata *md, char mode, int is_add,
                                 int params, const char *parambuf, int len);
static void flush_cmode(struct modedata *md);
static void flush_cmode_callback(Timeout *t);

/*************************************************************************/

void set_cmode(const char *sender, Channel *channel, ...)
{
    va_list args;
    const char *modes, *modes_orig;
    struct modedata *md;
    int which = -1, add;
    int i;
    char c;


    /* If `sender' is NULL, flush out pending modes for the channel (for
     * all channels if `channel' is also NULL) and return. */
    if (!sender) {
        for (i = 0; i < MERGE_CHANMODES_MAX; i++) {
            if (modedata[i].used && (!channel || modedata[i].channel==channel))
                flush_cmode(&modedata[i]);
        }
        return;
    }

    /* Get the mode string from the argument list; save the original value
     * for error messages. */
    va_start(args, channel);
    modes = modes_orig = va_arg(args, const char *);

    /* See if we already have pending modes for the channel; if so, reuse
     * that entry (if the entry is for a different sender, flush out the
     * pending modes first). */
    for (i = 0; i < MERGE_CHANMODES_MAX; i++) {
        if (modedata[i].used != 0 && modedata[i].channel == channel) {
            if (irc_stricmp(modedata[i].sender, sender) != 0)
                flush_cmode(&modedata[i]);
            which = i;
            break;
        }
    }
    /* If there are no pending modes for the channel, look for an empty
     * slot in the array. */
    if (which < 0) {
        for (i = 0; i < MERGE_CHANMODES_MAX; i++) {
            if (modedata[i].used == 0) {
                which = i;
                break;
            }
        }
    }
    /* If no slots are free, we'll have to purge one.  Find the oldest,
     * send its modes out, then clear and reuse it. */
    if (which < 0) {
        int oldest = 0;
        time_t oldest_time = modedata[0].used;
        for (i = 1; i < MERGE_CHANMODES_MAX; i++) {
            if (modedata[i].used < oldest_time) {
                oldest_time = modedata[i].used;
                oldest = i;
            }
        }
        flush_cmode(&modedata[oldest]);
        which = oldest;
    }

    /* Save a pointer to the entry, then set up sender and channel. */
    md = &modedata[which];
    strbcpy(md->sender, sender);
    md->channel = channel;

    /* Loop through and process all modes in the mode string. */
    add = -2;  /* -2 means we haven't warned about a missing leading +/- yet */
    while ((c = *modes++) != 0) {
        int32 flag;
        int params, is_chanuser;

        log_debug(2, "set_cmode(%s,%s): char=%c(%02X)",
                  sender, channel->name, c<0x20||c>0x7E ? '.' : c, c);

        /* + and - are handled specially. */
        if (c == '+') {
            add = 1;
            continue;
        } else if (c == '-') {
            add = 0;
            continue;
        }
        /* If we see any other character without first seeing a + or -,
         * note a bug in the logfile and move along. */
        if (add < 0) {
            if (add == -2) {
                log("set_cmode(): BUG: mode string `%s' needs leading +/-",
                    modes_orig);
                add = -1;
            }
            continue;
        }

        /* Find the flag value and parameter count for the character. */
        is_chanuser = 0;
        flag = mode_char_to_flag(c, MODE_CHANNEL);
        params = mode_char_to_params(c, MODE_CHANNEL);
        if (!flag) {
            is_chanuser = 1;
            flag = mode_char_to_flag(c, MODE_CHANUSER);
            params = mode_char_to_params(c, MODE_CHANUSER);
            if (!flag) {
                log("set_cmode: bad mode '%c'", c);
                continue;
            }
        }
        params = (params >> (add*8)) & 0xFF;

        if (params) {  /* Mode with parameters */
            char parambuf[BUFSIZE];  /* for putting the parameters in */
            int len = 0;

            if (params > MAXMODES) {
                /* Sanity check */
                fatal("set_cmode(): too many parameters (%d) for mode `%c'\n",
                      params, c);
            }
            /* Merge all the parameters into a single string (with no
             * leading whitespace) */
            for (i = 0; i < params; i++) {
                const char *s = va_arg(args, const char *);
                log_debug(2, "set_cmode(%s,%s):    param=%s",
                          sender, channel->name, s);
                len += snprintf(parambuf+len,  sizeof(parambuf)-len,
                                "%s%s", len ? " " : "", s);
            }
            if (flag != MODE_INVALID) {
                /* If it's a binary mode, see if we've set this mode before.
                 * If so (and if the nick is the same for channel user
                 * modes), remove it; the new one will be appended
                 * afterwards.  Note that this assumes that setting each
                 * mode is independent, i.e. that -a+ba 2 1 has the same
                 * effect as +ba 2 1 by itself when +a is set.  This doesn't
                 * work for +k/-k, so we let multiple such modes remain. */
                if (c != 'k') {
                    possibly_remove_mode(md, c, is_chanuser ? parambuf : NULL);
                }
            }
            add_mode_with_params(md, c, add, params, parambuf, len);
        } else {  /* Binary mode */
            /* Note that `flag' should already be set to this value, since
             * all channel user modes take parameters and thus will never
             * get here, but just in case... */
            flag = mode_char_to_flag(c, MODE_CHANNEL);
            if (add) {
                md->binmodes_on  |=  flag;
                md->binmodes_off &= ~flag;
            } else {
                md->binmodes_off |=  flag;
                md->binmodes_on  &= ~flag;
            }
        }
    }
    va_end(args);
    md->used = time(NULL);

    if (MergeChannelModes) {
        if (!md->timeout) {
            md->timeout = add_timeout_ms(MergeChannelModes,
                                         flush_cmode_callback, 0);
            md->timeout->data = md;
        }
    }
}

/*************************************************************************/

/* Remove the most recent occurrence of mode `mode' from the mode list if
 * there is one, provided either `user' is NULL or the parameter associated
 * with the previous mode is equal (according to irc_stricmp()) to the
 * string pointed to by `user'.
 */

static void possibly_remove_mode(struct modedata *md, char mode,
                                 const char *user)
{
    int i;
    char *s;

    log_debug(2, "possibly_remove_mode %c from %.*s%s%s",
              mode, md->nopmodes*2, md->opmodes,
              user ? " for user " : "", user ? user : "");
    for (i = md->nopmodes-1; i >= 0; i--) {
        if (md->opmodes[i*2+1] == mode) {
            /* We've already set this mode once */
            if (user) {
                /* Only remove the old mode if the nick matches */
                if (irc_stricmp(md->params[i], user) != 0)
                    continue;
            }
            /* Remove the mode */
            log_debug(2, "   removing mode %d/%d", i, md->nopmodes);
            md->nopmodes--;
            s = md->opmodes + (i*2);
            memmove(s, s+2, strlen(s+2)+1);
            /* Count parameters for this mode and decrement total by count */
            md->nparams--;
            s = md->params[i]-1;
            while ((s = strchr(s+1, ' ')) != NULL)
                md->nparams--;
            /* Move parameter pointers */
            if (i < md->nopmodes) {
                memmove(md->params+i, md->params+i+1,
                        sizeof(md->params[0])*(md->nopmodes-i));
            }
            /* Clear tail slot */
            memset(md->params+md->nopmodes, 0, sizeof(md->params[0]));
        }
    }
}

/*************************************************************************/

/* Add a single mode with parameters to the given mode data structure.
 * `params' is the number of parameters, `parambuf' is the space-separated
 * parameter list, and `len' is strlen(parambuf).
 */

static void add_mode_with_params(struct modedata *md, char mode, int is_add,
                                 int params, const char *parambuf, int len)
{
    char *s;

    if (len < 0) {
        log("add_mode_with_params(): BUG: parameter length < 0 (%d)", len);
        len = 0;
    }
    log_debug(2, "add_mode_with_params: current=%.*s mode=%c add=%d"
              " params=%d[%.*s]", md->nopmodes*2, md->opmodes, mode, is_add,
              params, len, parambuf);
            
    /* Check for overflow of parameter count or length */
    if (md->nparams+params > MAXMODES
     || md->paramslen+1+len > MAXPARAMSLEN
    ) {
        /* Doesn't fit, so flush modes out first */
        struct modedata mdtmp = *md;
        log_debug(2, "add_mode_with_params: ...flushing first");
        flush_cmode(md);
        memcpy(md->sender, mdtmp.sender, sizeof(md->sender));
        md->channel = mdtmp.channel;
        md->used = time(NULL);
    }
    s = md->opmodes + 2*md->nopmodes;
    *s++ = is_add ? '+' : '-';
    *s++ = mode;
    if (len > sizeof(md->params[0])-1) {
        log("set_cmode(): Parameter string for mode %c%c is too long,"
            " truncating to %d characters",
            is_add ? '+' : '-', mode, sizeof(md->params[0])-1);
        len = sizeof(md->params[0])-1;
    }
    if (len > 0)
        memcpy(md->params[md->nopmodes], parambuf, len);
    md->params[md->nopmodes][len] = 0;
    md->nopmodes++;
    md->nparams += params;
    if (md->paramslen)
        md->paramslen++;
    md->paramslen += len;
    /* If the parameters for this mode alone exceed MAXPARAMSLEN,
     * we'll now have a string longer than MAXPARAMSLEN in
     * md->params; not much we can do about it, though, and it'll
     * get flushed next time around anyway. */
}

/*************************************************************************/

/* Flush out pending mode changes for the given mode data structure. If
 * `clear' is nonzero, clear the entry, else leave it alone.
 */

static void flush_cmode(struct modedata *md)
{
    char buf[BUFSIZE], *s;
    char *argv[MAXMODES+2];
    int len = 0, i;
    char lastc = 0;

    /* Clear timeout for this entry if one is set */
    if (md->timeout) {
        del_timeout(md->timeout);
        md->timeout = NULL;
    }

    if (!md->channel) {
        /* This entry is unused, just return */
        goto done;
    }
    if (!md->binmodes_on && !md->binmodes_off && !*md->opmodes) {
        /* No actual modes here */
        goto done;
    }

    if (debug >= 2) {
        char onbuf[512];
        strbcpy(onbuf, mode_flags_to_string(md->binmodes_on,MODE_CHANNEL));
        log_debug(2, "flush_cmode(%s): bin_on=%s bin_off=%s opmodes=%d(%.*s)",
                  md->channel->name, onbuf,
                  mode_flags_to_string(md->binmodes_off, MODE_CHANNEL),
                  md->nopmodes, md->nopmodes*2, md->opmodes);
    }

    /* Note that - must come before + because some servers (Unreal, others?)
     * ignore +s if followed by -p. */
    if (md->binmodes_off) {
        len += snprintf(buf+len, sizeof(buf)-len, "-%s",
                        mode_flags_to_string(md->binmodes_off, MODE_CHANNEL));
        lastc = '-';
    }
    if (md->binmodes_on) {
        len += snprintf(buf+len, sizeof(buf)-len, "+%s",
                        mode_flags_to_string(md->binmodes_on, MODE_CHANNEL));
        lastc = '+';
    }
    s = md->opmodes;
    while (*s) {
        if (*s == lastc) {
            /* +/- matches last mode change */
            s++;
        } else {
            if (len < sizeof(buf)-1)
                buf[len++] = *s;
            else
                fatal("BUG: buf too small in flush_cmode() (1)");
            lastc = *s++;
        }
        if (len < sizeof(buf)-1) {
            buf[len++] = *s;
            buf[len] = 0;
        } else {
            fatal("BUG: buf too small in flush_cmode() (2)");
        }
        s++;
    }
    for (i = 0; i < md->nopmodes; i++) {
        if (*md->params[i])
            len += snprintf(buf+len, sizeof(buf)-len, " %s", md->params[i]);
    }

    /* Actually send the command */
    send_cmode_cmd(md->sender, md->channel->name, "%s", buf);

    /* Split buffer back up into individual parameters for do_cmode().
     * This is inefficient, but taking the faster route of setting modes
     * when they are sent to set_cmode() runs the risk of temporary desyncs.
     * (Example: SomeNick enters #channel -> autoop, but delayed -> SomeNick
     *  does /cs op SomeNick -> ChanServ says "SomeNick is already opped" ->
     *  SomeNick goes "Huh?")
     */
    argv[0] = md->channel->name;
    s = buf;
    for (i = 0; i <= md->nparams; i++) {
        argv[i+1] = s;
        s = strchr(s, ' ');
        if (!s) {
            md->nparams = i;
            break;
        }
        *s++ = 0;
    }
    /* Clear md->channel so a recursive set_cmode() doesn't find this entry
     * and try to use/flush it */
    md->channel = NULL;
    /* Adjust our idea of the channel modes */
    do_cmode(md->sender, md->nparams+2, argv);

  done:
    /* Clear entry and return */
    memset(md, 0, sizeof(*md));
}

/*************************************************************************/

/* Timeout called to flush mode changes for a channel after
 * `MergeChannelModes' seconds of inactivity.
 */

static void flush_cmode_callback(Timeout *t)
{
    flush_cmode((struct modedata *)t->data);
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
