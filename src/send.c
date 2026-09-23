/* Routines for sending stuff to the network.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "language.h"
#include "p10.h"
#include "services.h"

/*************************************************************************/

time_t last_send; /* Time last data was sent to server */

/* Modes to send for Services users. */
const char* pseudoclient_modes = "";
const char* enforcer_modes = "";
/* Do "oper" pseudoclients really need oper privileges? (1 or 0) */
int pseudoclient_oper = 1;

/* Protocol information, filled in by p10_init(). */
const char* protocol_name = NULL;
const char* protocol_version = NULL;
uint32 protocol_features = 0;
int protocol_nickmax = 0;

/*************************************************************************/
/*************************************************************************/

/* Send a command to the server.  The two forms here are like
 * printf()/vprintf() and friends.  If not connected to a remote server,
 * these functions do nothing.
 */

void send_cmd(const char* source, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsend_cmd(source, fmt, args);
    va_end(args);
}

void vsend_cmd(const char* source, const char* fmt, va_list args)
{
    char buf[BUFSIZE];

    if (!servsock)
        return;
    vsnprintf(buf, sizeof(buf), fmt, args);
    p10_send(source, buf);
}

/*************************************************************************/
/*************************************************************************/

/* Send an ERROR message and close the connection to the server. */

void send_error(const char* fmt, ...)
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

void send_cmode_cmd(const char* source, const char* channel, const char* fmt,
                    ...)
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

void send_pseudo_nick(const char* nick, const char* realname, int flags)
{
    char modebuf[BUFSIZE];

    snprintf(modebuf, sizeof(modebuf), "%s%s%s", pseudoclient_modes,
             (flags & PSEUDO_OPER) && pseudoclient_oper ? "o" : "",
             (flags & PSEUDO_INVIS) ? "i" : "");
    send_nick(nick, ServiceUser, ServiceHost, ServerName, realname, modebuf);
}

/*************************************************************************/

/* Send a NOTICE from the given source to the given nick. */

void notice(const char* source, const char* dest, const char* fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source, "NOTICE %s :%s", dest, buf);
}

/* Send a NULL-terminated array of text as NOTICEs. */

void notice_list(const char* source, const char* dest, const char** text)
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

void notice_lang(const char* source, const User* dest, int message, ...)
{
    va_list args;
    char buf[4096]; /* because messages can be really big */
    char *s, *t;
    const char* fmt;

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
void notice_help(const char* source, const User* dest, int message, ...)
{
    va_list args;
    char buf[4096], buf2[4096], outbuf[BUFSIZE];
    char *s, *t;
    const char* fmt;

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

void privmsg(const char* source, const char* dest, const char* fmt, ...)
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
