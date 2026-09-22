/* Routines for sending stuff to the network.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#define IN_SEND_C
#include "services.h"
#include "modules.h"
#include "language.h"

/*************************************************************************/

time_t last_send;       /* Time last data was sent to server */


/* Modes to send for Services users. */
const char *pseudoclient_modes = "";
const char *enforcer_modes = "";
/* Do "oper" pseudoclients really need oper privileges? (1 or 0) */
int pseudoclient_oper = 1;


/* Default handler for module-implemented functions. */
static void unimplemented(void);

/* Functions which are to be implemented by protocol modules.  See
 * documentation for details. */
FUNCPTR(void, send_nick, (const char *nick, const char *user,
                          const char *host, const char *server,
                          const char *name, const char *modes))
     = (void *)unimplemented;
FUNCPTR(void, send_nickchange, (const char *nick, const char *newnick))
     = (void *)unimplemented;
FUNCPTR(void, send_namechange, (const char *nick, const char *newname))
     = (void *)unimplemented;
FUNCPTR(void, send_server, (void))
     = (void *)unimplemented;
FUNCPTR(void, send_server_remote, (const char *server, const char *desc))
     = (void *)unimplemented;
FUNCPTR(void, wallops, (const char *source, const char *fmt, ...)
                        FORMAT(printf,2,3))
     = (void *)unimplemented;
FUNCPTR(void, notice_all, (const char *source, const char *fmt, ...)
                          FORMAT(printf,2,3))
     = (void *)unimplemented;
FUNCPTR(void, send_channel_cmd, (const char *source, const char *fmt, ...)
                                FORMAT(printf,2,3))
     = (void *)unimplemented;
FUNCPTR(void, send_nickchange_remote, (const char *nick, const char *newnick))
     = (void *)unimplemented;

/*************************************************************************/

/* Initialization: set up protocol_* variables, and verify on load that the
 * protocol module set everything up correctly.
 */

const char *protocol_name    = NULL;
const char *protocol_version = NULL;
uint32 protocol_features     = PF_UNSET;
int protocol_nickmax         = 0;

#define PROTOCHK(var) \
    if (!var) \
        fatal("Variable `" #var "' not set by protocol module `%s'", name);
#define FUNCCHK(var) \
    if ((void *)var == (void *)unimplemented) \
        fatal("Function `" #var "' not set by protocol module `%s'", name);

static int do_load_module(Module *mod, const char *name)
{
    /* Assume the first module loaded is a protocol module */

    PROTOCHK(protocol_name);
    if (protocol_features & PF_UNSET)
        fatal("Variable `protocol_features' not set by protocol module `%s'",
              name);
    PROTOCHK(protocol_nickmax);

    FUNCCHK(send_nick);
    FUNCCHK(send_nickchange);
    FUNCCHK(send_namechange);
    FUNCCHK(send_server);
    FUNCCHK(send_server_remote);
    FUNCCHK(wallops);
    FUNCCHK(notice_all);
    FUNCCHK(send_channel_cmd);
    if (protocol_features & PF_CHANGENICK)
        FUNCCHK(send_nickchange_remote);

    /* Make sure NICKMAX is large enough to hold the largest nickname
     * supported by the protocol plus a trailing NULL. */
    if (protocol_nickmax+1 > NICKMAX)
        fatal("NICKMAX is too small (%d)--increase to at least %d and"
              " recompile", NICKMAX, protocol_nickmax+1);

    /* Remove the callback now that everything's checked. */
    remove_callback(NULL, "load module", do_load_module);

    return 0;
}

#undef FUNCCHK
#undef PROTOCHK


int send_init(int ac, char **av)
{
    if (!add_callback(NULL, "load module", do_load_module)) {
        log("send.c: Unable to add load module callback");
        return 0;
    }
    return 1;
}

/*************************************************************************/

/* Cleanup. */

void send_cleanup(void)
{
    remove_callback(NULL, "load module", do_load_module);
}

/*************************************************************************/
/*************************************************************************/

/* Send a command to the server.  The two forms here are like
 * printf()/vprintf() and friends.  If not connected to a remote server,
 * these functions do nothing.
 */

void send_cmd(const char *source, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsend_cmd(source, fmt, args);
    va_end(args);
}

void vsend_cmd(const char *source, const char *fmt, va_list args)
{
    char buf[BUFSIZE];

    if (!servsock)
        return;
    vsnprintf(buf, sizeof(buf), fmt, args);
    if (source) {
        if (servsock)
            sockprintf(servsock, ":%s %s\r\n", source, buf);
        log_debug(1, "Sent: :%s %s", source, buf);
    } else {
        if (servsock)
            sockprintf(servsock, "%s\r\n", buf);
        log_debug(1, "Sent: %s", buf);
    }
    last_send = time(NULL);
}

/*************************************************************************/
/*************************************************************************/

/* Send an ERROR message and close the connection to the server. */

void send_error(const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    snprintf(buf, sizeof(buf), "ERROR :%s", fmt);
    va_start(args, fmt);
    vsend_cmd(NULL, buf, args);
    va_end(args);
    disconn(servsock);
}

/*************************************************************************/

/* Send a command to change channel modes.  (Needed to handle various
 * varieties of timestamping.  We currently cheat and use a timestamp of
 * zero to force our modes through.)
 */

void send_cmode_cmd(const char *source, const char *channel,
                    const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (protocol_features & PF_MODETS_FIRST)
        send_channel_cmd(source, "MODE %s 0 %s", channel, buf);
    else
        send_channel_cmd(source, "MODE %s %s", channel, buf);
}

/*************************************************************************/

/* Introduce a pseudoclient nickname.  `flags' includes PSEUDO_OPER if the
 * pseudoclient requires IRC operator privileges (however, the client may
 * not actually get +o if the server does not require it), and PSEUDO_INVIS
 * if the pseudoclient should be invisible (+i).
 */

void send_pseudo_nick(const char *nick, const char *realname, int flags)
{
    char modebuf[BUFSIZE];

    snprintf(modebuf, sizeof(modebuf), "%s%s%s",
             pseudoclient_modes,
             (flags & PSEUDO_OPER) && pseudoclient_oper ? "o" : "",
             (flags & PSEUDO_INVIS) ? "i" : "");
    send_nick(nick, ServiceUser, ServiceHost, ServerName, realname, modebuf);
}

/*************************************************************************/

/* Send a NOTICE from the given source to the given nick. */

void notice(const char *source, const char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source, "NOTICE %s :%s", dest, buf);
}


/* Send a NULL-terminated array of text as NOTICEs. */

void notice_list(const char *source, const char *dest, const char **text)
{
    while (*text) {
        /* Have to kludge around client/server silliness here: if a notice
         * includes no text, it is ignored, so we replace blank lines by
         * lines with a single space. */
        if (**text)
            notice(source, dest, *text);
        else
            notice(source, dest, " ");
        text++;
    }
}


/* Send a message in the user's selected language to the user using NOTICE. */

void notice_lang(const char *source, const User *dest, int message, ...)
{
    va_list args;
    char buf[4096];     /* because messages can be really big */
    char *s, *t;
    const char *fmt;

    if (!dest)
        return;
    fmt = getstring(dest->ngi, message);
    if (!fmt)
        return;
    va_start(args, message);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    s = buf;
    while (*s) {
        char c;
        t = s;
        s += strcspn(s, "\n");
        c = *s;
        *s = 0;
        send_cmd(source, "NOTICE %s :%s", dest->nick, *t ? t : " ");
        *s = c;
        if (c)
            s++;
    }
}


/* Like notice_lang(), but replace %S by the source.  This is an ugly hack
 * to simplify letting help messages display the name of the pseudoclient
 * that's sending them.
 */
void notice_help(const char *source, const User *dest, int message, ...)
{
    va_list args;
    char buf[4096], buf2[4096], outbuf[BUFSIZE];
    char *s, *t;
    const char *fmt;

    if (!dest)
        return;
    fmt = getstring(dest->ngi, message);
    if (!fmt)
        return;
    /* Some sprintf()'s eat %S or turn it into just S, so change all %S's
     * into \1\1... we assume this doesn't occur anywhere else in the
     * string. */
    strbcpy(buf2, fmt);
    strnrepl(buf2, sizeof(buf2), "%S", "\1\1");
    va_start(args, message);
    vsnprintf(buf, sizeof(buf), buf2, args);
    va_end(args);
    s = buf;
    while (*s) {
        char c;
        t = s;
        s += strcspn(s, "\n");
        c = *s;
        *s = 0;
        strbcpy(outbuf, t);
        *s = c;
        if (c)
            s++;
        strnrepl(outbuf, sizeof(outbuf), "\1\1", source);
        send_cmd(source, "NOTICE %s :%s", dest->nick, *outbuf ? outbuf : " ");
    }
}

/*************************************************************************/

/* Send a PRIVMSG from the given source to the given nick. */

void privmsg(const char *source, const char *dest, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source, "PRIVMSG %s :%s", dest, buf);
}

/*************************************************************************/
/*************************************************************************/

/* Handler for unimplemented functions. */

static void unimplemented(void)
{
    fatal("send.c: No (or bad) protocol module loaded.");
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
