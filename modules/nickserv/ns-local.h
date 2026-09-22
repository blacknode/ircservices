/* Include file for data local to the NickServ module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef NS_LOCAL_H
#define NS_LOCAL_H

/*************************************************************************/
/*************************************************************************/

/* User-configurable settings: */


/* Maximum number of tries to randomly select a new NickGroupInfo ID before
 * giving up.  Assuming a sufficiently random random-number generator and a
 * database of one million nicknames, the default setting of 1000 gives
 * odds of approximately 10^3600 to 1 against an accidental failure. */

#define NEWNICKGROUP_TRIES      1000


/* Number of DROPEMAIL commands to buffer for DROPEMAIL-CONFIRM. */

#define DROPEMAIL_BUFSIZE       4

/*************************************************************************/
/*************************************************************************/

#define TO_COLLIDE   0                  /* Collide the user with this nick */
#define TO_RELEASE   1                  /* Release a collided nick */
#define TO_SEND_433  2                  /* Send a 433 numeric */

/*************************************************************************/

/* External declarations: */


/**** collide.c ****/

E void introduce_enforcer(NickInfo *ni);
E void collide_nick(NickInfo *ni, int from_timeout);
E void release_nick(NickInfo *ni, int from_timeout);
E void add_ns_timeout(NickInfo *ni, int type, time_t delay);
E void rem_ns_timeout(NickInfo *ni, int type, int del_to);
E int init_collide(void);
E void exit_collide(void);


/**** main.c ****/

E int cb_reglink_check;

E char * s_NickServ;
E int32  NSRegEmailMax;
E int    NSRequireEmail;
E int    NSRegDenyIfSuspended;
E time_t NSRegDelay;
E time_t NSInitialRegDelay;
E time_t NSSetEmailDelay;
E int32  NSDefFlags;
E time_t NSExpire;
E time_t NSExpireWarning;
E int    NSShowPassword;
E char * NSEnforcerUser;
E char * NSEnforcerHost;
E int    NSForceNickChange;
E time_t NSReleaseTimeout;
E int    NSAllowKillImmed;
E int    NSListOpersOnly;
E int    NSSecureAdmins;
E time_t NSSuspendExpire;
E time_t NSSuspendGrace;


/**** set.c ****/

/* Avoid conflicts with chanserv/set.c */
#define do_set do_set_ns
#define do_unset do_unset_ns
#define init_set init_set_ns
#define exit_set exit_set_ns

E void do_set(User *u);
E void do_unset(User *u);
E int init_set(void);
E void exit_set(void);


/**** util.c ****/

/* Avoid conflicts with chanserv/util.c */
#define init_util init_util_ns
#define exit_util exit_util_ns

E int reglink_check(User *u, const char *nick, char *password, char *email);
E void update_userinfo(const User *u);
E int validate_user(User *u);
E void cancel_user(User *u);
E void set_identified(User *u);

E NickInfo *makenick(const char *nick, NickGroupInfo **nickgroup_ret);
E int delnick(NickInfo *ni);
E int delgroup(NickGroupInfo *ngi);
E int drop_nickgroup(NickGroupInfo *ngi, const User *u, const char *dropemail);
E void suspend_nick(NickGroupInfo *ngi, const char *reason,
                    const char *who, const time_t expires);
E void unsuspend_nick(NickGroupInfo *ngi, int set_time);

E int nick_check_password(User *u, NickInfo *ni, const char *password,
                          const char *command, int failure_msg);
E int count_nicks_with_email(const char *email);

E int init_util(void);
E void exit_util(void);


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
