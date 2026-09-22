/* trircd protocol module for IRC Services.
 * Provided by Yusuf Iskenderoglu <uhc0@stud.uni-karlsruhe.de>
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
#include "modules/chanserv/chanserv.h"
#include "modules/operserv/operserv.h"
#include "modules/nickserv/nickserv.h"

#include "banexcept.c"
#include "chanprot.c"
#include "halfop.c"
#include "invitemask.c"
#include "sjoin.c"
#include "svsnick.c"
#include "token.c"

/*************************************************************************/

static Module *module_operserv;
static Module *module_chanserv;

static char *NetworkDomain = NULL;

/*
 * We import `s_ChanServ' from the "chanserv/main" module; this variable  
 * holds a pointer to `s_ChanServ', obtained via get_module_symbol().  When
 * the ChanServ module is not loaded, this holds a pointer to `ServerName'
 * instead as a default value.
 *
 * Note that we use a #define here to substitute `*p_s_ChanServ' for
 * `s_ChanServ', so subsequent code can simply use `s_ChanServ' as if it
 * were declared normally.
 */

static char **p_s_ChanServ = &ServerName;
#define s_ChanServ (*p_s_ChanServ)


/*************************************************************************/
/***************** Local interface to external routines ******************/
/*************************************************************************/

static typeof(is_services_admin) *p_is_services_admin = NULL;
static int local_is_services_admin(User *u) {
    return p_is_services_admin && (*p_is_services_admin)(u);
}
#define is_services_admin local_is_services_admin

static typeof(get_channelinfo) *p_get_channelinfo = NULL;
static ChannelInfo *local_get_channelinfo(const char *name) {
    return p_get_channelinfo ? (*p_get_channelinfo)(name) : NULL;
}
#define get_channelinfo local_get_channelinfo

static typeof(put_channelinfo) *p_put_channelinfo = NULL;
static ChannelInfo *local_put_channelinfo(ChannelInfo *ci) {
    return p_put_channelinfo ? (*p_put_channelinfo)(ci) : NULL;
}
#define put_channelinfo local_put_channelinfo

static typeof(first_channelinfo) *p_first_channelinfo = NULL;
static ChannelInfo *local_first_channelinfo(void) {
    return p_first_channelinfo ? (*p_first_channelinfo)() : NULL;
}
#define first_channelinfo local_first_channelinfo

static typeof(next_channelinfo) *p_next_channelinfo = NULL;
static ChannelInfo *local_next_channelinfo(void) {
    return p_next_channelinfo ? (*p_next_channelinfo)() : NULL;
}
#define next_channelinfo local_next_channelinfo

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
    {'x', {0x00000100}},        /* Mask hostname */
    {'R', {0x00000200}},        /* Do not receive text from unregged nicks */
    {'L', {0x00000400}},        /* Nick language has been set */
    {'c', {0x00000800}},        /* No color in private messages */
    {'C', {0x00001000}},        /* No CTCPs in private messages */
    {'H', {0x00002000}},        /* User can see realhost, secret channels */
    {'p', {0x00004000}},        /* Hide idle time in /whois */
    {'P', {0x00008000}},        /* Do not receive private messages */
    {'t', {0x00010000}},        /* Greek->Greeklish translation active */
    {'z', {0x00020000}},        /* Dccallow all users */
};

static const struct modedata_init new_chanmodes[] = {
    {'R', {0x00000100,0,0,0,MI_REGNICKS_ONLY}},
                                /* Only identified users can join */
    {'r', {0x00000200,0,0,0,MI_REGISTERED}},
                                /* Set for all registered channels */
    {'c', {0x00000400,0,0}},    /* No ANSI colors in channel */
    {'O', {0x00000800,0,0,0,MI_OPERS_ONLY}},
                                /* Only opers can join channel */
    {'T', {0x00001000,0,0}},    /* /topic only by protected (+a) */
    {'x', {0x00002000,0,0}},    /* Hide ops (show no .@%+ prefixes) */
    {'N', {0x00004000,0,0}},    /* No clients with unresolved hostnames */
    {'d', {0x00008000,0,0}},    /* Hide part/quit reasons */
    {'f', {0x00010000,1,0}},    /* Flood limit */
    {'C', {0x00020000,0,0}},    /* No CTCP in channel */
    {'g', {0x00040000,0,0}},    /* Only registered nicks can talk */
    {'j', {0x00080000,0,0}},    /* /names only by members */
    {'L', {0x00100000,1,0}},    /* Channel link */
    {'K', {0x00200000,0,0}},    /* No knock */
    {'J', {0x00400000,1,0}},    /* /join delay */
    {'I', {0x80000000,1,1,0,MI_MULTIPLE}}, /* INVITE hosts */
    {'e', {0x80000000,1,1,0,MI_MULTIPLE}}, /* ban exceptions */
    {'M', {0x80000000,1,1,0,MI_MULTIPLE}}, /* moderated hosts */
    {'z', {0x80000000,1,1,0,MI_MULTIPLE}}, /* zapped channels */
};

static const struct modedata_init new_chanusermodes[] = {
    {'h', {0x00000004,1,1,'%'}},        /* Half-op */
    {'a', {0x00000008,1,1,'~'}},        /* Protected (no kick or deop by +o) */
    {'u', {0x00000010,1,1,'.'}},        /* Channel owner */
};

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

/* Language hash values (for usermode +L) */

#define HASH_MODULUS    387

static int langhash[NUM_LANGS];
static struct {
    int lang;
    const char *str;
} langhash_init[] = {
    { LANG_EN_US,       "English"       },
    { LANG_JA_EUC,      "Japanese-EUC"  },
    { LANG_JA_SJIS,     "Japanese-SJIS" },
    { LANG_ES,          "Espa\361ol"    },
    { LANG_PT,          "Portugues"     },
    { LANG_FR,          "French"        },
    { LANG_TR,          "Turkce"        },
    { LANG_NL,          "Nederlands"    },
    { LANG_DE,          "Deutsch"       },
    { LANG_IT,          "Italian"       },
    { LANG_HU,          "Magyar"        },
};

static void init_langhash()
{
    int i;

    memset(langhash, 0, sizeof(langhash));
    for (i = 0; i < lenof(langhash_init); i++) {
        int hashval = 0;
        const unsigned char *s = (const unsigned char *)langhash_init[i].str;
        while (*s)
            hashval += *s++ & 0xDF;
        langhash[langhash_init[i].lang] = hashval % HASH_MODULUS;
    }
}

/*************************************************************************/
/************************* IRC message receiving *************************/
/*************************************************************************/

/* Please note that the new CLIENT string of TRIRCD-5 cannot be supported
 * unless ircservices implements identity management of Bahamut 1.6 series.
 * Therefore, the good old NICK line with the IP will be used. -TimeMr14C */

static void m_nick(char *source, int ac, char **av)
{
    char ipbuf[16], *s, *newav[11];
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

    if (ac != 11) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }

    /* Convert IP address from 32 bit integer to string format */
    ip = strtoul(av[9], &s, 10);
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
            wallops(NULL,
                    "\2WARNING\2: missing IP address for new nick %s."
                    "  Make sure you have no pre-4.0 servers on your"
                    " network, or SZLINEs will not work correctly.",
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
    newav[ 0] = av[0];  /* Nick */
    newav[ 1] = av[1];  /* Hop count */
    newav[ 2] = av[2];  /* Timestamp */
    newav[ 3] = av[4];  /* Username */
    newav[ 4] = av[5];  /* Hostname */
    newav[ 5] = av[7];  /* Server */
    newav[ 6] = av[10]; /* Real name */
    newav[ 7] = av[8];  /* Services stamp */
    newav[ 8] = s;      /* IP address */
    newav[ 9] = av[3];  /* Modes */
    newav[10] = av[6];  /* User area (fake hostname) */

    /* Actually add the nick */
    do_nick(source, ac, av);
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

static void m_capab(char *source, int ac, char **av)
{
    int got_trircd5 = 0, got_excap = 0;
    int i;

    for (i = 0; i < ac; i++) {
        if (stricmp(av[i], "TRIRCD5") == 0) {
            got_trircd5 = 1;
        } else if (stricmp(av[i], "EXCAP") == 0) {
            got_excap = 1;
        }
    }
    if (!got_trircd5 || !got_excap) {
        send_error("Only trircd 5.5 and later are supported");
        strbcpy(quitmsg, "Remote server version is not 5.5 or later");
        quitting = 1;
    }
}

static void m_excap(char *source, int ac, char **av)
{
    char *s;

    if (ac < 1)
        return;
    for (s = strtok(av[0]," "); s != NULL; s = strtok(NULL," ")) {
        if (stricmp(s, "CHANLINK") == 0) {
        }
    }
}

/*************************************************************************/

static void m_svinfo(char *source, int ac, char **av)
{
    /* -TimeMr14C
     * Obviously we do need to send an initial PING, regardless of the
     * reality that services may be configured to send pings if there is
     * low traffic. Additionally, to comply with the TRIRCD Extensions, we
     * do set the topic of the services logging channel.
     */  

    send_cmd(NULL, "PING :%s", ServerName);
    send_cmd(NULL, "TOPIC #services DevNull %ld :Services Logging channel",
             (long)time(NULL)); 
}

/*************************************************************************/

static void m_tmode(char *source, int ac, char **av)
{
    if (ac < 3) {
        module_log_debug(1, "TMODE: expected >=3 params, got %d", ac);
        return;
    }
    memmove(av+1, av+2, sizeof(char *) * (ac-1));
    do_cmode(source, ac-1, av);
}

/*************************************************************************/

static Message trircd_messages[] = {
    { "ADMINS",    NULL },
    { "AKILL",     NULL },
    { "CAPAB",     m_capab },
    { "CLIENT",    NULL },
    { "DENYTEXT",  NULL },
    { "EWHOIS",    NULL },
    { "EXCAP",     m_excap },
    { "EXCLUDE",   NULL },
    { "GLINE",     NULL },
    { "GLOBOPS",   NULL },
    { "GNOTICE",   NULL },
    { "GOPER",     NULL },
    { "HASH",      NULL },
    { "JUPITER",   NULL },
    { "KNOCK",     NULL },
    { "MYID",      NULL },
    { "NETHTM",    NULL },
    { "NETSET",    NULL },
    { "NICK",      m_nick },
    { "RAKILL",    NULL },
    { "REXCLUDE",  NULL },
    { "REXCOM",    NULL },
    { "RNOTICE",   NULL },
    { "RPING",     NULL },
    { "RPUNG",     NULL },
    { "SADMINS",   NULL },
    { "SGLINE",    NULL },
    { "SILENCE",   NULL },
    { "SJOIN",     m_sjoin },
    { "SQLINE",    NULL },
    { "SVINFO",    m_svinfo },
    { "SZLINE",    NULL },
    { "TMODE",     m_tmode },
    { "UNDENYTEXT",NULL },
    { "UNGLINE",   NULL },
    { "UNJUPITER", NULL },
    { "UNSGLINE",  NULL },
    { "UNSQLINE",  NULL },
    { "UNSZLINE",  NULL },
    { NULL }
};

static TokenInfo trircd5_tokens[] = {  
    /* 0x21 */
    { "!",  NULL },
    { "\"", NULL },
    { "#",  NULL },
    { "$",  NULL },
    { "%",  NULL },
    { "&",  NULL },   
    { "'",  NULL },
    { "(",  NULL },
    { ")",  NULL },
    { "*",  NULL },
    { "+",  NULL },
    { ",",  "EWHOIS" },
    { "-",  NULL },
    { ".",  NULL },
    { "/",  NULL },

    /* 0x30 */
    { "0",  NULL },
    { "1",  "SVSKILL" },
    { "2",  "SVSMODE" },
    { "3",  "SVSNICK" },
    { "4",  "SVSNOOP" },
    { "5",  "SGLINE" },
    { "6",  "UNSGLINE" },       
    { "7",  "UNSZLINE" },
    { "8",  NULL },
    { "9",  NULL },
    { ":",  NULL },
    { ";",  NULL },
    { "<",  "ADMINS" },
    { "=",  "MYID" },
    { ">",  "SADMINS" },
    { "?",  "STATS" },

    /* 0x40 */
    { "@",  "LUSERS" },
    { "A",  "AWAY" },
    { "B",  "BURST" },
    { "C",  "JUPITER" },
    { "D",  "UNJUPE" }, 
    { "E",  "ERROR" },
    { "F",  "GLINE" },   
    { "G",  "GNOTICE" },
    { "H",  "CLIENT" },
    { "I",  "INVITE" },
    { "J",  "JOIN" },
    { "K",  "KICK" },
    { "L",  "GLOBOPS" },
    { "M",  "MODE" },
    { "N",  "NICK" },
    { "O",  "SQUERY" },

    /* 0x50 */
    { "P",  "PRIVMSG" },
    { "Q",  "QUIT" },
    { "R",  "RAKILL" },
    { "S",  "SJOIN" },
    { "T",  "TOPIC" },
    { "U",  "UNSQLINE" },
    { "V",  "NETSET" },
    { "W",  "WHOIS" },
    { "X",  "SERVICE" },
    { "Y",  "INFO" },
    { "Z",  "ZLINE" },
    { "[",  "RPING" },
    { "\\", "USERS" },
    { "]",  "RPONG" },
    { "^",  NULL },
    { "_",  "DENYTEXT" },

    /* 0x60 */
    { "`",  "UNDENYTEXT" },
    { "a",  "AKILL" },
    { "b",  "SVSJOIN" },
    { "c",  "CONNECT" },
    { "d",  "ADMIN" },
    { "e",  "EXCLUDE" },
    { "f",  "PING" },
    { "g",  "GOPER" },
    { "h",  "SILENCE" },
    { "i",  "TIME" },
    { "j",  "REXCLUDE" },
    { "k",  "KILL" },
    { "l",  "WALLOPS" },
    { "m",  "MOTD" },
    { "n",  "NOTICE" },
    { "o",  NULL },

    /* 0x70 */
    { "p",  "PART" },
    { "q",  "SQUIT" },
    { "r",  "RNOTICE" },
    { "s",  "SERVER" },
    { "t",  "TRACE" },
    { "u",  "PONG" },
    { "v",  "VERSION" },
    { "w",  "WHOWAS" },
    { "x",  "SQLINE" },
    { "y",  "UNGLINE" },
    { "z",  "SVINFO" },
    { "{",  "KNOCK" },
    { "|",  "REXCOM" },
    { "}",  NULL },
    { "~",  "TMODE" },

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
    /* NICK <nick> <hops> <TS> <umode> <user> <host> <server> <svsid>
     *      :<ircname> */
    send_cmd(NULL, "NICK %s 1 %ld +%s %s %s %s %s 0 0 :%s", nick,
             (long)time(NULL), modes, user, host, host, server, name);
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
    send_cmd(NULL, "PASS %s :TS7", RemotePassword);
    /* for 5.7 only: send_cmd(NULL, "CAPAB NICKIP TOKEN1 EXCAP"); */
    send_cmd(NULL,
             "CAPAB TS3 NOQUIT SSJOIN NICKIP DT1 EX-REX TOKEN1 TMODE EXCAP");
    send_cmd(NULL, "EXCAP :CHANLINK");
    send_cmd(NULL, "SERVER %s 1 :%s", ServerName, ServerDesc);
    send_cmd(NULL, "SVINFO 3 3 0 :%ld", (long)time(NULL));
}

/*************************************************************************/

/* Send a SERVER command for a remote (juped) server. */

/* Even with TRIRCD5, services does not need supporting the new SERVER
 * line, where identity and capability parameters are also sent. 
 * 1) Services does not need to know, who is also ULined.
 * 2) Services does not support servername hiding.
 * 3) Services does not do network rerouting.
 * So are both parameters unnecessary and old SERVER line will be used.
 * -TimeMr14C */

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

static int do_connect(void)
{
    /* Send MODEs to the server for all channels locked +L, in case the
     * servers aren't aware of them. */

    ChannelInfo *ci;

    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
        if ((ci->mlock.on & mode_char_to_flag('L',MODE_CHANNEL))
         && ci->mlock.link
        ) {
            send_cmd(s_ChanServ, "MODE %s +L %s", ci->name, ci->mlock.link);
        }
    }
    return 0;
}

/*************************************************************************/

static int do_receive_message(const char *source, const char *cmd, int ac,
                              char **av)
{
    /* Watch for MODE +/-L's for nonexistent channels, and:
     *    (1) reverse them if necessary
     *    (2) don't generate "no such channel" error messages for them
     */
    char *s;
    ChannelInfo *ci;
    int add;
    int modeL; /* status of +/-L wrt this message (1:on, 0:off, -1:not seen) */
    int lockL; /* status of MLOCK +/-L (1:+L, 0:-L) */

    if (stricmp(cmd,"MODE") != 0 || ac < 2 || av[0][0] != '#'
     || get_channel(av[0])  /* existing channels will get handled normally */
     || !(ci = get_channelinfo(av[0]))
    ) {
        return 0;
    }
    if ((ci->mlock.on & mode_char_to_flag('L',MODE_CHANNEL)) && ci->mlock.link)
        lockL = 1;
    else if (ci->mlock.off & mode_char_to_flag('L',MODE_CHANNEL))
        lockL = 0;
    else
        lockL = -1;
    put_channelinfo(ci);
    if (lockL == -1)
        return 0;
    add = -1;
    modeL = -1;
    for (s = av[1]; *s; s++) {
        if (*s == '+') {
            add = 1;
        } else if (*s == '-') {
            add = 0;
        } else if (*s == 'L') {
            if (add < 0) {
                module_log("Invalid MODE message from server: MODE %s",
                           merge_args(ac,av));
                return 0;
            } else {
                modeL = add;
            }
        }
    }
    if (modeL == -1)
        return 0;
    if (modeL != lockL) {
        if (lockL) {
            /* ci->mlock.link checked above */
            send_cmd(s_ChanServ, "MODE %s +L %s", av[0], ci->mlock.link);
        } else {
            send_cmd(s_ChanServ, "MODE %s -L", av[0]);
        }
    }
    return 1;  /* don't let the main code see it and say "no such channel" */
}

/*************************************************************************/

static int do_user_create(User *user, int ac, char **av)
{
    user->fakehost = sstrdup(av[9]);
    return 0;
}

/*************************************************************************/

static int do_user_servicestamp_change(User *user)
{
    send_cmd(ServerName, "SVSMODE %s +d %ld",
             user->nick, (long)user->servicestamp);
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
    }
    return 0;
}

/*************************************************************************/

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
    }
    return 0;
}

/*************************************************************************/

/* We set +L for the user's language here--but only if they haven't already
 * set it themselves. */

static int do_nick_identified(User *u, int old_status)
{
    int mode_L, lang;

    mode_L = u->mode & mode_char_to_flag('L', MODE_USER);
    /* valid_ngi(u) _should_ always be true, but let's be paranoid */
    if (valid_ngi(u) && u->ngi->language != LANG_DEFAULT)
        lang = u->ngi->language;
    else
        lang = DEF_LANGUAGE;
    if (is_oper(u) && is_services_admin(u)) {
        if (!mode_L)
            send_cmd(ServerName, "SVSMODE %s +aL %d", u->nick, langhash[lang]);
        else
            send_cmd(ServerName, "SVSMODE %s +a", u->nick);
    } else {
        if (!mode_L)
            send_cmd(ServerName, "SVSMODE %s +L %d", u->nick, langhash[lang]);
    }
    return 0;
}

/*************************************************************************/

static int do_set_topic(const char *source, Channel *c, const char *topic,
                        const char *setter, time_t t)
{
    if (setter)
        return 0;
    c->topic_time = t;
    send_cmd(source, "TOPIC %s %s %ld :%s", c->name,
             c->topic_setter, (long)c->topic_time, c->topic ? c->topic : "");
    return 1;
}

/*************************************************************************/

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
        }
    } else {  /* remove */
        switch (mode_flag_to_char(flag, MODE_CHANNEL)) {
          case 'L':
            /* Theoretically the channel can't exist, but send a set_cmode()
             * just to be safe */
            set_cmode(s_ChanServ, c, "-L");
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
            /* Legal format for flood mode is "[*]digits:digits" */
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

        } /* switch (mode) */

    } else {  /* !add -> lock off */

        switch (mode) {
          case 'L': {
            /* The channel should not exist here; check anyway just to be
             * safe, and do a set_cmode() or send_cmd() as appropriate. */
            Channel *c = get_channel(ci->name);
            if (c)
                set_cmode(s_ChanServ, c, "-L");
            else
                send_cmd(s_ChanServ, "MODE %s -L", ci->name);
          } /* case 'L' */

          case 'f':
            free(ci->mlock.flood);
            ci->mlock.flood = NULL;
            break;

          case 'J':
            ci->mlock.joindelay = 0;
            break;
        } /* switch (mode) */

    } /* if (add) */

    return 0;
}

/*************************************************************************/

static int do_send_svsjoin(const char *nick, const char *channel)
{
    send_cmd(ServerName, "SVSJOIN %s %s", nick, channel);
    return 1;
}

/*************************************************************************/

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    time_t now = time(NULL);

    send_cmd(ServerName, "AKILL %s %s %ld %s %ld :%s", host, username,
             (long)((expires && expires > now) ? expires - now : 0),
             who ? who : "<unknown>", (long)now, reason);
    return 1;
}

/*************************************************************************/

static int do_cancel_akill(const char *username, const char *host)
{
    send_cmd(ServerName, "RAKILL %s %s", host, username);
    return 1;
}

/*************************************************************************/

static int do_send_exclude(const char *username, const char *host,
                           time_t expires, const char *who, const char *reason)
{
    time_t now = time(NULL);

    send_cmd(ServerName, "EXCLUDE %s %s %ld %s %ld :%s", host, username,
             (long)((expires && expires > now) ? expires - now : 0),
             who ? who : "<unknown>", (long)now, reason);
    return 1;
}

/*************************************************************************/

static int do_cancel_exclude(const char *username, const char *host)
{
    send_cmd(ServerName, "REXCLUDE %s %s", host, username);
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
    send_cmd(ServerName, "SZLINE %s :%s", mask, reason);
    return 1;
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
    send_cmd(ServerName, "UNSZLINE %s", mask);
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
        if (!add_callback(mod, "send_exclude", do_send_exclude))
            module_log("Unable to add send_exclude callback");
        if (!add_callback(mod, "cancel_akill", do_cancel_akill))
            module_log("Unable to add cancel_akill callback");
        if (!add_callback(mod, "cancel_exclude", do_cancel_exclude))
            module_log("Unable to add cancel_exclude callback");
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
    } else if (strcmp(modname, "nickserv/autojoin") == 0) {
        if (!add_callback(mod, "send_svsjoin", do_send_svsjoin))
            module_log("Unable to add NickServ send_svsjoin callback");
    } else if (strcmp(modname, "chanserv/main") == 0) {
        module_chanserv = mod;
        p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (!p_s_ChanServ)
            p_s_ChanServ = &ServerName;
        /* first/next_channelinfo are defined in the database module */
        p_get_channelinfo = get_module_symbol(NULL, "get_channelinfo");
        p_put_channelinfo = get_module_symbol(NULL, "put_channelinfo");
        p_first_channelinfo = get_module_symbol(NULL, "first_channelinfo");
        p_next_channelinfo = get_module_symbol(NULL, "next_channelinfo");
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
        p_is_services_admin = NULL;
    } else if (mod == module_chanserv) {
        module_chanserv = NULL;
        p_s_ChanServ = &ServerName;
        p_first_channelinfo = NULL;
        p_next_channelinfo = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    unsigned char c;


    protocol_name     = "trircd";
    protocol_version  = "5.5+";
    protocol_features = PF_SZLINE | PF_SVSJOIN | PF_AKILL_EXCL | PF_HALFOP
                      | PF_NOQUIT;
    protocol_nickmax  = 30;

    if (!register_messages(trircd_messages)) {
        module_log("Unable to register messages");
        exit_module(1);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "connect", do_connect)
     || !add_callback(NULL, "receive message", do_receive_message)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "channel MODE", do_channel_mode)
     || !add_callback(NULL, "user servicestamp change",
                      do_user_servicestamp_change)
     || !add_callback(NULL, "user MODE", do_user_mode)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        exit_module(1);
        return 0;
    }

    if (!init_banexcept()
     || !init_chanprot()
     || !init_halfop()
     || !init_invitemask()
     || !init_sjoin()
     || !init_svsnick("SVSNICK")
     || !init_token(trircd5_tokens)
    ) {
        exit_module(1);
        return 0;
    }

    init_modes();
    init_langhash();

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

    exit_token();
    exit_svsnick();
    exit_sjoin();
    exit_invitemask();
    exit_halfop();
    exit_chanprot();
    exit_banexcept();

    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "user MODE", do_user_mode);
    remove_callback(NULL, "user servicestamp change",
                    do_user_servicestamp_change);
    remove_callback(NULL, "channel MODE", do_channel_mode);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "receive message", do_receive_message);
    remove_callback(NULL, "connect", do_connect);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);
    unregister_messages(trircd_messages);
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
