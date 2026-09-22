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

/* Priorities for use with receive memo callback: */

#define MS_RECEIVE_PRI_CHECK    10      /* For checking ability to send */
#define MS_RECEIVE_PRI_DELIVER  0       /* For actually sending the memo */

/*************************************************************************/

/* Exports: */

extern char *s_MemoServ;
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
