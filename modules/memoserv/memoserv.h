/* MemoServ-related structures.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MEMOSERV_H
#define MEMOSERV_H

#include "services.h"

/*************************************************************************/

/* Memo info structures. */

typedef struct {
    uint32 number;      /* Index number -- not necessarily array position! */
    int16 flags;
    time_t time;        /* When memo was sent */
    time_t firstread;   /* When memo was first read */
    char sender[NICKMAX];
    char *channel;      /* Channel name if this is a channel memo, else NULL */
    char *text;
} Memo;

#define MF_UNREAD       0x0001  /* Memo has not yet been read */
#define MF_EXPIREOK     0x0002  /* Memo may be expired */

#define MF_ALLFLAGS     0x0003  /* All memo flags */

struct memoinfo_ {
    Memo *memos;
    int16 memos_count;
    int16 memomax;
};

/*************************************************************************/

/* Definitions of special memo limit values: */

#define MEMOMAX_UNLIMITED       -1
#define MEMOMAX_DEFAULT         -2

#define MEMOMAX_MAX             32767   /* Maximum memo limit */

/*************************************************************************/

/* Events of memoserv/main (see include/events.h).  "Stop" is what a
 * handler returning nonzero does.
 *
 * memoserv.command:       (User *u, char *command)  a command, before it
 *                         runs; stop: it was handled
 * memoserv.help:          (User *u, char *topic)  stop: help was given
 * memoserv.help_commands: (User *u, int which)  append to the list of
 *                         commands: 0 = all users, 1 = operators
 * memoserv.set:           (User *u, MemoInfo *mi, char *option,
 *                         char *param)  SET; stop: handled
 * memoserv.receive_memo:  (const char *source, const char *target,
 *                         NickGroupInfo *ngi, Channel *channel,
 *                         const char *text)  a memo on its way; return 1
 *                         if you delivered it, a language string number
 *                         (> 1) to refuse it with that message, 0 to let
 *                         it through (attach with the priorities below)
 */
#define MEMOSERV_EVENT_COMMAND       "memoserv.command"
#define MEMOSERV_EVENT_HELP          "memoserv.help"
#define MEMOSERV_EVENT_HELP_COMMANDS "memoserv.help_commands"
#define MEMOSERV_EVENT_SET           "memoserv.set"
#define MEMOSERV_EVENT_RECEIVE_MEMO  "memoserv.receive_memo"

/* Priorities for use with the receive memo event: */

#define MS_RECEIVE_PRI_CHECK    10      /* For checking ability to send */
#define MS_RECEIVE_PRI_DELIVER  0       /* For actually sending the memo */

/*************************************************************************/

/* Exports: */

extern struct Service memoserv_service;
extern int32 MSMaxMemos;

/*************************************************************************/

#endif  /* MEMOSERV_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
