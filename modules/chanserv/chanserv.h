/* ChanServ-related structures.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CHANSERV_H
#define CHANSERV_H

#ifndef ENCRYPT_H
# include "../../encrypt.h"
#endif
#ifndef NICKSERV_H
# include "../nickserv/nickserv.h"
#endif

/*************************************************************************/

/* Access levels for users. */
typedef struct {
    ChannelInfo *channel;
    uint32 nickgroup;   /* Zero if entry is not in use */
    int16 level;
} ChanAccess;

/* Minimum and maximum valid access levels, inclusive. */
#define ACCLEV_MIN      -999
#define ACCLEV_MAX      999

/* Access level indicating founder access.  May be assumed to be strictly
 * greater than any other level. */
#define ACCLEV_FOUNDER  (ACCLEV_MAX+1)

/* Access level value used for disabled channel privileges. */
#define ACCLEV_INVALID  (ACCLEV_MIN-1)

/* Access levels used to represent SOPs, AOPs, HOPs and VOPs in channel
 * access lists. */
#define ACCLEV_SOP      100
#define ACCLEV_AOP      50
#define ACCLEV_HOP      40
#define ACCLEV_VOP      30
#define ACCLEV_NOP      -1

/* Access level value meaning "use the default level". */
#define ACCLEV_DEFAULT  -9999

/*************************************************************************/

/* AutoKick data. */
typedef struct {
    ChannelInfo *channel;
    char *mask;         /* NULL if not in use */
    char *reason;
    char who[NICKMAX];
    time_t set;
    time_t lastused;
} AutoKick;

/*************************************************************************/

/* Mode lock data. */
typedef struct {
#ifdef CONVERT_DB
    char *on, *off;     /* Strings of mode characters */
#else
    int32 on, off;      /* See channel modes below */
#endif
    int32 limit;        /* 0 if no limit */
    char *key;          /* NULL if no key */
    char *link;         /* +L (Unreal, trircd) */
    char *flood;        /* +f (Unreal, etc.) */
    int32 joindelay;    /* +J (trircd) */
    int32 joinrate1;    /* +j (Bahamut/Unreal) */
    int32 joinrate2;    /* +j (Bahamut/Unreal) */
} ModeLock;

/*************************************************************************/

/* Indices for ci->levels[]: (DO NOT REORDER THESE unless you hack
 * the database/version4 module to deal with any changes) */
#define CA_INVITE       0
#define CA_AKICK        1
#define CA_SET          2       /* but not FOUNDER or PASSWORD */
#define CA_UNBAN        3
#define CA_AUTOOP       4
#define CA_AUTODEOP     5       /* Maximum, not minimum; internal use only */
#define CA_AUTOVOICE    6
#define CA_OPDEOP       7       /* ChanServ commands OP and DEOP */
#define CA_ACCESS_LIST  8
#define CA_CLEAR        9
#define CA_NOJOIN       10      /* Maximum; internal use only */
#define CA_ACCESS_CHANGE 11
#define CA_MEMO         12
#define CA_VOICE        13      /* VOICE/DEVOICE commands */
#define CA_AUTOHALFOP   14
#define CA_HALFOP       15      /* HALFOP/DEHALFOP commands */
#define CA_AUTOPROTECT  16
#define CA_PROTECT      17
/*      CA_AUTOOWNER    18 */   /* No longer used */
#define CA_KICK         19
#define CA_STATUS       20
#define CA_TOPIC        21

#define CA_SIZE         22

/*************************************************************************/

/* Data for a registered channel. */

struct channelinfo_ {
    ChannelInfo *next, *prev;
    int usecount;

    char name[CHANMAX];
    uint32 founder;
    uint32 successor;           /* Who gets the channel if founder nick
                                 * group is dropped or expires */
    Password founderpass;
    char *desc;
    char *url;
    char *email;
    char *entry_message;        /* Notice sent on entering channel */

    time_t time_registered;
    time_t last_used;
    char *last_topic;           /* Last topic on the channel */
    char last_topic_setter[NICKMAX]; /* Who set the last topic */
    time_t last_topic_time;     /* When the last topic was set */

    int32 flags;                /* See below */

    char suspend_who[NICKMAX];  /* Who suspended this channel */
    char *suspend_reason;       /* Reason for suspension */
    time_t suspend_time;        /* Time channel was suspended */
    time_t suspend_expires;     /* Time suspension expires, 0 for no expiry */

    int16 levels[CA_SIZE];      /* Access levels for commands */

    ChanAccess *access;         /* List of authorized users */
    int16 access_count;
    AutoKick *akick;            /* List of users to kickban */
    int16 akick_count;

    ModeLock mlock;             /* Mode lock settings */

    /* Online-only data: */

    Channel *c;                 /* Pointer to channel record (if   *
                                 *    channel is currently in use) */
    int bad_passwords;          /* # of bad passwords since last good one */
};

/* Retain topic even after last person leaves channel */
#define CF_KEEPTOPIC    0x00000001
/* Don't allow non-authorized users to be opped */
#define CF_SECUREOPS    0x00000002
/* Hide channel from ChanServ LIST command */
#define CF_PRIVATE      0x00000004
/* Topic can only be changed by SET TOPIC */
#define CF_TOPICLOCK    0x00000008
/* Those not allowed ops are kickbanned */
#define CF_RESTRICTED   0x00000010
/* Don't auto-deop anyone */
#define CF_LEAVEOPS     0x00000020
/* Don't allow any privileges unless a user is IDENTIFY'd with NickServ */
#define CF_SECURE       0x00000040
/* Don't allow the channel to be registered or used */
#define CF_VERBOTEN     0x00000080
/* Unused; used to be ENCRYPTEDPW */
     /* CF_ENCRYPTEDPW  0x00000100 */
/* Channel does not expire */
#define CF_NOEXPIRE     0x00000200
/* Unused; used to be MEMO_HARDMAX */
     /* CF_MEMO_HARDMAX 0x00000400 */
/* Send notice to channel on use of OP/DEOP */
#define CF_OPNOTICE     0x00000800
/* Enforce +o, +v modes (don't allow deopping) */
#define CF_ENFORCE      0x00001000
/* Hide E-mail address from INFO */
#define CF_HIDE_EMAIL   0x00002000
/* Hide last topic from INFO */
#define CF_HIDE_TOPIC   0x00004000
/* Hide mode lock from INFO */
#define CF_HIDE_MLOCK   0x00008000
/* Channel is suspended */
#define CF_SUSPENDED    0x00010000
/* Limit memo sending to users with the MEMO privilege */
#define CF_MEMO_RESTRICTED  0x00020000

/* All channel flags */
#define CF_ALLFLAGS     0x0001FAFF

/*************************************************************************/

/* Prototypes for exported variables and functions. */

E char *s_ChanServ;
E int32 CSMaxReg;

E ChannelInfo *add_channelinfo(ChannelInfo *ci);
E void del_channelinfo(ChannelInfo *ci);
E ChannelInfo *get_channelinfo(const char *chan);
E ChannelInfo *put_channelinfo(ChannelInfo *ci);
E ChannelInfo *first_channelinfo(void);
E ChannelInfo *next_channelinfo(void);

#ifdef STANDALONE_CHANSERV  /* see util.c */
# define E2 static
#else
# define E2 extern
#endif
E2 ChannelInfo *new_channelinfo(void);
E2 void free_channelinfo(ChannelInfo *ci);
E2 void reset_levels(ChannelInfo *ci);
#undef E2
E int get_ci_level(const ChannelInfo *ci, int what);
E int get_access(const User *user, const ChannelInfo *ci);
E int check_access(const User *user, const ChannelInfo *ci, int what);
E int check_access_if_idented(const User *user, const ChannelInfo *ci,
                              int what);
E int check_access_cmd(const User *user, const ChannelInfo *ci,
                       const char *command, const char *subcommand);
E int check_channel_limit(const NickGroupInfo *ngi, int *max_ret);

/*************************************************************************/

#endif  /* CHANSERV_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
