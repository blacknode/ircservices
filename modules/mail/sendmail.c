/* Module to send mail using a "sendmail" program.
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
#include "modules/mail/mail.h"
#include "modules/mail/mail-local.h"
#include <sys/wait.h>  /* for WIFEXITED(), etc. */

/*************************************************************************/

static char *SendmailPath;


/*************************************************************************/
/***************************** Mail sending ******************************/
/*************************************************************************/

static void send_sendmail(MailMessage *msg)
{
    FILE *pipe;
    char cmd[PATH_MAX+4];
    char buf[BUFSIZE], *s;
    int res;
    time_t t;

    snprintf(cmd, sizeof(cmd), "%s -t", SendmailPath);
    pipe = popen(cmd, "w");
    if (!pipe) {
        module_log_perror("Unable to execute %s", SendmailPath);
        send_finished(msg, MAIL_STATUS_ERROR);
        return;
    }

    /* Quote all double-quotes in from-name string */
    if (*msg->fromname) {
        const char *fromname = msg->fromname;
        s = buf;
        while (s < buf+sizeof(buf)-2 && *fromname) {
            if (*fromname == '"')
                *s++ = '\\';
            *s++ = *fromname++;
        }
        *s = 0;
        fprintf(pipe, "From: \"%s\" <%s>\n", buf, msg->from);
    } else {
        fprintf(pipe, "From: %s\n", msg->from);
    }
    time(&t);
    if (!strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S", gmtime(&t)))
        strbcpy(buf, "Thu, 1 Jan 1970 00:00:00");
    fprintf(pipe, "To: %s\nSubject: %s\nDate: %s +0000\n",
            msg->to, msg->subject, buf);
    if (msg->charset) {
        fprintf(pipe,
                "MIME-Version: 1.0\nContent-Type: text/plain; charset=%s\n",
                msg->charset);
    }
    fprintf(pipe, "\n%s\n", msg->body);
    res = pclose(pipe);
    if (res == -1) {
        module_log_perror("pclose() failed");
    } else if (res != 0) {
        module_log_debug(2, "sendmail exit code = %04X\n", res);
        module_log("%s exited with %s %d%s", SendmailPath,
                   WIFEXITED(res) ? "code" : "signal",
                   WIFEXITED(res) ? WEXITSTATUS(res) : WTERMSIG(res),
                   WIFEXITED(res) && WEXITSTATUS(res)==127
                       ? " (unable to execute program?)" : "");
        send_finished(msg, MAIL_STATUS_ERROR);
        return;
    }
    send_finished(msg, MAIL_STATUS_SENT);
}

/*************************************************************************/

static void abort_sendmail(MailMessage *msg)
{
    /* send_sendmail() always completes handling of the message, so this
     * routine doesn't need to do anything */
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_SendmailPath(const char *filename, int linenum, char *param);
static ConfigDirective sendmail_config[] = {
    { "SendmailPath",     { { CD_FUNC, CF_DIRREQ, do_SendmailPath } } },
    { NULL }
};

/*************************************************************************/

static int do_SendmailPath(const char *filename, int linenum, char *param)
{
    static char *new_SendmailPath = NULL;

    if (filename) {
        /* Check parameter for validity and save */
        if (*param != '/') {
            config_error(filename, linenum,
                         "SendmailPath value must begin with a slash (`/')");
            return 0;
        }
        free(new_SendmailPath);
        new_SendmailPath = strdup(param);
        if (!new_SendmailPath) {
            config_error(filename, linenum, "Out of memory");
            return 0;
        }
    } else if (linenum == CDFUNC_SET) {
        /* Copy new values to config variables and clear */
        if (new_SendmailPath) {  /* paranoia */
            free(SendmailPath);
            SendmailPath = new_SendmailPath;
        } else {
            free(new_SendmailPath);
        }
        new_SendmailPath = NULL;
    } else if (linenum == CDFUNC_DECONFIG) {
        /* Reset to defaults */
        free(SendmailPath);
        SendmailPath = NULL;
    }
    return 1;
}

/*************************************************************************/

/* mail/main is required, so its hooks for the low-level sender are there
 * to be set directly (mail-local.h). */

static int sendmail_init(Module *module)
{
    low_send = send_sendmail;
    low_abort = abort_sendmail;
    return 1;
}

/*************************************************************************/

static int sendmail_fini(Module *module, int shutdown)
{
    if (low_send == send_sendmail)
        low_send = NULL;
    if (low_abort == abort_sendmail)
        low_abort = NULL;
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "Mail: send through the sendmail program",
    .requires = MODULE_REQUIRES("mail/main"),
    .config = sendmail_config,
    .init = sendmail_init,
    .fini = sendmail_fini,
};

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
