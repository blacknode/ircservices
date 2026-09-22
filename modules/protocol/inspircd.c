/* InspIRCd protocol module for IRC Services.
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
#include "messages.h"
#include "timeout.h"
#include "version.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"

#include "banexcept.c"
#include "chanprot.c"
#include "halfop.c"
#include "invitemask.c"
#include "svsnick.c"

/*************************************************************************/

static Module *module_operserv;
static Module *module_chanserv;

/* Flags for remote server capabilities */
static int have_m_services = 0;
static int have_globops = 0;

/*************************************************************************/
/****************** Local interface to external symbols ******************/
/*************************************************************************/

static char **p_s_OperServ = &ServerName;
#define s_OperServ (*p_s_OperServ)

static char **p_s_ChanServ = &ServerName;
#define s_ChanServ (*p_s_ChanServ)

/*************************************************************************/
/************************** User/channel modes ***************************/
/*************************************************************************/

struct modedata_init {
    uint8 mode;
    ModeData data;
};

static const struct modedata_init new_usermodes[] = {
    {'g', {0x00000008}},        /* Receive globops */
    {'h', {0x00000010}},        /* Helpop */
    {'r', {0x00000020,0,0,0,MI_REGISTERED}},
                                /* Registered nick */
    {'x', {0x00000040}},        /* Mask hostname */
};

/* New channel modes: */
static const struct modedata_init new_chanmodes[] = {
    {'R', {0x00000100,0,0,0,MI_REGNICKS_ONLY}},
                                /* Only identified users can join (m_services)*/
    {'r', {0x00000200,0,0,0,MI_REGISTERED}},
                                /* Set for all registered channels */
    {'c', {0x00000400,0,0}},    /* No ANSI colors in channel (m_blockcolor) */
    {'O', {0x00000800,0,0,0,MI_OPERS_ONLY}},
                                /* Only opers can join channel (m_operchans) */
    {'Q', {0x00004000,0,0}},    /* No kicks (m_nokicks) */
    {'K', {0x00008000,0,0}},    /* No knocks (m_knock) */
    {'V', {0x00010000,0,0}},    /* No invites (m_noinvite) */
    {'C', {0x00040000,0,0}},    /* No CTCPs (m_noctcp) */
    {'N', {0x00080000,0,0}},    /* No nick changing (m_nonicks) */
    {'S', {0x00100000,0,0}},    /* Strip colors (m_stripcolor) */
    {'G', {0x00200000,0,0}},    /* Strip "bad words" (m_censor) */
    {'L', {0x01000000,1,0}},    /* Channel link (m_redirect) */
    {'M', {0x02000000,0,0}},    /* Moderated to unregged nicks (m_services) */
    {'T', {0x04000000,0,0}},    /* Disable notices to channel (m_nonotice) */
    {'J', {0x08000000,1,0}},    /* Anti-rejoin protection (m_kicknorejoin) */
    {'f', {0x10000000,1,0}},    /* Anti-flood protection (m_messageflood) */
    {'j', {0x20000000,1,0}},    /* Anti-joinflood protection (m_joinflood) */
    {'P', {0x40000000,0,0}},    /* Anti-CAPS (m_blockcaps) */
    {'e', {0x80000000,1,1,0,MI_MULTIPLE}}, /* Ban exceptions */
    {'I', {0x80000000,1,1,0,MI_MULTIPLE}}, /* INVITE hosts */
    {'g', {0x80000000,1,1,0,MI_MULTIPLE}}, /* Message filter (m_chanfilter) */
};

/* New channel user modes: */
static const struct modedata_init new_chanusermodes[] = {
    {'h', {0x00000004,1,1,'%'}},        /* Half-op */
    {'a', {0x00000008,1,1,'~'}},        /* Protected (no kick or deop by +o) */
    {'q', {0x00000010,1,1,'*'}},        /* Channel owner */
};

/*************************************************************************/

/* Add protocol-specific modes into the global mode table. */

static void init_modes(void)
{
    int i;

    for (i = 0; i < lenof(new_usermodes); i++)
        usermodes[new_usermodes[i].mode] = new_usermodes[i].data;
    for (i = 0; i < lenof(new_chanmodes); i++)
        chanmodes[new_chanmodes[i].mode] = new_chanmodes[i].data;
    for (i = 0; i < lenof(new_chanusermodes); i++)
        chanusermodes[new_chanusermodes[i].mode] = new_chanusermodes[i].data;

    mode_setup();
};

/*************************************************************************/
/************************* IRC message receiving *************************/
/*************************************************************************/

static void m_capab(char *source, int ac, char **av)
{
    char *s;

    if (ac < 1) {
        return;
    } else if (stricmp(av[0], "START") == 0) {
        have_m_services = 0;
        have_globops = 0;
    } else if (stricmp(av[0], "CAPABILITIES") == 0 && ac >= 2) {
        /* Nothing to do at present */
    } else if (stricmp(av[0], "MODULES") == 0 && ac >= 2) {
        for (s = strtok(av[1], ","); s; s = strtok(NULL, ",")) {
            if (strcmp(s, "m_services.so") == 0)
                have_m_services = 1;
            if (strcmp(s, "m_globops.so") == 0)
                have_globops = 1;
        }
    } else if (stricmp(av[0], "END") == 0) {
        if (!have_m_services) {
            send_error("m_services.so is required, but not present");
            strbcpy(quitmsg, "Remote server does not have the required module"
                    " m_services.so loaded.");
        }
    } else {
        module_log_debug(1, "Unknown CAPAB type or missing parameters: %s",
                         av[0]);
    }
}

/*************************************************************************/

static void m_nick(char *source, int ac, char **av)
{
    char *newav[11];

    if (ac == 1) {
        /* Old user changing nicks.  No timestamp for InspIRCd. */
        do_nick(source, ac, av);
        return;
    }

    /* New user.  InspIRCd NICK format is:
     *     :server NICK <timestamp> <nick> <hostname> <fakehost>
     *                  <username> <modes> <IP> :<realname>
     * InspIRCd does away with the hopcount, so we substitute a dummy
     * value (since Services doesn't need it either). */

    if (ac != 8) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }
    if (!pack_ip(av[6])) {
        module_log("NICK message: invalid IP address %s", av[6]);
        av[6] = NULL;
    }

    /* Set up parameter list for do_nick():
     *     (current)     (desired)
     *  0: timestamp     nick
     *  1: nick          hopcount
     *  2: hostname      timestamp
     *  3: fakehost      username
     *  4: username      hostname
     *  5: modes         server
     *  6: ip            realname
     *  7: realname      servicestamp
     *  8: ---           ipaddr
     *  9: ---           modes
     * 10: ---           fakehost
     */
    newav[ 0] = av[1];
    newav[ 1] = "1";
    newav[ 2] = av[0];
    newav[ 3] = av[4];
    newav[ 4] = av[2];
    newav[ 5] = source;
    newav[ 6] = av[7];
    newav[ 7] = NULL;
    newav[ 8] = av[6];
    newav[ 9] = av[5];
    newav[10] = av[3];

    /* Record the new nick */
    do_nick("", 11, newav);
}

/*************************************************************************/

static void m_opertype(char *source, int ac, char **av)
{
    char *newav[2];
    newav[0] = source;
    newav[1] = "+o";
    do_umode(source, 2, newav);
}

/*************************************************************************/

static void m_fhost(char *source, int ac, char **av)
{
    if (ac == 1) {
        User *u = get_user(source);
        if (!u) {
            module_log("m_fhost: user record for %s not found", source);
            return;
        }
        free(u->fakehost);
        u->fakehost = sstrdup(av[0]);
    }
}

static void m_chghost(char *source, int ac, char **av)
{
    if (ac == 2) {
        module_log_debug(1, "m_chghost: calling m_fhost(%s,%s)",
                         av[0], av[1]);
        m_fhost(av[0], ac-1, av+1);
    }
}

/*************************************************************************/

static void m_fname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return;
    u = get_user(source);
    if (!u) {
        module_log("m_fname: user record for %s not found", source);
        return;
    }
    free(u->realname);
    u->realname = sstrdup(merge_args(ac, av));
}

/*************************************************************************/

static void m_sanick(char *source, int ac, char **av)
{
    if (ac == 2)
        do_nick(av[0], 1, &av[1]);
}

static void m_sajoin(char *source, int ac, char **av)
{
    if (ac == 2)
        do_join(av[0], 1, &av[1]);
}

static void m_sapart(char *source, int ac, char **av)
{
    if (ac == 2)
        do_part(av[0], 1, &av[1]);
}

/*************************************************************************/

static void m_fjoin(char *source, int ac, char **av)
{
    Channel *c = NULL;

    if (ac < 3) {
        module_log_debug(1, "FJOIN: expected >=3 params, got %d", ac);
        return;
    }

    const char *param;
    for (param = strtok(av[2], " "); param; param = strtok(NULL, " ")) {
        const char *nick = param;
        User *user;
        Channel *ctemp;
        int32 modes = 0, thismode;

        while (*nick && *nick != ',') {
            thismode = cumode_prefix_to_flag(*nick);
            modes |= thismode;
            nick++;
        }
        if (!*nick) {
            module_log_debug(1, "sjoin: invalid SJOIN nick parameter: %s",
                             param);
            continue;
        }
        nick++;  /* Skip comma */
        user = get_user(nick);
        if (!user) {
            module_log("sjoin: FJOIN to channel %s for non-existent nick %s",
                       av[0], nick);
            continue;
        }
        module_log_debug(1, "%s FJOINs %s", nick, av[0]);
        if ((ctemp = join_channel(user, av[0], modes)) != NULL)
            c = ctemp;
    }

    /* Did anyone actually join the channel?  Set creation time if so */
    if (c)
        c->creation_time = strtotime(av[1], NULL);
}

/*************************************************************************/

static void m_fmode(char *source, int ac, char **av)
{
    if (*av[0] == '#') {
        if (ac >= 3 && strcmp(av[1],"-b") == 0) {
            Channel *c = get_channel(av[0]);
            User *u = get_user(av[2]);
            if (c && u)
                clear_channel(c, CLEAR_BANS, u);
        } else if (ac >= 3) {
            char modestr[BUFSIZE];
            av[1] = av[0];  /* Remove the timestamp parameter */
            if (av[2][0] != '+' && av[2][0] != '-') {
                /* InspIRCd seems to omit the leading '+' when sending
                 * initial modes for the channel */
                snprintf(modestr, sizeof(modestr), "+%s", av[2]);
                av[2] = modestr;
            }
            do_cmode(source, ac-1, av+1);
        }
    } else {
        if (ac < 2)
            return;
        do_umode(source, ac, av);
    }
}

/*************************************************************************/

static void m_inspircd_topic(char *source, int ac, char **av)
{
    if (ac == 2) {
        char *newav[4];
        char timebuf[32];
        newav[0] = av[0];
        newav[1] = source;
        snprintf(timebuf, sizeof(timebuf), "%ld", (long)time(NULL));
        newav[2] = timebuf;
        newav[3] = av[1];
        do_topic(source, 4, newav);
    }
}

static void m_ftopic(char *source, int ac, char **av)
{
    if (ac == 2) {
        char *newav[4];
        newav[0] = av[0];
        newav[1] = av[2];
        newav[2] = av[1];
        newav[3] = av[3];
        do_topic(source, 4, newav);
    }
}

/*************************************************************************/

static void m_idle(char *source, int ac, char **av)
{
    if (ac == 1)
        send_cmd(av[0], "IDLE %s %ld 0", source, (long)time(NULL));
}

/*************************************************************************/

static void m_time(char *source, int ac, char **av)
{
    if (ac == 2) {
        /* Request by nick av[1] for the time on server av[0] */
        send_cmd(ServerName, "TIME %s %s %ld", source, av[1],
                 (long)time(NULL));
    }
}

/*************************************************************************/

static void m_aes(char *source, int ac, char **av)
{
    send_error("AES linking not supported");
    strbcpy(quitmsg, "AES requested by remote server, but not supported.");
    quitting = 1;
}

/*************************************************************************/

/* List of all InspIRCd-specific messages (handlers defined above). */

static Message inspircd_messages[] = {
    { "ADDLINE",   NULL },
    { "AES",       m_aes },
    { "BURST",     NULL },
    { "CAPAB",     m_capab },
    { "CHGHOST",   m_chghost },
    { "ELINE",     NULL },
    { "ENDBURST",  NULL },
    { "FHOST",     m_fhost },
    { "FJOIN",     m_fjoin },
    { "FMODE",     m_fmode },
    { "FNAME",     m_fname },
    { "FTOPIC",    m_ftopic },
    { "GLOBOPS",   NULL },
    { "IDLE",      m_idle },
    { "KLINE",     NULL },
    { "METADATA",  NULL },
    { "NICK",      m_nick },
    { "OPERTYPE",  m_opertype },
    { "QLINE",     NULL },
    { "SAJOIN",    m_sajoin },
    { "SANICK",    m_sanick },
    { "SAMODE",    m_fmode },
    { "SAPART",    m_sapart },
    { "SETNAME",   m_fname },
    { "SILENCE",   NULL },
    { "TIME",      m_time },
    { "TOPIC",     m_inspircd_topic },
    { "WATCH",     NULL },
    { "VERSION",   NULL },
    { "ZLINE",     NULL },
    { NULL }
};

/*************************************************************************/
/************************** IRC message sending **************************/
/*************************************************************************/

/* Send a NICK command for a new user. */

static void do_send_nick(const char *nick, const char *user, const char *host,
                         const char *server, const char *name,
                         const char *modes)
{
    send_cmd(ServerName, "NICK %ld %s %s %s %s +%s 0.0.0.0 :%s",
             (long)time(NULL), nick, host, host, user, modes, name);
    if (strchr(modes, 'o')) {
        send_cmd(nick, "OPERTYPE :Network_Service");
    }
}

/*************************************************************************/

/* Send a NICK command to change an existing user's nick. */

static void do_send_nickchange(const char *nick, const char *newnick)
{
    send_cmd(nick, "NICK %s", newnick);
}

/*************************************************************************/

/* Send a command to change a user's "real name". */

static void do_send_namechange(const char *nick, const char *newname)
{
    send_cmd(nick, "FNAME :%s", newname);
}

/*************************************************************************/

/* Send a SERVER command, and anything else needed at the beginning of the
 * connection.
 */

static void do_send_server(void)
{
    send_cmd(NULL, "SERVER %s %s 0 :%s", ServerName, RemotePassword,
             ServerDesc);
    send_cmd(NULL, "BURST");
    send_cmd(ServerName, "VERSION :ircservices-%s %s :%s",
             version_number, ServerName, version_build);
    send_cmd(NULL, "ENDBURST");
}

/*************************************************************************/

/* Send a SERVER command for a remote (juped) server. */

static void do_send_server_remote(const char *server, const char *reason)
{
    send_cmd(NULL, "SERVER %s * 1 :%s", server, reason);
    send_cmd(server, "VERSION :ircservices-%s %s :%s",
             version_number, server, version_build);
}

/*************************************************************************/

/* Send a WALLOPS (really a GLOBOPS if available). */

static void do_wallops(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
//@@@    send_cmd(source ? source : s_OperServ, "%s :%s",
//             have_globops ? "GLOBOPS" : "WALLOPS", buf);
}

/*************************************************************************/

/* Send a NOTICE to all users on the network. */

static void do_notice_all(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    snprintf(buf, sizeof(buf), "NOTICE $* :%s", fmt);
    vsend_cmd(source, buf, args);
    va_end(args);
}

/*************************************************************************/

/* Send a command which modifies channel status. */

static void do_send_channel_cmd(const char *source, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsend_cmd(source, fmt, args);
    va_end(args);
}

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

/* Callback for new users */

static int do_user_create(User *user, int ac, char **av)
{
    user->fakehost = sstrdup(av[10]);
    return 0;
}

/*************************************************************************/

/* User mode change callback */

static int do_user_mode(User *user, int modechar, int add, char **av)
{
    switch (modechar) {
      case 'r':
        if (user_identified(user)) {
            if (!add)
                send_cmd(ServerName, "SVSMODE %s +r", user->nick);
        } else {
            if (add)
                send_cmd(ServerName, "SVSMODE %s -r", user->nick);
        }
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Channel mode change callback */

static int do_channel_mode(const char *source, Channel *channel,
                           int modechar, int add, char **av)
{
    int32 flag = mode_char_to_flag(modechar, MODE_CHANNEL);

    switch (modechar) {

      case 'L':
        free(channel->link);
        if (add) {
            channel->mode |= flag;
            channel->link = sstrdup(av[0]);
        } else {
            channel->mode &= ~flag;
            channel->link = NULL;
        }
        return 1;

      case 'f':
        free(channel->flood);
        if (add) {
            channel->mode |= flag;
            channel->flood = sstrdup(av[0]);
        } else {
            channel->mode &= ~flag;
            channel->flood = NULL;
        }
        return 1;

      case 'J':
        if (add) {
            char *s;
            channel->mode |= flag;
            channel->joindelay = strtol(av[0], &s, 0);
            if (*s) {
                module_log("warning: invalid MODE +J %s for %s", av[0],
                           channel->name);
                channel->mode &= ~flag;
                channel->joindelay = 0;
            }
        } else {
            channel->mode &= ~flag;
            channel->joindelay = 0;
        }
        return 1;

      case 'j':
        if (add) {
            int ok = 0;
            char *s;
            int joinrate1 = strtol(av[0], &s, 0);
            if (*s == ':') {
                int joinrate2 = strtol(s+1, &s, 0);
                if (!*s) {
                    if (joinrate1 && joinrate2) {
                        channel->mode |= flag;
                        channel->joinrate1 = joinrate1;
                        channel->joinrate2 = joinrate2;
                    } else {
                        channel->mode &= ~flag;
                        channel->joinrate1 = 0;
                        channel->joinrate2 = 0;
                        ok = 1;
                    }
                    ok = 1;
                }
            } else if (joinrate1 == 0) {
                channel->mode &= ~flag;
                channel->joinrate1 = 0;
                channel->joinrate2 = 0;
                ok = 1;
            }
            if (!ok) {
                module_log("warning: invalid MODE +j %s for %s", av[0],
                           channel->name);
            }
        } else {
            channel->mode &= ~flag;
            channel->joinrate1 = 0;
            channel->joinrate2 = 0;
        }
        return 1;

    } /* switch (mode) */

    return 0;
}

/*************************************************************************/

/* Set-topic callback */

static int do_set_topic(const char *source, Channel *c, const char *topic,
                        const char *setter, time_t t)
{
    if (setter)
        return 0;
    if (t <= c->topic_time)
        t = c->topic_time + 1;  /* Force topic change */
    c->topic_time = t;
    send_cmd(source, "FTOPIC %s %ld %s :%s", c->name, (long)c->topic_time,
             c->topic_setter, c->topic ? c->topic : "");
    return 1;
}

/*************************************************************************/

/* ChanServ channel mode check callback */

static int do_check_modes(Channel *c, ChannelInfo *ci, int add, int32 flag)
{
    if (add) {
        switch (mode_flag_to_char(flag, MODE_CHANNEL)) {

          case 'L':
            if (!ci->mlock.link) {
                module_log("warning: removing +L from channel %s mode lock"
                           " (missing parameter)", ci->name);
                ci->mlock.on &= ~mode_char_to_flag('L', MODE_CHANNEL);
            } else {
                if (!c->link || irc_stricmp(ci->mlock.link, c->link) != 0)
                    set_cmode(s_ChanServ, c, "+L", ci->mlock.link);
            }
            return 1;

          case 'f':
            if (!ci->mlock.flood) {
                module_log("warning: removing +f from channel %s mode lock"
                           " (missing parameter)", ci->name);
                ci->mlock.on &= ~mode_char_to_flag('f', MODE_CHANNEL);
            } else {
                if (!c->flood || irc_stricmp(ci->mlock.flood, c->flood) != 0)
                    set_cmode(s_ChanServ, c, "+f", ci->mlock.flood);
            }
            return 1;

          case 'J':
            if (ci->mlock.joindelay <= 0) {
                module_log("warning: removing +J from channel %s mode lock"
                           " (invalid parameter: %d)", ci->name,
                           ci->mlock.joindelay);
                ci->mlock.on &= ~mode_char_to_flag('J', MODE_CHANNEL);
                ci->mlock.joindelay = 0;
            } else {
                if (c->joindelay != ci->mlock.joindelay) {
                    char buf[32];
                    snprintf(buf, sizeof(buf), "%d", ci->mlock.joindelay);
                    set_cmode(s_ChanServ, c, "+J", buf);
                }
            }
            return 1;

          case 'j':
            if (sgn(ci->mlock.joinrate1) != sgn(ci->mlock.joinrate2)) {
                module_log("warning: removing +j from channel %s mode lock"
                           " (invalid parameter: %d:%d)", ci->name,
                           ci->mlock.joinrate1, ci->mlock.joinrate2);
                ci->mlock.on &= ~mode_char_to_flag('j', MODE_CHANNEL);
                ci->mlock.joinrate1 = ci->mlock.joinrate2 = 0;
            } else if (ci->mlock.joinrate1 < 0) {
                if (c->joinrate1 || c->joinrate2)
                    set_cmode(s_ChanServ, c, "-j");
            } else {
                if (c->joinrate1 != ci->mlock.joinrate1
                 || c->joinrate2 != ci->mlock.joinrate2
                ) {
                    char buf[BUFSIZE];
                    snprintf(buf, sizeof(buf), "%d:%d",
                             ci->mlock.joinrate1, ci->mlock.joinrate2);
                    set_cmode(s_ChanServ, c, "+j", buf);
                }
            }
            return 1;

        } /* switch (modechar) */
    } /* if (add) */
    return 0;
}

/*************************************************************************/

/* ChanServ SET MLOCK callback */

static int do_set_mlock(User *u, ChannelInfo *ci, int mode, int add, char **av)
{
    if (!mode) {
        /* Final check of new mode lock */
        if ((ci->mlock.on & mode_char_to_flag('K',MODE_CHANNEL))
         && !(ci->mlock.on & CMODE_i)
        ) {
            /* +K requires +i */
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_REQUIRES, 'K', 'i');
            return 1;
        }
        if (ci->mlock.link && !ci->mlock.limit) {
            /* +L requires +l */
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_REQUIRES, 'L', 'l');
            return 1;
        }
        return 0;
    }

    /* Single mode set/clear */

    if (add) {

        switch (mode) {

          case 'L':
            if (!valid_chan(av[0])) {
                /* Invalid channel name */
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_LINK_BAD, mode);
                return 1;
            }
            if (irc_stricmp(av[0], ci->name) == 0) {
                /* Trying to link to the same channel */
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_LINK_SAME, mode);
                return 1;
            }
            ci->mlock.link = sstrdup(av[0]);
            break;

          case 'f': {
            char *s, *t;
            /* Legal format for flood mode is "digits:digits" or
             * "*digits:digits" */
            s = av[0];
            if (*s == '*')
                s++;
            t = strchr(s, ':');
            if (t) {
                t++;
                if (s[strspn(s,"0123456789")] == ':'
                 && t[strspn(t,"0123456789")] == 0
                ) {
                    /* String is valid */
                    ci->mlock.flood = sstrdup(av[0]);
                    break;
                }
            }
            /* String is invalid */
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_BAD_PARAM, mode);
            return 1;
          } /* case 'f' */

          case 'J': {
            char *s;
            ci->mlock.joindelay = strtol(av[0], &s, 0);
            if (*s || ci->mlock.joindelay <= 0) {
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_NEED_POSITIVE, 'J');
                return 1;
            }
            break;
          } /* case 'J' */

          case 'j': {
            int ok = 0;
            char *s;
            ci->mlock.joinrate1 = strtol(av[0], &s, 0);
            if (ci->mlock.joinrate1 > 0 && *s == ':') {
                ci->mlock.joinrate2 = strtol(s+1, &s, 0);
                if (ci->mlock.joinrate2 > 0 && !*s)
                    ok = 1;
            }
            if (!ok) {
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_BAD_PARAM, mode);
                return 1;
            }
            break;
          } /* case 'j' */

        } /* switch (mode) */

    } /* if (add) */

    return 0;
}

/*************************************************************************/

/* Callback for forcing users into channels (autojoin) */

static int do_send_svsjoin(const char *nick, const char *channel)
{
    send_cmd(ServerName, "SVSJOIN %s %s", nick, channel);
    return 1;
}

/*************************************************************************/

/* Callbacks for sending/removing autokills and S-lines */

static int send_line(char type, const char *mask, const char *who,
                     time_t expires, const char *reason)
{
    send_cmd(ServerName, "ADDLINE %c %s %s %ld %ld :%s",
             type, mask, who, (long)time(NULL),
             expires ? (long)(expires - time(NULL)) : 0L, reason);
    return 1;
}

static int cancel_line(char type, const char *mask)
{
    send_cmd(ServerName, "%cLINE %s", type, mask);
    return 1;
}

/************************************/

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    char maskbuf[BUFSIZE];
    snprintf(maskbuf, sizeof(maskbuf), "%s@%s", username, host);
    return send_line('G', maskbuf, who, expires, reason);
}

static int do_cancel_akill(const char *username, const char *host)
{
    char maskbuf[BUFSIZE];
    snprintf(maskbuf, sizeof(maskbuf), "%s@%s", username, host);
    return cancel_line('G', maskbuf);
}

/************************************/

static int do_send_exclude(const char *username, const char *host,
                           time_t expires, const char *who, const char *reason)
{
    char maskbuf[BUFSIZE];
    snprintf(maskbuf, sizeof(maskbuf), "%s@%s", username, host);
    return send_line('E', maskbuf, who, expires, reason);
}

static int do_cancel_exclude(const char *username, const char *host)
{
    char maskbuf[BUFSIZE];
    snprintf(maskbuf, sizeof(maskbuf), "%s@%s", username, host);
    return cancel_line('E', maskbuf);
}

/************************************/

static int do_send_sgline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    return 0;  /* not supported on InspIRCd */
}

static int do_cancel_sgline(const char *mask)
{
    return 0;  /* not supported on InspIRCd */
}

/************************************/

static int do_send_sqline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    return send_line('Q', mask, who, expires, reason);
}

static int do_cancel_sqline(const char *mask)
{
    return cancel_line('Q', mask);
}

/************************************/

static int do_send_szline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    return send_line('Z', mask, who, expires, reason);
}

static int do_cancel_szline(const char *mask)
{
    return cancel_line('Z', mask);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/main") == 0) {
        module_operserv = mod;
        p_s_OperServ = get_module_symbol(mod, "s_OperServ");
        if (!p_s_OperServ)
            p_s_OperServ = &ServerName;
    } else if (strcmp(modname, "operserv/akill") == 0) {
        if (!add_callback(mod, "send_akill", do_send_akill))
            module_log("Unable to add send_akill callback");
        if (!add_callback(mod, "cancel_akill", do_cancel_akill))
            module_log("Unable to add cancel_akill callback");
        if (!add_callback(mod, "send_exclude", do_send_exclude))
            module_log("Unable to add send_exclude callback");
        if (!add_callback(mod, "cancel_exclude", do_cancel_exclude))
            module_log("Unable to add cancel_exclude callback");
    } else if (strcmp(modname, "operserv/sline") == 0) {
        if (!add_callback(mod, "send_sgline", do_send_sgline))
            module_log("Unable to add send_sgline callback");
        if (!add_callback(mod, "cancel_sgline", do_cancel_sgline))
            module_log("Unable to add cancel_sgline callback");
        if (!add_callback(mod, "send_sqline", do_send_sqline))
            module_log("Unable to add send_sqline callback");
        if (!add_callback(mod, "cancel_sqline", do_cancel_sqline))
            module_log("Unable to add cancel_sqline callback");
        if (!add_callback(mod, "send_szline", do_send_szline))
            module_log("Unable to add send_szline callback");
        if (!add_callback(mod, "cancel_szline", do_cancel_szline))
            module_log("Unable to add cancel_szline callback");
    } else if (strcmp(modname, "nickserv/autojoin") == 0) {
        if (!add_callback(mod, "send_svsjoin", do_send_svsjoin))
            module_log("Unable to add NickServ send_svsjoin callback");
    } else if (strcmp(modname, "chanserv/main") == 0) {
        module_chanserv = mod;
        p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (!p_s_ChanServ)
            p_s_ChanServ = &ServerName;
        if (!add_callback(mod, "check_modes", do_check_modes))
            module_log("Unable to add ChanServ check_modes callback");
        if (!add_callback(mod, "SET MLOCK", do_set_mlock))
            module_log("Unable to add ChanServ SET MLOCK callback");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_operserv) {
        module_operserv = NULL;
        p_s_OperServ = &ServerName;
    } else if (mod == module_chanserv) {
        module_chanserv = NULL;
        p_s_ChanServ = &ServerName;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    protocol_name     = "InspIRCd";
    protocol_version  = "1.1+";
    protocol_features = PF_SZLINE | PF_SVSJOIN;
    protocol_nickmax  = 30;

    if (!register_messages(inspircd_messages)) {
        module_log("Unable to register messages");
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "user MODE", do_user_mode)
     || !add_callback(NULL, "channel MODE", do_channel_mode)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        return 0;
    }

    if (!init_banexcept()
     || !init_chanprot()
     || !init_halfop()
     || !init_invitemask()
     || !init_svsnick("SVSNICK")
    ) {
        return 0;
    }

    have_globops = 0;  /* set by CAPAB handler */
    init_modes();

    /* Set up function pointers and mode strings for send.c. */
    send_nick          = do_send_nick;
    send_nickchange    = do_send_nickchange;
    send_namechange    = do_send_namechange;
    send_server        = do_send_server;
    send_server_remote = do_send_server_remote;
    wallops            = do_wallops;
    notice_all         = do_notice_all;
    send_channel_cmd   = do_send_channel_cmd;
    pseudoclient_modes = "";
    enforcer_modes     = "";
    pseudoclient_oper  = 1;  /* Needed for InspIRCd <=1.1.13 */

    mapstring(OPER_BOUNCY_MODES, OPER_BOUNCY_MODES_U_LINE);

    /* Return success. */
    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    if (!shutdown) {
        /* Do not allow removal of this module */
        return 0;
    }

    exit_svsnick();
    exit_invitemask();
    exit_halfop();
    exit_chanprot();
    exit_banexcept();

    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "channel MODE", do_channel_mode);
    remove_callback(NULL, "user MODE", do_user_mode);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_messages(inspircd_messages);

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
