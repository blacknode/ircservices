/* NickServ-related structures.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef NICKSERV_H
#define NICKSERV_H

#ifndef ENCRYPT_H
# include "../../encrypt.h"
#endif
#ifndef MEMOSERV_H
# include "../memoserv/memoserv.h"
#endif

/*************************************************************************/
/*************************************************************************/

/* Data structure for a single nickname. */

struct nickinfo_ {
    NickInfo *next, *prev;
    int usecount;

    char nick[NICKMAX];

    int16 status;       /* See NS_* below */
    char *last_usermask; /* Last user@host mask (uses fakehost if avail.) */
    char *last_realmask; /* Last user@realhost (not fakehost) mask seen */
    char *last_realname;
    char *last_quit;
    time_t time_registered;
    time_t last_seen;

    uint32 nickgroup;   /* ID of nick group for this nick */

    uint32 id_stamp;    /* Services stamp of user who last ID'd for nick */

    /* Online-only information: */

    User *user;         /* User using this nick, NULL if not online */
    int16 authstat;     /* Authorization status; see NA_* below */
    int bad_passwords;  /* # of bad passwords for nick since last good one */
};

/*************************************************************************/

/* Data for a group of nicks. */

struct nickgroupinfo_ {
    NickGroupInfo *next, *prev;
    int usecount;

    uint32 id;
    nickname_t *nicks;  /* Array of nicks */
    uint16 nicks_count; /* Number of nicks in this nick group */
    uint16 mainnick;    /* Index of "main" nick (for chan access lists etc) */

    Password pass;
    char *url;
    char *email;
    char *last_email;   /* Previous (authenticated) E-mail address */
    char *info;

    int32 flags;        /* See NF_* below */
    int16 os_priv;      /* OperServ privilege level; see NP_* below */

    int32 authcode;     /* Authentication code (used by mail-auth module) */
    time_t authset;     /* Time authentication code was set */
    int16 authreason;   /* Reason auth. code was set; see NICKAUTH_* below */

    char suspend_who[NICKMAX];  /* Who suspended this nickname */
    char *suspend_reason;       /* Reason for suspension */
    time_t suspend_time;        /* Time nick was suspended */
    time_t suspend_expires;     /* Time suspension expires, 0 for no expiry */

    int16 language;     /* Language selected by nickname owner (LANG_*) */
    int16 timezone;     /* Offset from UTC, in minutes */
    int16 channelmax;   /* Maximum number of registered channels allowed */

    char **access;      /* Array of access masks */
    int16 access_count; /* # of entries */

    char **ajoin;       /* Array of autojoin channel */
    int16 ajoin_count;  /* # of entries */

    char **ignore;      /* Array of memo ignore entries */
    int16 ignore_count; /* # of entries */

    MemoInfo memos;

    /* Online-only info: */

    channame_t *channels; /* Array of names of channels currently registered */
    int16 channels_count; /* Number of channels currently registered */
    User **id_users;    /* Users who have identified for this group (array) */
    int id_users_count;
    time_t last_sendauth; /* Time of last SENDAUTH (mail-auth module) */
    int bad_auths;      /* Number of bad AUTH commands for this group */
};

/*************************************************************************/

/* Nickname status flags: */
#define NS_VERBOTEN     0x0002      /* Nick may not be registered or used */
#define NS_NOEXPIRE     0x0004      /* Nick never expires */
#define NS_PERMANENT    0x0006      /* All permanent flags */
/* The following flags are temporary: */
#define NS_KILL_HELD    0x8000      /* Nick is being held after a kill */
#define NS_GUESTED      0x4000      /* SVSNICK has been sent but nick has not
                                     * yet changed.  An enforcer will be
                                     * introduced when it does change. */
#define NS_TEMPORARY    0xC000      /* All temporary flags */


/* Nickname authorization flags: */
#define NA_IDENTIFIED   0x0001      /* User has IDENTIFY'd */
#define NA_IDENT_NOMAIL 0x0002      /* User has identified, but hasn't set an
                                     * E-mail address and one is required
                                     * (in this case we don't allow privs
                                     * except for SET EMAIL on the nick) */
#define NA_RECOGNIZED   0x0004      /* User is recognized as nick owner */


/* Nickgroup setting flags: */
#define NF_KILLPROTECT  0x00000001  /* Kill others who take this nick */
#define NF_SECURE       0x00000002  /* Don't recognize unless IDENTIFY'd */
#define NF_MEMO_HARDMAX 0x00000008  /* Don't allow user to change memo limit */
#define NF_MEMO_SIGNON  0x00000010  /* Notify of memos at signon and un-away */
#define NF_MEMO_RECEIVE 0x00000020  /* Notify of new memos when sent */
#define NF_PRIVATE      0x00000040  /* Don't show in LIST to non-servadmins */
#define NF_HIDE_EMAIL   0x00000080  /* Don't show E-mail in INFO */
#define NF_HIDE_MASK    0x00000100  /* Don't show last seen address in INFO */
#define NF_HIDE_QUIT    0x00000200  /* Don't show last quit message in INFO */
#define NF_KILL_QUICK   0x00000400  /* Kill in 20 seconds instead of 60 */
#define NF_KILL_IMMED   0x00000800  /* Kill immediately instead of in 60 sec */
#define NF_MEMO_FWD     0x00001000  /* Forward memos to E-mail address */
#define NF_MEMO_FWDCOPY 0x00002000  /* Save copy of forwarded memos */
#define NF_SUSPENDED    0x00004000  /* Nickgroup is suspended */
#define NF_NOOP         0x00008000  /* Do not add to channel access lists */

#define NF_ALLFLAGS     0x0000FFFB  /* All flags */

#define NF_NOGROUP      0x00000004  /* Used by database/version4 during load */


/* Nickgroup OperServ privilege levels: */
#define NP_SERVOPER     0x1000
#define NP_SERVADMIN    0x2000


/* Maximum number of nicks we can record for a nick group: */
#define MAX_NICKCOUNT           32767

/* Maximum number of access entries: */
#define MAX_NICK_ACCESS         32767

/* Maximum number of channels we can record: */
#define MAX_CHANNELCOUNT        32767
/* Value indicating "no limit" */
#define CHANMAX_UNLIMITED       -2
/* Value indicating "default limit" (as set in configuration file) */
#define CHANMAX_DEFAULT         -1

/* Value indicating "no time zone set, use local default" */
#define TIMEZONE_DEFAULT        0x7FFF

/* Used when we can't retrieve the NickGroupInfo record for a nick, to
 * prevent the nick from being registered. */
#define NICKGROUPINFO_INVALID   ((NickGroupInfo *)PTR_INVALID)

/* Authentication code reasons: */
#define NICKAUTH_MIN            NICKAUTH_REGISTER
#define NICKAUTH_REGISTER       1       /* Newly registered */
#define NICKAUTH_SET_EMAIL      2       /* E-mail address changed */
#define NICKAUTH_SETAUTH        3       /* SETAUTH command used */
#define NICKAUTH_REAUTH         4       /* REAUTH command used */
#define NICKAUTH_MAX            NICKAUTH_REAUTH

/*************************************************************************/

/* Macros for checking whether a user is recognized or has identified for
 * their nickname, by either NickInfo or User structure. */

#define nick_recognized(ni)   ((ni) && ((ni)->authstat & NA_RECOGNIZED))
#define user_recognized(u)    nick_recognized((u)->ni)

#define nick_identified(ni)   ((ni) && ((ni)->authstat & NA_IDENTIFIED))
#define user_identified(u)    nick_identified((u)->ni)

#define nick_id_or_rec(ni) \
    ((ni) && ((ni)->authstat & (NA_IDENTIFIED|NA_RECOGNIZED)))
#define user_id_or_rec(u)     nick_id_or_rec((u)->ni)

#define nick_ident_nomail(ni) ((ni) && ((ni)->authstat & NA_IDENT_NOMAIL))
#define user_ident_nomail(u)  nick_ident_nomail((u)->ni)


/* Is the given nickgroup unauthenticated?  (Currently, this means that the
 * nickgroup has an authcode set from something other than REAUTH.) */

#define ngi_unauthed(ngi) \
    ((ngi) && (ngi)->authcode && ((ngi)->authreason != NICKAUTH_REAUTH))


/* Does the given user have a valid NickGroupInfo? */

#define valid_ngi(u) ((u) && ((u)->ngi) && ((u)->ngi != NICKGROUPINFO_INVALID))


/* Macro to retrieve the main nick for a given NickGroupInfo. */

#define ngi_mainnick(ngi) ((ngi)->nicks[(ngi)->mainnick])

/*************************************************************************/
/*************************************************************************/

/* Prototypes for exported functions and variables. */

E char *s_NickServ;

E NickInfo *add_nickinfo(NickInfo *ni);
E void del_nickinfo(NickInfo *ni);
E NickInfo *get_nickinfo(const char *nick);
E NickInfo *put_nickinfo(NickInfo *ni);
E NickInfo *first_nickinfo(void);
E NickInfo *next_nickinfo(void);
E NickGroupInfo *add_nickgroupinfo(NickGroupInfo *ngi);
E void del_nickgroupinfo(NickGroupInfo *ngi);
E NickGroupInfo *get_nickgroupinfo(uint32 id);
E NickGroupInfo *put_nickgroupinfo(NickGroupInfo *ngi);
E NickGroupInfo *first_nickgroupinfo(void);
E NickGroupInfo *next_nickgroupinfo(void);

#ifdef STANDALONE_NICKSERV  /* see util.c */
# define E2 static
#else
# define E2 extern
#endif
E2 NickInfo *new_nickinfo(void);
E2 void free_nickinfo(NickInfo *ni);
E2 NickGroupInfo *new_nickgroupinfo(const char *seed);
E2 void free_nickgroupinfo(NickGroupInfo *ngi);
#undef E2

E NickInfo *get_nickinfo_noexpire(const char *nick);

#define get_ngi(ni) _get_ngi((ni), __FILE__, __LINE__)
#define check_ngi(ni) (put_nickgroupinfo(get_ngi((ni))) != NULL)
#define get_ngi_id(id) _get_ngi_id((id), __FILE__, __LINE__)
#define check_ngi_id(id) (put_nickgroupinfo(get_ngi_id((id))) != NULL)
E NickGroupInfo *_get_ngi(const NickInfo *ni, const char *file, int line);
E NickGroupInfo *_get_ngi_id(uint32 id, const char *file, int line);
E int has_identified_nick(const User *u, uint32 group);

/*************************************************************************/

#endif  /* NICKSERV_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
