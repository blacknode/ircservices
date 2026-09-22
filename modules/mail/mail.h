/* Declarations for public mail routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MAIL_H
#define MAIL_H

/*************************************************************************/

/* Completion callback type: */
typedef void (*MailCallback)(int status, void *data);

/* Constants for mail delivery results, passed to the sendmail() completion
 * callback: */
#define MAIL_STATUS_SENT        0       /* Mail sent successfully */
#define MAIL_STATUS_ERROR       1       /* Some unspecified error occurred */
#define MAIL_STATUS_NORSRC      2       /* Insufficient resources available */
#define MAIL_STATUS_REFUSED     3       /* Remote system refused message */
#define MAIL_STATUS_TIMEOUT     4       /* Mail sending timed out */
#define MAIL_STATUS_ABORTED     5       /* Mail sending aborted (e.g. module
                                         *    removed while sending) */

/*************************************************************************/

/* Send mail to the given address, calling the given function (if any) on
 * completion or failure. */
extern void sendmail(const char *to, const char *subject, const char *body,
                     const char *charset,
                     MailCallback completion_callback, void *callback_data);

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
