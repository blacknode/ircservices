/* Online channel data structure.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CHANNELS_H
#define CHANNELS_H

/*************************************************************************/

struct channel_ {
    Channel *next, *prev;

    char name[CHANMAX];
    ChannelInfo *ci;                    /* Corresponding ChannelInfo */
    time_t creation_time;               /* When channel was created */

    char *topic;
    char topic_setter[NICKMAX];         /* Who set the topic */
    time_t topic_time;                  /* When topic was set */

    int32 mode;                         /* CMODE_* (binary) channel modes */
    int32 limit;                        /* 0 if none */
    char *key;                          /* NULL if none */
    char *link;                         /* +L (Unreal, trircd) */
    char *flood;                        /* +f (Unreal, etc.) */
    int32 joindelay;                    /* +J (trircd) */
    int32 joinrate1, joinrate2;         /* +j (Bahamut) */

    char **bans;
    int32 bans_count;
    char **excepts;
    int32 excepts_count;
    char **invites;
    int32 invites_count;

    struct c_userlist {
        struct c_userlist *next, *prev;
        User *user;
        int32 mode;                     /* CUMODE_* modes (chanop, voice) */
        int16 flags;                    /* CUFLAG_* flags (below) */
    } *users;

    time_t server_modetime;             /* Time of last server MODE */
    time_t chanserv_modetime;           /* Time of last check_modes() */
    int16 server_modecount;             /* Number of server MODEs this second*/
    int16 chanserv_modecount;           /* Number of check_mode()'s this sec */
    int16 bouncy_modes;                 /* Did we fail to set modes here? */
};

/* Set by ChanServ if it deops a user on joining a channel */
#define CUFLAG_DEOPPED          0x0001

/*************************************************************************/

#endif

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
