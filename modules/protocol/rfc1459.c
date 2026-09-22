/* RFC1459 protocol module for IRC Services.
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

/*************************************************************************/

static char *NetworkDomain = NULL;

/*************************************************************************/
/************************* IRC message receiving *************************/
/*************************************************************************/

static void m_nick(char *source, int ac, char **av)
{
    if (*source) {
        /* Old user changing nicks. */
        if (ac != 1) {
            module_log_debug(1, "NICK message: wrong number of parameters"
                             " (%d) for source `%s'", ac, source);
        } else {
            do_nick(source, ac, av);
        }
        return;
    }

    /* New user.  We get the information we want from the USER command, so
     * we don't do anything else here. */
}

/*************************************************************************/

static void m_user(char *source, int ac, char **av)
{
    char *new_av[7];

    if (ac != 4)
        return;
    new_av[0] = source;      /* Nickname */
    new_av[1] = (char *)"0"; /* # of hops (was in NICK command... we lose) */
    new_av[2] = (char *)"0"; /* Timestamp */
    new_av[3] = av[0];       /* Username */
    new_av[4] = av[1];       /* Hostname */
    new_av[5] = av[2];       /* Server */
    new_av[6] = av[3];       /* Real name */
    do_nick("", 7, new_av);
}

/*************************************************************************/

static Message rfc1459_messages[] = {
    { "NICK",      m_nick },
    { "USER",      m_user },
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
    send_cmd(NULL, "NICK %s :1", nick);
    send_cmd(nick, "USER %s %s %s :%s", user, host, server, name);
    if (modes)
        send_cmd(nick, "MODE %s +%s", nick, modes);
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

/* Send a WALLOPS. */

static void do_wallops(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source ? source : ServerName, "WALLOPS :%s", buf);
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

static int do_set_topic(const char *source, Channel *c, const char *topic,
                        const char *setter, time_t t)
{
    if (setter)
        return 0;
    c->topic_time = t;
    send_cmd(source, "TOPIC %s %s :%s", c->name,
             c->topic_setter, c->topic ? c->topic : "");
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

int init_module(void)
{
    protocol_name     = "RFC1459";
    protocol_version  = "";
    protocol_features = 0;
    protocol_nickmax  = 9;

    if (!register_messages(rfc1459_messages)) {
        module_log("Unable to register messages");
        exit_module(1);
        return 0;
    }

    if (!add_callback(NULL, "set topic", do_set_topic)) {
        module_log("Unable to add callbacks");
        exit_module(1);
        return 0;
    }

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

    remove_callback(NULL, "set topic", do_set_topic);
    unregister_messages(rfc1459_messages);
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
