/* Core mail-sending module.
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

static char *FromAddress;
static char def_FromName[] = "";
static char *FromName = def_FromName;
static int MaxMessages;
static time_t SendTimeout;

/* Low-level send/abort routines (filled in by submodule) */
EXPORT_VAR(void *,low_send)
void (*low_send)(MailMessage *msg) = NULL;
EXPORT_VAR(void *,low_abort)
void (*low_abort)(MailMessage *msg) = NULL;

/* List of mail messages currently in transit */
MailMessage *messages = NULL;

/*************************************************************************/

static void send_timeout(Timeout *t);

/*************************************************************************/
/***************************** Mail sending ******************************/
/*************************************************************************/

/* The function to send mail.  Parameters `to', `subject', and `body' must
 * be filled in, `to' must be a valid E-mail address, and only the body may
 * contain newlines.  If `charset' is not NULL, it is used as the MIME
 * character set (if NULL, the character set is assumed to be unspecified).
 * If `completion_callback' is not NULL, it will be called when delivery of
 * the mail has completed or failed (which may be before this function
 * returns!) with a delivery code (MAIL_STATUS_*) and the value passed as
 * `callback_data'.
 *
 * The parameters `to', `subject', `body', and `charset' may be modified
 * after calling this function, even if the completion callback has not yet
 * been called.
 */

EXPORT_FUNC(sendmail)
void sendmail(const char *to, const char *subject, const char *body,
              const char *charset,
              MailCallback completion_callback, void *callback_data)
{
    MailMessage *msg;
    int status;

    status = MAIL_STATUS_ERROR;
    if (!low_send || !low_abort) {
        module_log("sendmail(): No low-level mail module installed!");
        goto error_return;
    }
    if (!to || !subject || !body) {
        module_log("sendmail(): Got a NULL parameter!");
        goto error_return;
    }
    if (!valid_email(to)) {
        module_log("sendmail(): Destination address is invalid: %s", to);
        goto error_return;
    }
    if (strchr(subject, '\n')) {
        module_log("sendmail(): Subject contains newlines (invalid)");
        goto error_return;
    }

    status = MAIL_STATUS_NORSRC;
    if (MaxMessages) {
        /* Count number of messages in transit, and abort if there are
         * too many */
        int count = 0;
        LIST_FOREACH (msg, messages) {
            if (++count >= MaxMessages)
                break;
        }
        if (count >= MaxMessages) {
            module_log("sendmail(): Too many messages in transit (max %d)",
                       MaxMessages);
            goto error_return;
        }
    }
    msg = malloc(sizeof(*msg));
    if (!msg) {
        module_log("sendmail(): No memory for message structure");
        goto error_return;
    }
    LIST_INSERT(msg, messages);
    msg->completion_callback = completion_callback;
    msg->callback_data = callback_data;
    msg->from = strdup(FromAddress);
    msg->to = strdup(to);
    msg->subject = strdup(subject);
    msg->body = strdup(body);
    if (charset)
        msg->charset = strdup(charset);
    else
        msg->charset = NULL;
    if (FromName)
        msg->fromname = strdup(FromName);
    else
        msg->fromname = NULL;
    if (!msg->from || !msg->to || !msg->subject || !msg->body
     || (charset && !msg->charset) || (FromName && !msg->fromname)
    ) {
        module_log("sendmail(): No memory for message data");
        send_finished(msg, MAIL_STATUS_NORSRC);
        return;
    }
    if (SendTimeout > 0) {
        msg->timeout = add_timeout(SendTimeout, send_timeout, 0);
        if (!msg->timeout) {
            module_log("sendmail(): Unable to create timeout");
            send_finished(msg, MAIL_STATUS_NORSRC);
            return;
        }
        msg->timeout->data = msg;
    } else {
        msg->timeout = NULL;
    }
    
    module_log_debug(1, "sendmail: from=%s to=%s subject=[%s]",
                     msg->from, msg->to, msg->subject);
    module_log_debug(2, "sendmail: body=[%s]", msg->body);
    (*low_send)(msg);
    return;

  error_return:
    if (completion_callback)
        (*completion_callback)(status, callback_data);
}

/*************************************************************************/

/* Handle a timeout on a send operation.  Aborts the send and returns
 * MAIL_STATUS_TIMEOUT to the caller's completion callback.
 */

static void send_timeout(Timeout *t)
{
    MailMessage *msg = t->data;

    if (!low_abort) {
        module_log("send_timeout(): No low-level mail module installed!");
        return;
    }
    (*low_abort)(msg);
    send_finished(msg, MAIL_STATUS_TIMEOUT);
}

/*************************************************************************/

/* Call a message's completion callback function with the given status,
 * then unlink and free the message structure.  Called by sendmail() and
 * low_send().
 */

EXPORT_FUNC(send_finished)
void send_finished(MailMessage *msg, int status)
{
    if (!msg) {
        module_log("BUG: send_finished() called with msg==NULL");
        return;
    }
    if (msg->completion_callback)
        (*msg->completion_callback)(status, msg->callback_data);
    free(msg->from);
    free(msg->fromname);
    free(msg->to);
    free(msg->subject);
    free(msg->body);
    free(msg->charset);
    if (msg->timeout)
        del_timeout(msg->timeout);
    LIST_REMOVE(msg, messages);
    free(msg);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_FromAddress(const char *filename, int linenum, char *param);
static int do_FromName(const char *filename, int linenum, char *param);
ConfigDirective module_config[] = {
    { "FromAddress",      { { CD_FUNC, CF_DIRREQ, do_FromAddress } } },
    { "FromName",         { { CD_FUNC, 0, do_FromName } } },
    { "MaxMessages",      { { CD_POSINT, 0, &MaxMessages } } },
    { "SendTimeout",      { { CD_TIME, 0, &SendTimeout } } },
    { NULL }
};

/*************************************************************************/

static int do_FromAddress(const char *filename, int linenum, char *param)
{
    static char *new_FromAddress = NULL;

    if (filename) {
        /* Check parameter for validity and save */
        if (!valid_email(param)) {
            config_error(filename, linenum,
                         "FromAddress requires a valid E-mail address");
            return 0;
        }
        free(new_FromAddress);
        new_FromAddress = strdup(param);
        if (!new_FromAddress) {
            config_error(filename, linenum, "Out of memory");
            return 0;
        }
    } else if (linenum == CDFUNC_SET) {
        /* Copy new values to config variables and clear */
        if (new_FromAddress) {  /* paranoia */
            free(FromAddress);
            FromAddress = new_FromAddress;
        } else {
            free(new_FromAddress);
        }
        new_FromAddress = NULL;
    } else if (linenum == CDFUNC_DECONFIG) {
        /* Reset to defaults */
        free(FromAddress);
        FromAddress = NULL;
    }
    return 1;
}

/*************************************************************************/

static int do_FromName(const char *filename, int linenum, char *param)
{
    static char *new_FromName = NULL;

    if (filename) {
        /* Check parameter for validity and save */
        if (strchr(param, '\n')) {
            config_error(filename, linenum,
                         "FromName may not contain newlines");
            return 0;
        }
        free(new_FromName);
        new_FromName = strdup(param);
        if (!new_FromName) {
            config_error(filename, linenum, "Out of memory");
            return 0;
        }
    } else if (linenum == CDFUNC_SET) {
        /* Copy new values to config variables and clear */
        if (new_FromName) {  /* paranoia */
            if (FromName != def_FromName)
                free(FromName);
            FromName = new_FromName;
        } else {
            free(new_FromName);
        }
        new_FromName = NULL;
    } else if (linenum == CDFUNC_DECONFIG) {
        /* Reset to defaults */
        if (FromName != def_FromName)
            free(FromName);
        FromName = def_FromName;
    }
    return 1;
}

/*************************************************************************/

int init_module()
{
    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    MailMessage *msg, *msg2;

    LIST_FOREACH_SAFE(msg, messages, msg2) {
        if (!low_abort) {
            if (msg == messages)  /* only print message once */
                module_log("exit_module(): BUG: No low-level mail module"
                           " installed but in-transit messages remain"
                           " (submodule forgot to abort them?)");
        } else {
            (*low_abort)(msg);
        }
        send_finished(msg, MAIL_STATUS_ABORTED);
    }

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
