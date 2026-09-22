/* Hybrid 7 protocol module for IRC Services.
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
#include "messages.h"

#define BAHAMUT_HACK
#include "banexcept.c"
#include "invitemask.c"
#include "sjoin.c"
#include "svsnick.c"

/*************************************************************************/

static Module *module_operserv;

static char **p_s_OperServ = &ServerName;
#define s_OperServ (*p_s_OperServ)

static char *NetworkDomain = NULL;

/*************************************************************************/
/************************** User/channel modes ***************************/
/*************************************************************************/

struct modedata_init {
    uint8 mode;
    ModeData data;
};

static const struct modedata_init new_usermodes[] = {
    {'a', {0x00000008,0,0}},    /* Server admin */
};

static const struct modedata_init new_chanmodes[] = {
    {'a', {0x00000100,0,0}},    /* Hide ops */
    {'e', {0x80000000,1,1,0,MI_MULTIPLE}},
                                /* Ban exceptions */
    {'I', {0x80000000,1,1,0,MI_MULTIPLE}},
            /* Auto-invite masks (Hybrid calls these "invite exceptions") */
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
    char *newav[10];

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

    if (ac != 8) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }
    newav[0] = av[0];   /* Nick */
    newav[1] = av[1];   /* Hop count */
    newav[2] = av[2];   /* Timestamp */
    newav[3] = av[4];   /* Username */
    newav[4] = av[5];   /* Hostname */
    newav[5] = av[6];   /* Server */
    newav[6] = av[7];   /* Real name */
    newav[7] = NULL;    /* Services stamp */
    newav[8] = NULL;    /* IP address */
    newav[9] = av[3];   /* Modes */
    do_nick(source, 10, newav);
}

/*************************************************************************/

static void m_capab(char *source, int ac, char **av)
{
    char *s;
    int has_tburst = 0;

    if (ac != 1)
        return;
    for (s = strtok(av[0]," "); s; s = strtok(NULL," ")) {
        if (stricmp(s, "EX") == 0) {
            protocol_features |= PF_BANEXCEPT;
            init_banexcept();
        } else if (stricmp(s, "IE") == 0) {
            protocol_features |= PF_INVITEMASK;
            init_invitemask();
        } else if (stricmp(s, "QS") == 0) {
            protocol_features |= PF_NOQUIT;
        } else if (stricmp(s, "TBURST") == 0) {
            has_tburst = 1;
        }
    }
    if (!has_tburst) {
        send_error("TBURST support required");
        strbcpy(quitmsg,
                "Remote server does not support TBURST (see the manual)");
        quitting = 1;
    }
}

/*************************************************************************/

static void m_sjoin(char *source, int ac, char **av)
{
    if (ac < 4) {
        module_log_debug(1, "SJOIN: expected >=4 params, got %d", ac);
        return;
    }
    do_sjoin(source, ac, av);
}

/*************************************************************************/

static void m_tburst(char *source, int ac, char **av)
{
    if (ac != 5)
        return;
    av[0] = av[1];
    av[1] = av[3];
    av[3] = av[4];
    do_topic(source, 4, av);
}

/*************************************************************************/

static void m_hybrid_topic(char *source, int ac, char **av)
{
    char *newav[4];
    char timebuf[32];

    if (ac != 2)
        return;
    newav[0] = av[0];
    newav[1] = source;
    snprintf(timebuf, sizeof(timebuf), "%ld", (long)time(NULL));
    newav[2] = timebuf;
    newav[3] = av[1];
    do_topic(source, 4, newav);
}

/*************************************************************************/

static Message hybrid_messages[] = {
    { "CAPAB",     m_capab },
    { "GLINE",     NULL },
    { "NICK",      m_nick },
    { "SJOIN",     m_sjoin },
    { "SVINFO",    NULL },
    { "TBURST",    m_tburst },
    { "TOPIC",     m_hybrid_topic },
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
    /* NICK <nick> <hops> <TS> <umode> <user> <host> <server> :<ircname> */
    send_cmd(NULL, "NICK %s 1 %ld +%s %s %s %s :%s", nick,
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
    send_cmd(NULL, "CAPAB :EX IE KLN UNKLN HUB TBURST");
    send_cmd(NULL, "SERVER %s 1 :%s", ServerName, ServerDesc);
    send_cmd(NULL, "SVINFO 5 3 0 :%ld", (long)time(NULL));
}

/*************************************************************************/

/* Send a SERVER command for a remote (juped) server. */

static void do_send_server_remote(const char *server, const char *reason)
{
    send_cmd(NULL, "SERVER %s 2 :%s", server, reason);
}

/*************************************************************************/

/* Send a WALLOPS. */

static void do_wallops(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source ? source : ServerName, "OPERWALL :%s", buf);
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
        send_cmd(source, "NOTICE $$*.%s :%s", NetworkDomain, msgbuf);
    } else {
        /* Go through all common top-level domains.  If you have others,
         * add them here. */
        send_cmd(source, "NOTICE $$*.com :%s", msgbuf);
        send_cmd(source, "NOTICE $$*.net :%s", msgbuf);
        send_cmd(source, "NOTICE $$*.org :%s", msgbuf);
        send_cmd(source, "NOTICE $$*.edu :%s", msgbuf);
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

static int do_set_topic(const char *source, Channel *c, const char *topic,
                        const char *setter, time_t t)
{
    if (setter)
        return 0;
    c->topic_time = t;
    send_cmd(source, "TBURST %ld %s %ld %s :%s", (long)c->creation_time,
             c->name, (long)c->topic_time, c->topic_setter,
             c->topic ? c->topic : "");
    return 1;
}

/*************************************************************************/

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    if (expires) {
        expires -= time(NULL);
        if (expires < 1)
            expires = 1;
    }
    send_cmd(s_OperServ, "KLINE * %ld %s %s :%s", (long)expires, username,
             host, reason);
    return 1;
}

/*************************************************************************/

static int do_cancel_akill(const char *username, const char *host)
{
    send_cmd(s_OperServ, "UNKLINE * %s %s", username, host);
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
        p_s_OperServ = get_module_symbol(mod, "s_OperServ");
        if (!p_s_OperServ)
            p_s_OperServ = &ServerName;
    } else if (strcmp(modname, "operserv/akill") == 0) {
        if (!add_callback(mod, "send_akill", do_send_akill))
            module_log("Unable to add send_akill callback");
        if (!add_callback(mod, "cancel_akill", do_cancel_akill))
            module_log("Unable to add cancel_akill callback");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_operserv) {
        module_operserv = NULL;
        p_s_OperServ = &ServerName;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    protocol_name     = "Hybrid";
    protocol_version  = "7.0";
    protocol_features = 0;
    protocol_nickmax  = 30;

    if (!register_messages(hybrid_messages)) {
        module_log("Unable to register messages");
        exit_module(1);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        exit_module(1);
        return 0;
    }

    if (!init_sjoin() || !init_svsnick("SVSNICK")) {
        return 0;
    }

    init_modes();

    irc_lowertable['^'] = '~';

    send_nick          = do_send_nick;
    send_nickchange    = do_send_nickchange;
    send_namechange    = do_send_namechange;
    send_server        = do_send_server;
    send_server_remote = do_send_server_remote;
    wallops            = do_wallops;
    notice_all         = do_notice_all;
    send_channel_cmd   = do_send_channel_cmd;
    pseudoclient_modes = "+i";
    enforcer_modes     = "+i";
    pseudoclient_oper  = 1;

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    if (!shutdown) {
        /* Do not allow removal */
        return 0;
    }

    if (protocol_features & PF_INVITEMASK)
        exit_invitemask();
    if (protocol_features & PF_BANEXCEPT)
        exit_banexcept();
    exit_svsnick();
    exit_sjoin();

    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_messages(hybrid_messages);
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
