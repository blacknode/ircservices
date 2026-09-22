/* solid-ircd protocol module for IRC Services.
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
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"

#define BAHAMUT_HACK  /* For SJOIN; see comments in sjoin.c */
#include "banexcept.c"
#include "halfop.c"
#include "invitemask.c"
#include "sjoin.c"
#include "svsnick.c"

/*************************************************************************/

static Module *module_operserv;
static Module *module_chanserv;

static char *NetworkDomain = NULL;

static int32 usermode_secure = 0;       /* +z */
static int32 chanmode_secure_only = 0;  /* +S */

#define MI_SECURE       0x01000000
#define MI_SECURE_ONLY  0x02000000

/*************************************************************************/
/***************** Local interface to external routines ******************/
/*************************************************************************/

static typeof(is_services_admin) *p_is_services_admin = NULL;

static int local_is_services_admin(User *u)
{
    return p_is_services_admin && (*p_is_services_admin)(u);
}
#define is_services_admin local_is_services_admin

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
    {'a', {0x00000040}},        /* Services admin */
    {'A', {0x00000080}},        /* Server admin */
    {'z', {0x00000100,0,0,0,MI_SECURE}},
                                /* Secure connection */
};

static const struct modedata_init new_chanmodes[] = {
    {'R', {0x00000100,0,0,0,MI_REGNICKS_ONLY}},
                                /* Only identified users can join */
    {'r', {0x00000200,0,0,0,MI_REGISTERED}},
                                /* Set for all registered channels */
    {'c', {0x00000400,0,0}},    /* No ANSI colors in channel */
    {'O', {0x00000800,0,0,0,MI_OPERS_ONLY}},
                                /* Only opers can join channel */
    {'S', {0x00002000,0,0,0,MI_SECURE_ONLY}},
                                /* Only secure (+z) users allowed */
    {'N', {0x00080000,0,0}},    /* No nick changing in channel */
    {'M', {0x02000000,0,0}},    /* Moderated to unregged nicks */
    {'j', {0x04000000,1,0}},    /* /join rate limit */
    {'e', {0x80000000,1,1,0,MI_MULTIPLE}},
                                /* Ban exceptions */
    {'I', {0x80000000,1,1,0,MI_MULTIPLE}},
                                /* INVITE hosts */
};

static const struct modedata_init new_chanusermodes[] = {
};

static void init_modes(void)
{
    int i;

    for (i = 0; i < lenof(new_usermodes); i++) {
        usermodes[new_usermodes[i].mode] = new_usermodes[i].data;
        if (new_usermodes[i].data.info & MI_SECURE)
            usermode_secure |= new_usermodes[i].data.flag;
    }
    for (i = 0; i < lenof(new_chanmodes); i++) {
        chanmodes[new_chanmodes[i].mode] = new_chanmodes[i].data;
        if (new_chanmodes[i].data.info & MI_SECURE_ONLY)
            chanmode_secure_only |= new_chanmodes[i].data.flag;
    }
    for (i = 0; i < lenof(new_chanusermodes); i++)
        chanusermodes[new_chanusermodes[i].mode] = new_chanusermodes[i].data;

    mode_setup();
};

/*************************************************************************/
/************************* IRC message receiving *************************/
/*************************************************************************/

static void m_nick(char *source, int ac, char **av)
{
    char ipbuf[16], *s;
    uint32 ip;

    if (*source) {
        /* Old user changing nicks. */
        if (ac != 2) {
            module_log_debug(1, "NICK message: wrong number of parameters"
                             " (%d) for source `%s'", ac, source);
        } else {
            do_nick(source, ac, av);
        }
        return;
    }

    /* New user. */

    if (ac != 10) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }

    /* Move modes to the end, like do_nick() wants */
    s = av[3];
    memmove(&av[3], &av[4], sizeof(char *) * (ac-4));
    av[9] = s;

    /* Convert IP address from 32 bit integer to string format */
    ip = strtoul(av[7], &s, 10);
    if (*s) {
        wallops(NULL,
                "\2WARNING\2: invalid IP address `%s' for new nick %s",
                av[7], av[0]);
        module_log("WARNING: invalid IP address `%s' for new nick %s",
                   av[7], av[0]);
        s = NULL;
    } else if (!ip && find_module("operserv/sline")) {
        static int warned_no_nickip = 0;
        if (!warned_no_nickip) {
            wallops(NULL, "\2WARNING\2: missing IP address for new nick %s",
                    av[0]);
            warned_no_nickip = 1;
        }
        module_log("WARNING: missing IP address for new nick %s", av[0]);
        s = strcpy(ipbuf, "0.0.0.0");
    } else {
        uint8 rawip[4];
        rawip[0] = ip>>24;
        rawip[1] = ip>>16;
        rawip[2] = ip>>8;
        rawip[3] = ip;
        s = unpack_ip(rawip);
        if (!s || strlen(s) > sizeof(ipbuf)-1) {
            /* super duper paranoia */
            module_log("WARNING: unpack_ip() returned overlong or null"
                       " string: %s", s ? s : "(null)");
            s = NULL;
        } else {
            strcpy(ipbuf, s);  /* safe: we checked length above */
            s = ipbuf;
        }
    }

    /* Rearrange parameters for do_nick() (IP address is in `s') */
    av[7] = av[6];
    av[6] = av[8];
    av[8] = s;

    do_nick(source, ac, av);
}

/*************************************************************************/

static void m_capab(char *source, int ac, char **av)
{
    int i;

    for (i = 0; i < ac; i++) {
        if (stricmp(av[i], "NOQUIT") == 0)
            protocol_features |= PF_NOQUIT;
    }
}

/*************************************************************************/

static void m_sjoin(char *source, int ac, char **av)
{
    if (ac == 3 || ac < 2) {
        module_log_debug(1, "SJOIN: expected 2 or >=4 params, got %d", ac);
        return;
    }
    do_sjoin(source, ac, av);
}

/*************************************************************************/

/* SVSMODE message handler. */

static void m_svsmode(char *source, int ac, char **av)
{
    if (*av[0] == '#') {
        if (ac >= 3 && strcmp(av[1],"-b") == 0) {
            Channel *c = get_channel(av[0]);
            User *u = get_user(av[2]);
            if (c && u)
                clear_channel(c, CLEAR_BANS, u);
        } else {
            module_log("Invalid SVSMODE from %s for channel %s: %s",
                       source, av[0], merge_args(ac-1,av+1));
        }
    } else if (*av[0] == '&') {
        module_log("SVSMODE from %s for invalid target (channel %s): %s",
                   source, av[0], merge_args(ac-1,av+1));
    } else {
        if (ac < 2)
            return;
        if (ac >= 3 && (*av[2] == '+' || *av[2] == '-')) {
            /* Move the timestamp to the end */
            char *ts = av[1];
            memmove(av+1, av+2, sizeof(char *) * (ac-2));
            av[ac-1] = ts;
        }
        do_umode(source, ac, av);
    }
}

/*************************************************************************/

/* SGLINE/SQLINE message handlers.  These remove any SGLINE/SQLINE received
 * which does not exist in Services' databases, to avoid servers which were
 * split during an UNSGLINE/UNSQLINE re-propogating the mask across the
 * network.  See m_tkl() in unreal.c for details.
 */

static void do_sgqline(char *source, int ac, char **av, int type)
{
    typeof(get_maskdata) *p_get_maskdata = NULL;
    typeof(put_maskdata) *p_put_maskdata = NULL;
    char *mask;

    if (ac != 2)
        return;
    if (type == MD_SGLINE) {
        int masklen = strtol(av[0], NULL, 10);
        mask = av[1];
        if (masklen <= 0)
            return;
        mask[masklen] = 0;
    } else {  /* MD_SQLINE */
        mask = av[0];
    }
    if (!(p_get_maskdata = get_module_symbol(NULL, "get_maskdata")))
        return;
    if (!(p_put_maskdata = get_module_symbol(NULL, "put_maskdata")))
        return;
    if ((*p_put_maskdata)((*p_get_maskdata)(type, mask)))
        return;
    send_cmd(ServerName, "UNS%cLINE :%s", type==MD_SGLINE ? 'G' : 'Q', mask);
}

static void m_sgline(char *source, int ac, char **av) {
    do_sgqline(source, ac, av, MD_SGLINE);
}

static void m_sqline(char *source, int ac, char **av) {
    do_sgqline(source, ac, av, MD_SQLINE);
}

/*************************************************************************/

/* Abbreviated message handlers.  See bahamut.c for why we don't allow
 * SERVICESHUB. */

static void no_serviceshub(char *source, int ac, char **av)
{
    fatal("IRC Services will not function correctly when solid-ircd is"
          " configured as a SERVICESHUB.  Please reconfigure solid-ircd as"
          " a regular hub and restart Services.");
}

#define m_ns no_serviceshub
#define m_cs no_serviceshub
#define m_ms no_serviceshub
#define m_rs no_serviceshub
#define m_os no_serviceshub
#define m_ss no_serviceshub
#define m_hs no_serviceshub


/*************************************************************************/

/* SQUIT handler, for remote SQUITs.  This has to be implemented as a
 * "receive message" callback because we can't layer message functions on
 * top of each other. */

static void do_bahamut_squit(char *source, int ac, char **av)
{
    Server *server;

    if (ac < 1)  /* also checked in do_receive_message, but for completeness */
        return;
    server = get_server(av[0]);
    if (!server) {
        send_cmd(ServerName, "402 %s %s :No such server", source, av[0]);
    } else if (!server->fake || server == get_server(ServerName)) {
        /* We should never see an SQUIT for ourselves, since the remote
         * server will disconnect us instead, but just in case... */
        send_cmd(ServerName, "402 %s %s :Not a juped server", source, av[0]);
    } else {
        do_squit(source, ac, av);
        send_cmd(NULL, "SQUIT %s :%s", av[0], av[1] ? av[1] : "");
    }
}

static int do_receive_message(char *source, char *cmd, int ac, char **av)
{
    if (irc_stricmp(cmd,"SQUIT") != 0)
        return 0;
    if (!source || !*source || ac < 1 || irc_stricmp(source,av[0]) == 0)
        return 0;
    do_bahamut_squit(source, ac, av);
    return 1;
}

/*************************************************************************/

static Message bahamut_messages[] = {
    { "AKILL",     NULL },
    { "CAPAB",     m_capab },
    { "CS",        m_cs },
    { "GLOBOPS",   NULL },
    { "GNOTICE",   NULL },
    { "GOPER",     NULL },
    { "HS",        m_hs },
    { "LOCKLUSERS",NULL },
    { "MS",        m_ms },
    { "NICK",      m_nick },
    { "NS",        m_ns },
    { "OS",        m_os },
    { "RAKILL",    NULL },
    { "RS",        m_rs },
    { "SGLINE",    m_sgline },
    { "SILENCE",   NULL },
    { "SJOIN",     m_sjoin },
    { "SQLINE",    m_sqline },
    { "SS",        m_ss },
    { "SVINFO",    NULL },
    { "SVSMODE",   m_svsmode },
    { "UNSGLINE",  NULL },
    { "UNSQLINE",  NULL },
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
    /* NICK <nick> <hops> <TS> <umode> <user> <host> <server> <svsid> <ip>
     *      :<ircname> */
    send_cmd(NULL, "NICK %s 1 %ld +%s %s %s %s 0 0 :%s", nick,
             (long)time(NULL), modes, user, host, server, name);
}

/*************************************************************************/

/* Send a NICK command to change an existing user's nick. */

static void do_send_nickchange(const char *nick, const char *newnick)
{
    send_cmd(nick, "NICK %s %ld", newnick, (long)time(NULL));
}

/*************************************************************************/

/* Send a command to change a user's "real name". */

static void do_send_namechange(const char *nick, const char *newname)
{
    /* Not supported by this protocol. */
}

/*************************************************************************/

/* Send a SERVER command, and anything else needed at the beginning of the
 * connection.
 */

static void do_send_server(void)
{
    send_cmd(NULL, "PASS %s :TS", RemotePassword);
    send_cmd(NULL, "CAPAB TS3 SSJOIN NICKIP NOQUIT TSMODE UNCONNECT");
    send_cmd(NULL, "SERVER %s 1 :%s", ServerName, ServerDesc);
    send_cmd(NULL, "SVINFO 3 3 0 :%ld", (long)time(NULL));
}

/*************************************************************************/

/* Send a SERVER command for a remote (juped) server. */

static void do_send_server_remote(const char *server, const char *reason)
{
    send_cmd(NULL, "SERVER %s 2 :%s", server, reason);
}

/*************************************************************************/

/* Send a WALLOPS (really a GLOBOPS). */

static void do_wallops(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/*************************************************************************/

/* Send a NOTICE to all users on the network. */

static void do_notice_all(const char *source, const char *fmt, ...)
{
    va_list args;
    char msgbuf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(msgbuf, sizeof(msgbuf), fmt, args);
    va_end(args);
    if (NetworkDomain) {
        send_cmd(source, "NOTICE $*.%s :%s", NetworkDomain, msgbuf);
    } else {
        /* Go through all common top-level domains.  If you have others,
         * add them here. */
        send_cmd(source, "NOTICE $*.com :%s", msgbuf);
        send_cmd(source, "NOTICE $*.net :%s", msgbuf);
        send_cmd(source, "NOTICE $*.org :%s", msgbuf);
        send_cmd(source, "NOTICE $*.edu :%s", msgbuf);
    }
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

static int do_user_servicestamp_change(User *user)
{
    send_cmd(ServerName, "SVSMODE %s +d %u", user->nick, user->servicestamp);
    return 0;
}

/*************************************************************************/

static int do_user_mode(User *user, int modechar, int add, char **av)
{
    switch (modechar) {

      case 'd':
        module_log("MODE tried to change services stamp for %s", user->nick);
        send_cmd(ServerName, "SVSMODE %s +d %u", user->nick,
                 user->servicestamp);
        return 0;

      case 'o':
        if (add) {
            user->mode |= UMODE_o;
            if (add && user_identified(user) && is_services_admin(user))
                send_cmd(ServerName, "SVSMODE %s +a", user->nick);
            user->mode &= ~UMODE_o;
        }
        return 0;

      case 'r':
        if (user_identified(user)) {
            if (!add)
                send_cmd(ServerName, "SVSMODE %s +r", user->nick);
        } else {
            if (add)
                send_cmd(ServerName, "SVSMODE %s -r", user->nick);
        }
        return 1;

      case 'a':
        if (is_oper(user)) {
            if (is_services_admin(user)) {
                if (!add)
                    send_cmd(ServerName, "SVSMODE %s +a", user->nick);
            } else {
                if (add)
                    send_cmd(ServerName, "SVSMODE %s -a", user->nick);
            }
            return 1;
        }

    } /* switch (mode) */

    return 0;
}

/*************************************************************************/

static int do_channel_mode(const char *source, Channel *channel,
                           int modechar, int add, char **av)
{
    int32 flag = mode_char_to_flag(modechar, MODE_CHANNEL);

    switch (modechar) {
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
    } /* switch (modechar) */
    return 0;
}

/*************************************************************************/

static int do_check_kick(User *user, const char *chan, ChannelInfo *ci,
                         char **mask_ret, const char **reason_ret)
{
    Channel *c = get_channel(chan);

    if ((((c?c->mode:0) | (ci?ci->mlock.on:0)) & chanmode_secure_only)
     && !(user->mode & usermode_secure)
    ) {
        *mask_ret = create_mask(user, 1);
        *reason_ret = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        return 1;
    }
    return 0;
}

/*************************************************************************/

static int do_nick_identified(User *u, int old_status)
{
    if (is_oper(u) && is_services_admin(u))
        send_cmd(ServerName, "SVSMODE %s +a", u->nick);
    return 0;
}

/*************************************************************************/

static int do_set_topic(const char *source, Channel *c, const char *topic,
                        const char *setter, time_t t)
{
    if (setter)
        return 0;
    c->topic_time = t;
    send_cmd(source, "TOPIC %s %s %ld :%s", c->name, c->topic_setter,
             (long)c->topic_time, c->topic ? c->topic : "");
    return 1;
}

/*************************************************************************/

static int do_check_modes(Channel *c, ChannelInfo *ci, int add, int32 flag)
{
    if (add) {
        switch (mode_flag_to_char(flag, MODE_CHANNEL)) {
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
        }
    }
    return 0;
}

/*************************************************************************/

static int do_set_mlock(User *u, ChannelInfo *ci, int mode, int add, char **av)
{
    if (!mode) {
        /* Final check of new mode lock--nothing to do */
        return 0;
    }

    /* Single mode set/clear */

    if (add) {

        switch (mode) {
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

    } else {  /* !add -> lock off */

        switch (mode) {
          case 'j':
            ci->mlock.joinrate1 = ci->mlock.joinrate2 = -1;
            break;
        } /* switch (mode) */

    } /* if (add) */

    return 0;
}

/*************************************************************************/

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    time_t now = time(NULL);

    /* Bahamut 1.8.0 has a "feature" which converts an expiration delay of
     * zero (supposedly "unlimited") into a delay of one week.  Therefore,
     * we have a "feature" which sends an unlimited expiration time as
     * many years in the future. */
    time_t length = ((expires && expires > now) ? expires - now : 0);
    if (length == 0 && now < 0x7FFFFFFF)
        length = 0x7FFFFFFF - now;

    send_cmd(ServerName, "AKILL %s %s %ld %s %ld :%s", host, username,
             (long)length, who ? who : "<unknown>", (long)now, reason);
    return 1;
}

/*************************************************************************/

static int do_cancel_akill(const char *username, const char *host)
{
    send_cmd(ServerName, "RAKILL %s %s", host, username);
    return 1;
}

/*************************************************************************/

static int do_send_sgline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    send_cmd(ServerName, "SGLINE %d :%s:%s", (int)strlen(mask), mask, reason);
    return 1;
}

static int do_send_sqline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    send_cmd(ServerName, "SQLINE %s :%s", mask, reason);
    return 1;
}

static int do_send_szline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    return do_send_akill("*", mask, expires, who, reason);
}

/*************************************************************************/

static int do_cancel_sgline(const char *mask)
{
    send_cmd(ServerName, "UNSGLINE :%s", mask);
    return 1;
}

static int do_cancel_sqline(const char *mask)
{
    send_cmd(ServerName, "UNSQLINE %s", mask);
    return 1;
}

static int do_cancel_szline(const char *mask)
{
    return do_cancel_akill("*", mask);
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "NetworkDomain", { { CD_STRING, 0, &NetworkDomain } } },
    SJOIN_CONFIG,
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/main") == 0) {
        module_operserv = mod;
        p_is_services_admin = get_module_symbol(mod, "is_services_admin");
        if (!p_is_services_admin) {
            module_log("warning: unable to look up symbol `is_services_admin'"
                       " in module `operserv/main'");
        }
    } else if (strcmp(modname, "operserv/akill") == 0) {
        if (!add_callback(mod, "send_akill", do_send_akill))
            module_log("Unable to add send_akill callback");
        if (!add_callback(mod, "cancel_akill", do_cancel_akill))
            module_log("Unable to add cancel_akill callback");
    } else if (strcmp(modname, "operserv/sline") == 0) {
        if (!add_callback(mod, "send_sgline", do_send_sgline))
            module_log("Unable to add send_sgline callback");
        if (!add_callback(mod, "send_sqline", do_send_sqline))
            module_log("Unable to add send_sqline callback");
        if (!add_callback(mod, "send_szline", do_send_szline))
            module_log("Unable to add send_szline callback");
        if (!add_callback(mod, "cancel_sgline", do_cancel_sgline))
            module_log("Unable to add cancel_sgline callback");
        if (!add_callback(mod, "cancel_sqline", do_cancel_sqline))
            module_log("Unable to add cancel_sqline callback");
        if (!add_callback(mod, "cancel_szline", do_cancel_szline))
            module_log("Unable to add cancel_szline callback");
    } else if (strcmp(modname, "nickserv/main") == 0) {
        if (!add_callback(mod, "identified", do_nick_identified))
            module_log("Unable to add NickServ identified callback");
    } else if (strcmp(modname, "chanserv/main") == 0) {
        module_chanserv = mod;
        p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (!p_s_ChanServ)
            p_s_ChanServ = &ServerName;
        if (!add_callback(mod, "check_modes", do_check_modes))
            module_log("Unable to add ChanServ check_modes callback");
        if (!add_callback(mod, "check_kick", do_check_kick))
            module_log("Unable to add ChanServ check_kick callback");
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
        p_is_services_admin = NULL;
    } else if (mod == module_chanserv) {
        module_chanserv = NULL;
        p_s_ChanServ = &ServerName;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    unsigned char c;


    protocol_name     = "Bahamut";
    protocol_version  = "1.8.0+";
    protocol_features = PF_SZLINE | PF_MODETS_FIRST;
    protocol_nickmax  = 30;

    if (!register_messages(bahamut_messages)) {
        module_log("Unable to register messages");
        exit_module(1);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "receive message", do_receive_message)
     || !add_callback(NULL, "user servicestamp change",
                      do_user_servicestamp_change)
     || !add_callback(NULL, "channel MODE", do_channel_mode)
     || !add_callback(NULL, "user MODE", do_user_mode)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        exit_module(1);
        return 0;
    }

    if (!init_banexcept()
     || !init_invitemask()
     || !init_sjoin()
     || !init_svsnick("SVSNICK")
    ) {
        exit_module(1);
        return 0;
    }

    init_modes();

    irc_lowertable['['] = '[';
    irc_lowertable['\\'] = '\\';
    irc_lowertable[']'] = ']';
    for (c = 0; c < 32; c++)
        valid_chan_table[c] = 0;
    valid_chan_table[160] = 0;

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
    pseudoclient_oper  = 0;

    mapstring(OPER_BOUNCY_MODES, OPER_BOUNCY_MODES_U_LINE);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    if (!shutdown) {
        /* Do not allow removal */
        return 0;
    }

    exit_svsnick();
    exit_sjoin();
    exit_invitemask();
    exit_banexcept();
    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "user MODE", do_user_mode);
    remove_callback(NULL, "channel MODE", do_channel_mode);
    remove_callback(NULL, "user servicestamp change",
                    do_user_servicestamp_change);
    remove_callback(NULL, "receive message", do_receive_message);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);
    unregister_messages(bahamut_messages);
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
