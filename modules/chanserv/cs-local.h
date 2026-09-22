/* Include file for data local to the ChanServ module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CS_LOCAL_H
#define CS_LOCAL_H

/*************************************************************************/
/*************************************************************************/

/* Maximum number of parameters to allow to a SET MLOCK command.  Anything
 * over this will be silently discarded. */

#define MAX_MLOCK_PARAMS        256     /* impossible at 512 bytes/line */

/*************************************************************************/

/* Data for a channel option. */

typedef struct {
    const char *name;
    int32 flag;
    int namestr;  /* If -1, will be ignored by cs_flags_to_string() */
    int onstr, offstr, syntaxstr;
} ChanOpt;

/*************************************************************************/

/* Return values for access list add/delete/list functions.  Success > 0,
 * failure < 0.  Do not use zero as a return value.
 */
#define RET_ADDED       1
#define RET_CHANGED     2
#define RET_UNCHANGED   3
#define RET_DELETED     4
#define RET_LISTED      5
#define RET_PERMISSION  -1
#define RET_NOSUCHNICK  -2
#define RET_NICKFORBID  -3
#define RET_NOOP        -4
#define RET_LISTFULL    -5
#define RET_NOENTRY     -6
#define RET_INTERR      -99

/*************************************************************************/

/* External declarations: */


/**** access.c ****/

E int16 def_levels[CA_SIZE];

E int get_access(const User *user, const ChannelInfo *ci);
E int check_access_cumode(const User *user, const ChannelInfo *ci,
                          int32 newmodes, int32 changemask);
E int access_add(ChannelInfo *ci, const char *nick, int level, int uacc);
E int access_del(ChannelInfo *ci, const char *nick, int uacc);
E int init_access(void);
E void exit_access(void);


/**** check.c ****/

E void check_modes(Channel *c);
E void check_chan_user_modes(const char *source, struct c_userlist *u,
                             Channel *c, int32 oldmodes);
E int check_kick(User *user, const char *chan, int on_join);
E int check_topiclock(Channel *c, time_t topic_time);
E int init_check(void);
E void exit_check(void);


/**** main.c ****/

E int    CSRegisteredOnly;
E int32  CSMaxReg;
E int32  CSDefFlags;
E time_t CSExpire;
E int    CSShowPassword;
E int32  CSAccessMax;
E int32  CSAutokickMax;
E time_t CSInhabit;
E time_t CSRestrictDelay;
E int    CSListOpersOnly;
E time_t CSSuspendExpire;
E time_t CSSuspendGrace;
E int    CSForbidShortChannel;
E int    CSSkipModeRCheck;
E ChanOpt chanopts[];


/**** set.c ****/

/* Avoid conflicts with nickserv/set.c */
#define do_set do_set_cs
#define do_unset do_unset_cs
#define init_set init_set_cs
#define exit_set exit_set_cs

E void do_set(User *u);
E void do_unset(User *u);
E int init_set(void);
E void exit_set(void);


/**** util.c ****/

/* Avoid conflicts with nickserv/util.c */
#define init_util init_util_cs
#define exit_util exit_util_cs

E ChannelInfo *makechan(const char *chan);
E int delchan(ChannelInfo *ci);
E void count_chan(const ChannelInfo *ci);
E void uncount_chan(const ChannelInfo *ci);
E int is_founder(const User *user, const ChannelInfo *ci);
E int is_identified(const User *user, const ChannelInfo *ci);
E void restore_topic(Channel *c);
E void record_topic(ChannelInfo *ci, const char *topic,
                    const char *setter, time_t topic_time);
E void suspend_channel(ChannelInfo *ci, const char *reason,
                       const char *who, const time_t expires);
E void unsuspend_channel(ChannelInfo *ci, int set_time);
E void chan_bad_password(User *u, ChannelInfo *ci);
E ChanOpt *chanopt_from_name(const char *optname);
E char *chanopts_to_string(const ChannelInfo *ci, const NickGroupInfo *ngi);


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
