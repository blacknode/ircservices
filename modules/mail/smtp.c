/* Module to send mail using the SMTP protocol.
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
#include "mail.h"
#include "mail-local.h"

/*************************************************************************/

static char **RelayHosts;
int RelayHosts_count;
static char *SMTPName;

static Module *module_mail_main;
static typeof(low_send) *low_send_p;
static typeof(low_abort) *low_abort_p;

/*************************************************************************/

/* Maximum number of garbage lines to accept before disconnecting: */
#define GARBAGE_MAX     10

typedef struct SocketInfo_ {
    struct SocketInfo_ *next, *prev;
    Socket *sock;
    MailMessage *msg;
    int msg_status;
    int relaynum;   /* Index into RelayHosts[], used to try backup servers */
    enum { ST_GREETING, ST_HELO, ST_MAIL, ST_RCPT, ST_DATA, ST_FINISH } state;
    int replycode;  /* nonzero if in the middle of a line (no EOL) */
    char replychar; /* 4th character of reply (space or hyphen) */
    int garbage;    /* number of garbage lines seen so far */
} SocketInfo;
static SocketInfo *connections;

static SocketInfo *get_socketinfo(const Socket *sock, const MailMessage *msg);
static void free_socketinfo(SocketInfo *si);
static void try_next_relay(SocketInfo *si);
static void smtp_readline(Socket *sock, void *param_unused);
static void smtp_writeline(Socket *sock, const char *fmt, ...)
    FORMAT(printf,2,3);
static void smtp_disconnect(Socket *sock, void *why);

/*************************************************************************/
/***************************** Mail sending ******************************/
/*************************************************************************/

static void send_smtp(MailMessage *msg)
{
    SocketInfo *si;

    /* Remove any double quotes in the From: name and log a warning */
    if (strchr(msg->fromname, '"')) {
        int i;
        module_log("warning: double quotes (\") are not allowed in the"
                   " sender name; will be changed to single quotes (')");
        for (i = 0; msg->fromname[i]; i++) {
            if (msg->fromname[i] == '"')
                msg->fromname[i] = '\'';
        }
    }

    /* Set up a new SocketInfo and start the connection */
    si = malloc(sizeof(*si));
    if (!si) {
        module_log("send_smtp(): no memory for SocketInfo");
        send_finished(msg, MAIL_STATUS_NORSRC);
        return;
    }
    si->sock = sock_new();
    if (!si->sock) {
        module_log("send_smtp(): sock_new() failed");
        send_finished(msg, MAIL_STATUS_NORSRC);
        free(si);
        return;
    }
    LIST_INSERT(si, connections);
    module_log_debug(1, "SMTP(%p) connecting", si->sock);
    si->msg = msg;
    si->msg_status = MAIL_STATUS_ERROR;  /* default--don't depend on this! */
    si->state = ST_GREETING;
    si->replycode = 0;
    si->garbage = 0;
    sock_setcb(si->sock, SCB_READLINE, smtp_readline);
    sock_setcb(si->sock, SCB_DISCONNECT, smtp_disconnect);
    si->relaynum = -1;  /* incremented by try_next_relay() */
    /* Initiate connection and return */
    errno = 0;  /* just in case */
    try_next_relay(si);
}

/*************************************************************************/
/*************************************************************************/

/* Auxiliary routines: */

/*************************************************************************/

/* Return the SocketInfo corresponding to the given socket or message, or
 * NULL if none exists.  (If `sock' is non-NULL, the search is done by
 * socket, else by message.)
 */

static SocketInfo *get_socketinfo(const Socket *sock, const MailMessage *msg)
{
    SocketInfo *si;

    LIST_FOREACH (si, connections) {
        if (sock ? si->sock == sock : si->msg == msg)
            return si;
    }
    return NULL;
}

/*************************************************************************/

/* Free/clear all data associated with the given SocketInfo.  If a message
 * is still associated with the SocketInfo, abort it.
 */

static void free_socketinfo(SocketInfo *si)
{
    if (si->msg) {
        send_finished(si->msg, MAIL_STATUS_ABORTED);
        si->msg = NULL;
    }
    if (si->sock) {
        /* The disconnect callback will try to call us again, so avoid
         * that confusing situation */
        sock_setcb(si->sock, SCB_DISCONNECT, NULL);
        module_log_debug(1, "SMTP(%p) closed (free_socketinfo)", si->sock);
        sock_free(si->sock);
        si->sock = NULL;
    }
    LIST_REMOVE(si, connections);
    free(si);
}

/*************************************************************************/
/*************************************************************************/

/* Try connecting to the next relay in RelayHosts[].  Return 0 if a
 * connection was successfully initiated (but possibly not completed), else
 * free the SocketInfo and return -1. */

static void try_next_relay(SocketInfo *si)
{
    for (;;) {
        si->relaynum++;
        if (si->relaynum >= RelayHosts_count) {
            module_log("send_smtp(): No relay hosts available");
            if (errno == ECONNREFUSED)
                si->msg_status = MAIL_STATUS_REFUSED;
            else
                si->msg_status = MAIL_STATUS_ERROR;
            send_finished(si->msg, si->msg_status);
            si->msg = NULL;
            free_socketinfo(si);
            return;
        }
        if (conn(si->sock, RelayHosts[si->relaynum], 25, NULL, 0) == 0)
            break;
        module_log_perror("send_smtp(): Connection to %s:25 failed",
                          RelayHosts[si->relaynum]);
    }
}

/*************************************************************************/

/* Read a line from an SMTP socket. */

static void smtp_readline(Socket *sock, void *param_unused)
{
    SocketInfo *si;
    char buf[BUFSIZE], *s;
    int have_eol = 0;
    int replycode;


    if (!(si = get_socketinfo(sock, NULL))) {
        module_log("smtp_readline(): no SocketInfo for socket %p!", sock);
        sock_setcb(sock, SCB_DISCONNECT, NULL);
        disconn(sock);
        return;
    }

    sgets(buf, sizeof(buf), sock);
    s = buf + strlen(buf);
    if (s[-1] == '\n') {
        s--;
        have_eol++;
    }
    if (s[-1] == '\r') {
        s--;
    }
    *s = 0;
    module_log_debug(1, "SMTP(%p) received: %s", sock, buf);
    if (!si->replycode) {
        if (buf[0] < '1' || buf[0] > '5'
         || buf[1] < '0' || buf[1] > '9'
         || buf[2] < '0' || buf[2] > '9'
         || (buf[3] != ' ' && buf[3] != '-')) {
            module_log("smtp_readline(%p) got garbage line: %s", sock, buf);
            si->garbage++;
            if (si->garbage > GARBAGE_MAX) {
                int count = 0;
                module_log("Too many garbage lines, giving up.  Message was:");
                module_log("   From: %s%s<%s>",
                           si->msg->fromname ? si->msg->fromname : "",
                           si->msg->fromname ? " " : "", si->msg->from);
                module_log("   To: %s", si->msg->to);
                module_log("   Subject: %s", si->msg->subject);
                s = si->msg->body;
                while (*s) {
                    char *t = s + strcspn(s, "\n");
                    if (*t)
                        *t++ = 0;
                    module_log("   %s %s", count ? "     " : "Body:", s);
                    count++;
                    s = t;
                }
                si->msg_status = MAIL_STATUS_ERROR;
                disconn(si->sock);
                return;
            }
        }
        si->replycode = strtol(buf, &s, 10);
        if (s != buf+3) {
            module_log("BUG: strtol ate %d characters from reply (should be"
                       " 3)!", (int)(s-buf));
        }
        if (si->replycode == 0) {
            module_log("Got bizarre response code 0 from %s",
                       RelayHosts[si->relaynum]);
            si->replycode = 1;
        }
        si->replychar = buf[3];
    }
    if (!have_eol)
        return;
    replycode = si->replycode;
    si->replycode = 0;
    if (si->replychar != ' ')
        return;

    if (replycode >= 400) {
        module_log("Received error reply (%d) for socket %p state %d,"
                   " aborting", replycode, sock, si->state);
        si->msg_status = MAIL_STATUS_REFUSED;
        disconn(si->sock);
        return;
    }
    switch (si->state++) {
      case ST_GREETING:
        smtp_writeline(sock, "HELO %s", SMTPName);
        break;
      case ST_HELO:
        smtp_writeline(sock, "MAIL FROM:<%s>", si->msg->from);
        break;
      case ST_MAIL:
        smtp_writeline(sock, "RCPT TO:<%s>", si->msg->to);
        break;
      case ST_RCPT:
        smtp_writeline(sock, "DATA");
        break;
      case ST_DATA: {
        time_t t;
        time(&t);
        if (!strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", gmtime(&t)))
            strbcpy(buf, "Thu, 1 Jan 1970 00:00:00");
        if (si->msg->fromname)
            smtp_writeline(sock, "From: \"%s\" <%s>", si->msg->fromname,
                           si->msg->from);
        else
            smtp_writeline(sock, "From: <%s>", si->msg->from);
        smtp_writeline(sock, "To: <%s>", si->msg->to);
        smtp_writeline(sock, "Subject: %s", si->msg->subject);
        smtp_writeline(sock, "Date: %s +0000", buf);
        if (si->msg->charset) {
            smtp_writeline(sock, "MIME-Version: 1.0");
            smtp_writeline(sock, "Content-Type: text/plain; charset=%s",
                           si->msg->charset);
        }
#if CLEAN_COMPILE
        /* writeline(sock,"") makes GCC warn about an empty format string */
        smtp_writeline(sock, "%s", "");
#else
        smtp_writeline(sock, "");
#endif
        s = si->msg->body;
        while (*s) {
            char *t = s + strcspn(s, "\r\n");
            if (*t == '\r')
                *t++ = 0;
            if (*t == '\n')
                *t++ = 0;
            smtp_writeline(sock, "%s%s", *s=='.' ? "." : "", s);
            s = t;
        }
        smtp_writeline(sock, ".");
        break;
      } /* ST_DATA */
      case ST_FINISH:
        smtp_writeline(sock, "QUIT");
        si->msg_status = MAIL_STATUS_SENT;
        disconn(si->sock);
        break;
      default:
        module_log("BUG: bad state %d for socket %p", si->state-1, sock);
        si->msg_status = MAIL_STATUS_ERROR;
        disconn(si->sock);
        break;
    } /* switch (si->state++) */
}

/*************************************************************************/

static void smtp_writeline(Socket *sock, const char *fmt, ...)
{
    va_list args;
    char buf[4096];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    sockprintf(sock, "%s", buf);
    swrite(sock, "\r\n", 2);
    module_log_debug(1, "SMTP(%p) sent: %s", sock, buf);
}

/*************************************************************************/

/* Handle a socket disconnection. */

static void smtp_disconnect(Socket *sock, void *why)
{
    SocketInfo *si;

    if (!(si = get_socketinfo(sock, NULL))) {
        module_log("smtp_disconnect(): no SocketInfo for socket %p!", sock);
        return;
    }

    module_log_debug(1, "SMTP(%p) closed (%s)", sock,
                     why==DISCONN_LOCAL ? "local" :
                     why==DISCONN_CONNFAIL ? "connfail" : "remote");
    if (why == DISCONN_CONNFAIL) {
        module_log_perror("Connection to server %s failed for socket %p",
                          RelayHosts[si->relaynum], sock);
        try_next_relay(si);  /* will call send_finished(), etc. if needed */
        return;
    } else if (why == DISCONN_REMOTE) {
        module_log("Connection to server %s broken for socket %p",
                   RelayHosts[si->relaynum], sock);
        si->msg_status = MAIL_STATUS_ERROR;
    }
    send_finished(si->msg, si->msg_status);
    si->msg = NULL;
    free_socketinfo(si);
}

/*************************************************************************/

static void abort_smtp(MailMessage *msg)
{
    SocketInfo *si;

    if (!(si = get_socketinfo(NULL, msg))) {
        module_log("abort_smtp(): no SocketInfo for message %p!", msg);
        return;
    }
    module_log_debug(1, "SMTP(%p) aborted", si);
    si->msg = NULL;
    free_socketinfo(si);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_RelayHost(const char *filename, int linenum, char *param);
ConfigDirective module_config[] = {
    { "RelayHost",        { { CD_FUNC, CF_DIRREQ, do_RelayHost } } },
    { "SMTPName",         { { CD_STRING, CF_DIRREQ, &SMTPName } } },
    { NULL }
};

/*************************************************************************/

static int do_RelayHost(const char *filename, int linenum, char *param)
{
    static char **new_RelayHosts = NULL;
    static int new_RelayHosts_count = 0;
    int i;

    if (filename) {
        /* Check parameter for validity and save */
        if (!*param) {
            /* Empty value */
            config_error(filename, linenum, "Empty hostname in RelayHost");
        }
        ARRAY_EXTEND(new_RelayHosts);
        new_RelayHosts[new_RelayHosts_count-1] = sstrdup(param);
    } else if (linenum == CDFUNC_INIT) {
        /*Prepare for reading config file: clear out "new" array just in case*/
        ARRAY_FOREACH (i, new_RelayHosts)
            free(new_RelayHosts[i]);
        free(new_RelayHosts);
        new_RelayHosts = NULL;
        new_RelayHosts_count = 0;
    } else if (linenum == CDFUNC_SET) {
        /* Copy new values to config variables and clear */
        ARRAY_FOREACH (i, RelayHosts)
            free(RelayHosts[i]);
        free(RelayHosts);
        RelayHosts = new_RelayHosts;
        RelayHosts_count = new_RelayHosts_count;
        new_RelayHosts = NULL;
        new_RelayHosts_count = 0;
    } else if (linenum == CDFUNC_DECONFIG) {
        /* Reset to defaults */
        ARRAY_FOREACH (i, RelayHosts)
            free(RelayHosts[i]);
        free(RelayHosts);
        RelayHosts = NULL;
        RelayHosts_count = 0;
    }
    return 1;
}

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "mail/main") == 0) {
        module_mail_main = mod;
        low_send_p = get_module_symbol(mod, "low_send");
        if (low_send_p)
            *low_send_p = send_smtp;
        else
            module_log("Unable to find `low_send' symbol, cannot send mail");
        low_abort_p = get_module_symbol(mod, "low_abort");
        if (low_abort_p)
            *low_abort_p = abort_smtp;
        else
            module_log("Unable to find `low_abort' symbol, cannot send mail");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_mail_main) {
        if (low_send_p)
            *low_send_p = NULL;
        if (low_abort_p)
            *low_abort_p = NULL;
        low_send_p = NULL;
        low_abort_p = NULL;
        module_mail_main = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    Module *tmpmod;

    connections = NULL;

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    tmpmod = find_module("mail/main");
    if (tmpmod)
        do_load_module(tmpmod, "mail/main");

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    SocketInfo *si, *si2;

    if (module_mail_main)
        do_unload_module(module_mail_main);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    LIST_FOREACH_SAFE (si, connections, si2)
        free_socketinfo(si);

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
