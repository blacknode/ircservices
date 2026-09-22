/* Include file for ChanServ access level data.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CHANSERV_ACCESS_H
#define CHANSERV_ACCESS_H

/*************************************************************************/

/* Access level data structure: */
typedef struct levelinfo_ LevelInfo;
struct levelinfo_ {
    int what;           /* Level constant (CA_*) */
    int defval;         /* Default level */
    const char *name;   /* Level name as a string */
    int desc;           /* Description message number */
    int action;         /* What this level does (CL_*) */
    union {             /* Target of `action', as appropriate */
        struct {            /* Mode(s) to auto-apply to user */
            const char *modes;
            int cont;       /* If we set this level, do we ignore the next */
                            /*    levelinfo[] entry? */
            int32 flags;    /* Set at init time */
        } cumode;
        struct {            /* Command (and subcommand, e.g. option for SET) */
            const char *main;
            const char *sub;
        } cmd;
    } target;
};

/* What does a level do? */
#define CL_SET_MODE     0       /* Set a user mode */
#define CL_CLEAR_MODE   1       /* Clear a user mode */
#define CL_ALLOW_CMD    2       /* Allow a command to be used */
#define CL_OTHER        0x7F    /* Specially handled, or no-op */
#define CL_TYPEMASK     0x7F
#define CL_LESSEQUAL    0x80    /* Apply to users <= level (OR with type) */

/* List of defined levels with descriptions: */
extern LevelInfo levelinfo[];

/*************************************************************************/

#endif  /* CHANSERV_ACCESS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
