/* Internal mail system declarations.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MAIL_LOCAL_H
#define MAIL_LOCAL_H

#ifndef TIMEOUT_H
# include "timeout.h"
#endif

/*************************************************************************/

/* Structure describing a message to be sent. */

typedef struct mailmessage_struct MailMessage;
struct mailmessage_struct {
    MailMessage *next, *prev;
    char *from;
    char *fromname;
    char *to;
    char *subject;
    char *body;
    char *charset;
    MailCallback completion_callback;
    void *callback_data;
    Timeout *timeout;
};

/*************************************************************************/

/* Pointer to low-level send routine.  Low-level modules should set this
 * to point to their own send routine.  This routine should initiate mail
 * sending, then call send_finished() with an appropriate status code when
 * the sending completes (successfully or otherwise).
 *
 * The routine can assume that the `from', `to', `subject', and `body'
 * fields of the message will be non-NULL.
 *
 * The routine can do as it likes with the message data stored in the
 * message structure (`from', `fromname', `to', `subject', and `body'),
 * but the (pointer) values of the fields themselves should not be changed.
 */
extern void (*low_send)(MailMessage *msg);

/* Pointer to low-level abort routine.  Low-level modules should set this
 * to point to their own abort routine.  This routine will be called when
 * the high-level code needs to abort a message (for example, on a send
 * timeout); the routine MUST abort sending of the given message.
 * send_finished() should not be called, as the high-level code will take
 * care of that.
 *
 * For modules which always complete processing before returning from
 * low_send(), this will never be called, so it may be an empty routine;
 * however, the `low_abort' pointer must be set or sendmail() will report
 * that no low-level sending module is installed.
 */
extern void (*low_abort)(MailMessage *msg);

/* Routine to be called by low_send() when the given message has been sent
 * (or sending has failed).  The message structure must not be accessed
 * after calling this routine!
 */
extern void send_finished(MailMessage *msg, int status);

/*************************************************************************/

#endif  /* MAIL_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
