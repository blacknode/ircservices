/* Dreamforge protocol module for IRC Services.
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
#include "modules/nickserv/nickserv.h"

#include "svsnick.c"

/*************************************************************************/

static Module *module_operserv;

static char *NetworkDomain = NULL;

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

static const struct modedata_init new_usermodes[] = {
    {'g', {0x00000008}},        /* Receive globops */
    {'h', {0x00000010}},        /* Helpop */
    {'r', {0x00000020,0,0,0,MI_REGISTERED}},
                                /* Registered nick */
    {'a', {0x00000040}},        /* Services admin */
    {'A', {0x00000080}},        /* Server admin */
};

static const struct modedata_init new_chanmodes[] = {
    {'R', {0x00000100,0,0,0,MI_REGNICKS_ONLY}},
                                /* Only identified users can join */
    {'r', {0x00000200,0,0,0,MI_REGISTERED}},
                                /* Set for all registered channels */
};

static const struct modedata_init new_chanusermodes[] = {
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
/************************* IRC message receiving *************************/
/*************************************************************************/

static void m_nick(char *source, int ac, char **av)
{
    char *tmp;

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

    if (ac != 8) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }

    /* Swap parameters so the Services timestamp is last. */
    tmp = av[6];
    av[6] = av[7];
    av[7] = tmp;

    do_nick(source, ac, av);
}

/*************************************************************************/

static void m_svsmode(char *source, int ac, char **av)
{
    if (*av[0] == '#' || *av[0] == '&') {
        module_log("SVSMODE from %s for invalid target (channel %s): %s",
                   source, av[0], merge_args(ac-1,av+1));
    } else {
        if (ac < 2)
            return;
        do_umode(source, ac, av);
    }
}

/*************************************************************************/

static Message dreamforge_messages[] = {
    { "AKILL",     NULL },
    { "GLOBOPS",   NULL },
    { "GNOTICE",   NULL },
    { "GOPER",     NULL },
    { "NICK",      m_nick },
    { "PROTOCTL",  NULL },
    { "RAKILL",    NULL },
    { "SILENCE",   NULL },
    { "SQLINE",    NULL },
    { "SVSMODE",   m_svsmode },
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
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 :%s", nick, (long)time(NULL),
             user, host, server, name);
    if (modes)
        send_cmd(nick, "MODE %s +%s", nick, modes);
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
    send_cmd(NULL, "PASS :%s", RemotePassword);
    send_cmd(NULL, "SERVER %s 1 :%s", ServerName, ServerDesc);
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
    if (c->topic_time && t >= c->topic_time)
        t = c->topic_time - 1;  /* Force topic change */
    c->topic_time = t;
    send_cmd(source, "TOPIC %s %s %ld :%s", c->name, c->topic_setter,
             (long)c->topic_time, c->topic ? c->topic : "");
    return 1;
}

/*************************************************************************/

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    send_cmd(ServerName, "AKILL %s %s :%s", host, username, reason);
    return 1;
}

/*************************************************************************/

static int do_cancel_akill(const char *username, const char *host)
{
    send_cmd(ServerName, "RAKILL %s %s", host, username);
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "NetworkDomain", { { CD_STRING, 0, &NetworkDomain } } },
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
    } else if (strcmp(modname, "nickserv/main") == 0) {
        if (!add_callback(mod, "identified", do_nick_identified))
            module_log("Unable to add NickServ identified callback");
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


    protocol_name     = "Dreamforge/DALnet";
    protocol_version  = "4.4.15+";
    protocol_features = 0;
    protocol_nickmax  = 30;

    if (!register_messages(dreamforge_messages)) {
        module_log("Unable to register messages");
        exit_module(1);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "user servicestamp change",
                      do_user_servicestamp_change)
     || !add_callback(NULL, "user MODE", do_user_mode)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        exit_module(1);
        return 0;
    }

    if (!init_svsnick("SVSNICK")) {
        exit_module(1);
        return 0;
    }

    init_modes();

    irc_lowertable['['] = '[';
    irc_lowertable['\\'] = '\\';
    irc_lowertable[']'] = ']';
    for (c = 0; c < 32; c++)
        valid_chan_table[c] = 0;
    valid_chan_table['+'] = 3;
    valid_chan_table[':'] = 0;
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
    remove_callback(NULL, "load module", do_load_module);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "user MODE", do_user_mode);
    remove_callback(NULL, "user servicestamp change",
                    do_user_servicestamp_change);
    remove_callback(NULL, "set topic", do_set_topic);
    unregister_messages(dreamforge_messages);
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
