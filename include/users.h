/* Online user data structure.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef USERS_H
#define USERS_H

/*************************************************************************/

struct user_ {
    User *next, *prev;
    User *snext,*sprev;         /* Next/previous for the user's server
                                 *    (unordered) */

    char nick[NICKMAX];
    NickInfo *ni;               /* NickInfo for this nick */
    NickGroupInfo *ngi;         /* NickGroupInfo for this nick; guaranteed
                                 *    valid if `ni' is non-NULL */
    Server *server;             /* Server user is on */
    char *username;
    char *host;                 /* User's hostname */
    char *realname;
    char *fakehost;             /* Hostname seen by other users */
    char *ipaddr;               /* User's IP address in string form (takes
                                 *    more memory but saves CPU work and
                                 *    gives free compatibility with IPv6) */

    time_t signon;              /* Timestamp sent (from remote server) with
                                 *    user when they connected */
    time_t my_signon;           /* Our local timestamp when the user
                                 *    connected */
    uint32 servicestamp;        /* ID value for user; used in split recovery */
    int32 mode;                 /* UMODE_* user modes */
    int16 flags;                /* UF_* status flags */

    double ignore;              /* Ignore level */
    uint32 lastcmd;             /* Time of last command, from time_msec() */
    time_t lastcmd_s;           /* Same from time(), to check rollover */

    int16 bad_pw_count;         /* # of invalid password attempts */
    time_t bad_pw_time;         /* Time of last invalid password */
    time_t lastmemosend;        /* Last time MS SEND command used */
    time_t lastmemofwd;         /* Last time MS FORWARD command used */
    time_t lastnickreg;         /* Last time NS REGISTER command used */
    time_t last_nick_set_email; /* Last time NS SET EMAIL command used */

    uint32 *id_nicks;
    int id_nicks_count;

    struct u_chanlist { /* Channels user has joined */
        struct u_chanlist *next, *prev;
        Channel *chan;
    } *chans;

    struct u_chaninfolist { /* Channels user has identified for */
        struct u_chaninfolist *next, *prev;
        char chan[CHANMAX];
    } *id_chans;
};


/* Status flags: */
#define UF_SERVROOT     0x0001  /* User has Services root privileges */

/*************************************************************************/

#endif /* USERS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
