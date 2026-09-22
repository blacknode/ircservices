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
#include "mail.h"
#include "mail-local.h"
#include <sys/wait.h>  /* for WIFEXITED(), etc. */

/*************************************************************************/

static char *SendmailPath;

static Module *module_mail_main;
static typeof(low_send) *low_send_p;
static typeof(low_abort) *low_abort_p;

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
ConfigDirective module_config[] = {
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

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "mail/main") == 0) {
        module_mail_main = mod;
        low_send_p = get_module_symbol(mod, "low_send");
        if (low_send_p)
            *low_send_p = send_sendmail;
        else
            module_log("Unable to find `low_send' symbol, cannot send mail");
        low_abort_p = get_module_symbol(mod, "low_abort");
        if (low_abort_p)
            *low_abort_p = abort_sendmail;
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
    if (module_mail_main)
        do_unload_module(module_mail_main);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);
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
