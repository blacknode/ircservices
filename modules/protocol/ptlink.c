/* PTlink protocol module for IRC Services.
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
#include "version.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"

#include "banexcept.c"
#include "sjoin.c"
#include "svsnick.c"

/*************************************************************************/

/* "Nick" to use to indicate Services-set GLINEs */
#define GLINE_WHO       "<ircservices>"


static Module *module_operserv;

static char *NetworkDomain = NULL;

static int32 usermode_admin = 0;        /* +aANT */
static int32 usermode_hiding = 0;       /* +S */
static int32 chanmode_admins_only = 0;  /* +A */

/*************************************************************************/
/***************** Local interface to external routines ******************/
/*************************************************************************/

static typeof(is_services_admin) *p_is_services_admin = NULL;

static int local_is_services_admin(User *u)
{
    return p_is_services_admin && (*p_is_services_admin)(u);
}
#define is_services_admin local_is_services_admin

/*************************************************************************/
/************************** User/channel modes ***************************/
/*************************************************************************/

struct modedata_init {
    uint8 mode;
    ModeData data;
};

#define MI_ADMIN        0x01000000  /* Usermode given to admins */
#define MI_HIDING       0x02000000  /* Usermode for hiding users */
#define MI_ADMINS_ONLY  0x01000000  /* Chanmode for admin-only channels */

static const struct modedata_init new_usermodes[] = {
    {'g', {0x00000008}},        /* Receive globops */
    {'h', {0x00000010}},        /* Helpop */
    {'r', {0x00000020,0,0,0,MI_REGISTERED}},
                                /* Registered nick */
    {'a', {0x00000040}},        /* Services admin */
    {'A', {0x00000080}},        /* Server admin */
    {'N', {0x00000100,0,0,0,MI_ADMIN}},
                                /* Network admin */
    {'T', {0x00000200,0,0,0,MI_ADMIN}},
                                /* Technical admin */
    /* Flags in this range are used by Unreal */
    {'S', {0x00002000,0,0,0,MI_HIDING}},
                                /* Stealth mode (hides joins etc) */
    {'B', {0x00004000}},        /* Is a bot ("deaf" in Unreal) */
    {'R', {0x00008000}},        /* Allow PRIVMSGs from +r clients only */
    {'p', {0x00010000}},        /* Private (don't show channels in /whois) */
    {'v', {0x00020000}},        /* Don't allow DCCs */
};

static const struct modedata_init new_chanmodes[] = {
    {'R', {0x00000100,0,0,0,MI_REGNICKS_ONLY}},
                                /* Only identified users can join */
    {'r', {0x00000200,0,0,0,MI_REGISTERED}},
                                /* Set for all registered channels */
    {'c', {0x00000400,0,0}},    /* No ANSI colors in channel */
    {'O', {0x00000800,0,0,0,MI_OPERS_ONLY}},
                                /* Only opers can join channel */
    {'A', {0x00001000,0,0,0,MI_OPERS_ONLY|MI_ADMINS_ONLY}},
                                /* Only admins can join channel */
    {'c', {0x00100000,0,0}},    /* Strip colors */
    {'d', {0x00800000,0,0}},    /* No flooding */
    {'S', {0x04000000,0,0}},    /* No spam */
    {'q', {0x08000000,0,0}},    /* No quits */
    {'K', {0x10000000,0,0}},    /* Send knock message on failed join */
    {'e', {0x80000000,1,1,0,MI_MULTIPLE}},
                                /* Ban exceptions */
};

static const struct modedata_init new_chanusermodes[] = {
    {'a', {0x00000010,1,1,'.'}},        /* Channel owner */
};

static void init_modes(void)
{
    int i;

    for (i = 0; i < lenof(new_usermodes); i++) {
        usermodes[new_usermodes[i].mode] = new_usermodes[i].data;
        if (new_usermodes[i].data.info & MI_ADMIN)
            usermode_admin |= new_usermodes[i].data.flag;
        if (new_usermodes[i].data.info & MI_HIDING)
            usermode_hiding |= new_usermodes[i].data.flag;
    }
    for (i = 0; i < lenof(new_chanmodes); i++) {
        chanmodes[new_chanmodes[i].mode] = new_chanmodes[i].data;
        if (new_chanmodes[i].data.info & MI_ADMINS_ONLY)
            chanmode_admins_only |= new_chanmodes[i].data.flag;
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
    char *newav[11];

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
    if (ac != 9) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }
    newav[ 0] = av[0];  /* Nick */
    newav[ 1] = av[1];  /* Hop count */
    newav[ 2] = av[2];  /* Timestamp */
    newav[ 3] = av[4];  /* Username */
    newav[ 4] = av[5];  /* Hostname */
    newav[ 5] = av[7];  /* Server */
    newav[ 6] = av[8];  /* Real name */
    newav[ 7] = NULL;   /* Services stamp */
    newav[ 8] = NULL;   /* IP address */
    newav[ 9] = av[3];  /* Modes */
    newav[10] = av[6];  /* User area (fake hostname) */
    do_nick(source, 11, newav);
}

/*************************************************************************/

static void m_capab(char *source, int ac, char **av)
{
    char *s;
    int got_PTS4 = 0;
    int got_QS   = 0;
    int got_EX   = 0;

    if (ac < 1) {
        module_log("received CAPAB with no parameters--broken ircd?");
    } else {
        for (s = strtok(av[0]," "); s; s = strtok(NULL," ")) {
            if (stricmp(s, "PTS4") == 0)
                got_PTS4 = 1;
            else if (stricmp(s, "QS") == 0)
                got_QS = 1;
            else if (stricmp(s, "EX") == 0)
                got_EX = 1;
        }
    }
    if (!got_PTS4 || !got_QS || !got_EX) {
        module_log("CAPAB: capabilities missing:%s%s%s",
                   got_PTS4 ? "" : " PTS4",
                   got_QS   ? "" : " QS",
                   got_EX   ? "" : " EX");
        send_error("Need PTS4/QS/EX capabilities");
        strbcpy(quitmsg, "Remote server doesn't support all of PTS4/QS/EX");
        quitting = 1;
    }
}

/*************************************************************************/

static void m_svinfo(char *source, int ac, char **av)
{
    if (ac < 2) {
        module_log("received SVINFO with <2 parameters--broken ircd?");
        send_error("Invalid SVINFO received (at least 2 parameters needed)");
        quitting = 1;
    } else {
        if (atoi(av[1]) > 6 || atoi(av[0]) < 6) {
            send_error("Need protocol version 6 support");
            strbcpy(quitmsg,
                    "Remote server doesn't support protocol version 6");
            quitting = 1;
        }
    }
}

/*************************************************************************/

static void m_sjoin(char *source, int ac, char **av)
{
    if (ac < 4) {
        module_log("SJOIN: expected >=4 params, got %d (broken ircd?)", ac);
        return;
    }
    do_sjoin(source, ac, av);
}

/*************************************************************************/

static void m_newmask(char *source, int ac, char **av)
{
    char *newuser, *newhost;
    User *u;

    if (ac < 1) {
        module_log("NEWUSER: parameters missing--broken ircd?");
        return;
    }
    if (!(u = get_user(source))) {
        module_log("got NEWUSER from nonexistent user %s", source);
        return;
    }
    newuser = av[0];
    newhost = strchr(newuser, '@');
    if (newhost)
        *newhost++ = 0;
    else
        newhost = "";
    free(u->username);
    u->username = sstrdup(newuser);
    free(u->host);
    u->host = sstrdup(newhost);
}

/*************************************************************************/

/* GLINE/SGLINE/SQLINE handling: cancel any Services-set lines from remote
 * servers (see comments in unreal.c/m_tkl() for details).
 */

static void m_gline(char *source, int ac, char **av)
{
    typeof(get_maskdata) *p_get_maskdata = NULL;
    typeof(put_maskdata) *p_put_maskdata = NULL;

    if (ac < 3 || strcmp(av[2], GLINE_WHO) != 0)
        return;
    if (!(p_get_maskdata = get_module_symbol(NULL, "get_maskdata")))
        return;
    if (!(p_put_maskdata = get_module_symbol(NULL, "put_maskdata")))
        return;
    if ((*p_put_maskdata)((*p_get_maskdata)(MD_AKILL, av[0])))
        return;
    send_cmd(ServerName, "UNGLINE :%s", av[0]);
}

static void m_sgline(char *source, int ac, char **av)
{
    typeof(get_maskdata) *p_get_maskdata = NULL;
    typeof(put_maskdata) *p_put_maskdata = NULL;
    int masklen;

    if (ac < 3)
        return;
    masklen = atoi(av[1]);
    if (masklen < strlen(av[2]))
        av[2][masklen] = 0;
    if (!(p_get_maskdata = get_module_symbol(NULL, "get_maskdata")))
        return;
    if (!(p_put_maskdata = get_module_symbol(NULL, "put_maskdata")))
        return;
    if ((*p_put_maskdata)((*p_get_maskdata)(MD_SGLINE, av[2])))
        return;
    send_cmd(ServerName, "UNSGLINE :%s", av[2]);
}

static void m_sqline(char *source, int ac, char **av)
{
    typeof(get_maskdata) *p_get_maskdata = NULL;
    typeof(put_maskdata) *p_put_maskdata = NULL;

    if (ac < 1)
        return;
    if (!(p_get_maskdata = get_module_symbol(NULL, "get_maskdata")))
        return;
    if (!(p_put_maskdata = get_module_symbol(NULL, "put_maskdata")))
        return;
    if ((*p_put_maskdata)((*p_get_maskdata)(MD_SQLINE, av[0])))
        return;
    send_cmd(ServerName, "UNSQLINE :%s", av[0]);
}

/*************************************************************************/

static Message ptlink_messages[] = {
    { "CAPAB",     m_capab },
    { "GLINE",     m_gline },
    { "GLOBOPS",   NULL },
    { "GNOTICE",   NULL },
    { "GOPER",     NULL },
    { "NEWMASK",   m_newmask },
    { "NICK",      m_nick },
    { "SILENCE",   NULL },
    { "SJOIN",     m_sjoin },
    { "SGLINE",    m_sgline },
    { "SQLINE",    m_sqline },
    { "SVINFO",    m_svinfo },
    { "SVLINE",    NULL },
    { "UNGLINE",   NULL },
    { "UNSGLINE",  NULL },
    { "UNSQLINE",  NULL },
    { "UNSVLINE",  NULL },
    { "UNZOMBIE",  NULL },
    { "ZOMBIE",    NULL },
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
    /* NICK <nick> <hops> <TS> <umode> <user> <host> <fakehost> <server>
     *      :<ircname> */
    send_cmd(NULL, "NICK %s 1 %ld +%s %s %s %s %s 0 :%s", nick,
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
    Module *mod;
    int32 maxusercnt = 0;

    send_cmd(NULL, "PASS %s :TS", RemotePassword);
    send_cmd(NULL, "CAPAB :PTS4 QS EX");
    send_cmd(NULL, "SERVER %s 1 ircservices-%s :%s", ServerName,
             version_number, ServerDesc);
    send_cmd(NULL, "SVINFO 6 6");
    if ((mod = find_module("operserv/main")) != NULL) {
        typeof(get_operserv_data) *p_get_operserv_data;
        p_get_operserv_data = get_module_symbol(mod, "get_operserv_data");
        if (p_get_operserv_data)
            p_get_operserv_data(OSDATA_MAXUSERCNT, &maxusercnt);
    }
    send_cmd(NULL, "SVSINFO %ld %d", (long)start_time, maxusercnt);
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
    vsend_cmd(ServerName, fmt, args);
    va_end(args);
}

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

static int do_user_create(User *user, int ac, char **av)
{
    user->fakehost = sstrdup(av[9]);
    return 0;
}

/*************************************************************************/

static int do_user_mode(User *user, int modechar, int add, char **av)
{
    switch (modechar) {
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

static int do_check_chan_user_modes(const char *source, User *user,
                                    Channel *c, int32 modes)
{
    /* Don't do anything to hiding users */
    return ((user->mode & usermode_hiding) ? 1 : 0);
}

/*************************************************************************/

static int do_check_kick(User *user, const char *chan, ChannelInfo *ci,
                         char **mask_ret, const char **reason_ret)
{
    Channel *c = get_channel(chan);

    /* Don't let plain opers into +A (admin only) channels */
    if ((((c?c->mode:0) | (ci?ci->mlock.on:0)) & chanmode_admins_only)
     && !(user->mode & usermode_admin)
    ) {
        *mask_ret = create_mask(user, 1);
        *reason_ret = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        return 1;
    }
    return 0;
}

/*************************************************************************/

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    time_t now = time(NULL);

    send_cmd(ServerName, "GLINE %s@%s %ld %s :%s", username, host,
             (long)((expires && expires > now) ? expires - now : 0),
             GLINE_WHO, reason);
    return 1;
}

/*************************************************************************/

static int do_cancel_akill(const char *username, const char *host)
{
    send_cmd(ServerName, "UNGLINE %s@%s", username, host);
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
        if (!add_callback(mod, "cancel_sgline", do_cancel_sgline))
            module_log("Unable to add cancel_sgline callback");
        if (!add_callback(mod, "cancel_sqline", do_cancel_sqline))
            module_log("Unable to add cancel_sqline callback");
    } else if (strcmp(modname, "nickserv/main") == 0) {
        if (!add_callback(mod, "identified", do_nick_identified))
            module_log("Unable to add NickServ identified callback");
    } else if (strcmp(modname, "chanserv/main") == 0) {
        if (!add_callback(mod, "check_chan_user_modes",
                          do_check_chan_user_modes))
            module_log("Unable to add ChanServ check_chan_user_modes"
                       " callback");
        if (!add_callback(mod, "check_kick", do_check_kick))
            module_log("Unable to add ChanServ check_kick callback");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_operserv) {
        module_operserv = NULL;
        p_is_services_admin = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    unsigned char c;


    protocol_name     = "PTlink";
    protocol_version  = "6.x";
    protocol_features = PF_BANEXCEPT | PF_NOQUIT;
    protocol_nickmax  = 20;

    if (!register_messages(ptlink_messages)) {
        module_log("Unable to register messages");
        exit_module(1);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "user MODE", do_user_mode)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        exit_module(1);
        return 0;
    }

    if (!init_banexcept() || !init_sjoin() || !init_svsnick("SVSNICK")) {
        exit_module(1);
        return 0;
    }

    init_modes();

    irc_lowertable['['] = '[';
    irc_lowertable['\\'] = '\\';
    irc_lowertable[']'] = ']';
    /* Note that PTlink extended character sets are not supported */
    valid_nick_table['\\'] = 0;
    for (c = 0; c < 32; c++)
        valid_chan_table[c] = 0;
    valid_chan_table['\\'] = 0;
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
    exit_banexcept();
    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "user MODE", do_user_mode);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);
    unregister_messages(ptlink_messages);
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
