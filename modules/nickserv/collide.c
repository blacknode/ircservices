/* Routines related to nickname colliding.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "language.h"
#include "timeout.h"

#include "nickserv.h"
#include "ns-local.h"

/*************************************************************************/

static int cb_collide = -1;


/* Structure used for collide/release/433 timeouts */

static struct my_timeout {
    struct my_timeout *next, *prev;
    NickInfo *ni;
    Timeout *to;
    int type;
} *my_timeouts;


static void timeout_collide(Timeout *t);
static void timeout_release(Timeout *t);
static void timeout_send_433(Timeout *t);

/*************************************************************************/
/*************************************************************************/

/* Collide a nick. */

void collide_nick(NickInfo *ni, int from_timeout)
{
    if (!ni->user)
        return;
    if (!from_timeout) {
        rem_ns_timeout(ni, TO_COLLIDE, 1);
        rem_ns_timeout(ni, TO_SEND_433, 1);
    }
    if (call_callback_1(cb_collide, ni->user) > 0)
        return;
    if (NSForceNickChange) {
        char *guestnick = make_guest_nick();
        notice_lang(s_NickServ, ni->user, FORCENICKCHANGE_NOW, guestnick);
        send_nickchange_remote(ni->nick, guestnick);
        ni->status |= NS_GUESTED;
        return;
    } else {
        notice_lang(s_NickServ, ni->user, DISCONNECT_NOW);
        kill_user(s_NickServ, ni->nick, "Nick kill enforced");
        introduce_enforcer(ni);
    }
}

/*************************************************************************/

/* Introduce an enforcer for a given nick. */

void introduce_enforcer(NickInfo *ni)
{
    char realname[NICKMAX+16]; /*Long enough for s_NickServ + " Enforcement"*/

    snprintf(realname, sizeof(realname), "%s Enforcement", s_NickServ);
    send_nick(ni->nick, NSEnforcerUser, NSEnforcerHost, ServerName,
              realname, enforcer_modes);
    ni->status |= NS_KILL_HELD;
    add_ns_timeout(ni, TO_RELEASE, NSReleaseTimeout);
}

/*************************************************************************/

/* Release hold on a nick. */

void release_nick(NickInfo *ni, int from_timeout)
{
    if (!from_timeout)
        rem_ns_timeout(ni, TO_RELEASE, 1);
    send_cmd(ni->nick, "QUIT");
    ni->status &= ~NS_KILL_HELD;
}

/*************************************************************************/

/* Add a collide/release/433 timeout. */

void add_ns_timeout(NickInfo *ni, int type, time_t delay)
{
    Timeout *to;
    struct my_timeout *t;
    void (*timeout_routine)(Timeout *);

    if (!ni) {
        log("BUG: NULL NickInfo in add_ns_timeout (type=%d delay=%ld)",
            type, (long)delay);
        return;
    }
    if (type == TO_COLLIDE)
        timeout_routine = timeout_collide;
    else if (type == TO_RELEASE)
        timeout_routine = timeout_release;
    else if (type == TO_SEND_433)
        timeout_routine = timeout_send_433;
    else {
        module_log("BUG: unknown timeout type %d!  ni=%p (%s), delay=%ld",
                   type, ni, ni->nick, (long)delay);
        return;
    }
    to = add_timeout(delay, timeout_routine, 0);
    to->data = ni;
    t = smalloc(sizeof(*t));
    LIST_INSERT(t, my_timeouts);
    t->ni = ni;
    t->to = to;
    t->type = type;
    ni->usecount++;  /* make sure it isn't deleted when we're not looking */
}

/*************************************************************************/

/* Remove a collide/release timeout from our private list.  If del_to is
 * nonzero, also delete the associated timeout.  If type == -1, delete
 * timeouts of all types.  If ni == NULL, delete all timeouts of the given
 * type(s).
 */

void rem_ns_timeout(NickInfo *ni, int type, int del_to)
{
    struct my_timeout *t, *t2;

    LIST_FOREACH_SAFE (t, my_timeouts, t2) {
        if ((!ni || t->ni == ni) && (type < 0 || t->type == type)) {
            LIST_REMOVE(t, my_timeouts);
            if (del_to)
                del_timeout(t->to);
            put_nickinfo(t->ni);  /* cancel usecount++ above */
            free(t);
        }
    }
}

/*************************************************************************/
/*************************************************************************/

/* Collide a nick on timeout. */

static void timeout_collide(Timeout *t)
{
    NickInfo *ni = t->data;
    int idented = 0;

    if (ni) {
        if (ni->nickgroup != 0) {
            NickGroupInfo *ngi = get_ngi(ni);
            idented = (ngi && (nick_identified(ni) || nick_ident_nomail(ni)));
            put_nickgroupinfo(ngi);
        }
    } else {
        log("BUG: NULL NickInfo in timeout_collide");
        return;
    }
    /* If they identified or don't exist anymore, don't kill them. */
    if (!(idented || !ni->user || ni->user->my_signon > t->settime)) {
        /* The RELEASE timeout will always add to the beginning of the
         * list, so we won't see it.  Which is fine because it can't be
         * triggered yet anyway. */
        collide_nick(ni, 1);
    }
    rem_ns_timeout(ni, TO_COLLIDE, 0);
}

/*************************************************************************/

/* Release a nick on timeout. */

static void timeout_release(Timeout *t)
{
    NickInfo *ni = t->data;

    if (!ni) {
        log("BUG: NULL NickInfo in timeout_release");
        return;
    }
    release_nick(ni, 1);
    rem_ns_timeout(ni, TO_RELEASE, 0);
}

/*************************************************************************/

/* Send a 433 (nick in use) numeric to the given user. */

static void timeout_send_433(Timeout *t)
{
    NickInfo *ni = t->data;
    User *u;

    if (!ni) {
        log("BUG: NULL NickInfo in timeout_send_433");
        return;
    }
    /* If they identified or don't exist anymore, don't send the 433. */
    if (!((nick_identified(ni) || nick_ident_nomail(ni))
          || !(u = get_user(ni->nick))
          || u->my_signon > t->settime)
    ) {
        if (ni->status & NS_VERBOTEN) {
            send_cmd(ServerName, "433 %s %s :Nickname may not be used",
                     ni->nick, ni->nick);
        } else {
            send_cmd(ServerName, "433 %s %s :Nickname is registered to"
                     " someone else", ni->nick, ni->nick);
        }
    }
    rem_ns_timeout(ni, TO_SEND_433, 0);
}

/*************************************************************************/
/*************************************************************************/

int init_collide(void)
{
    cb_collide = register_callback("collide");
    if (cb_collide < 0) {
        module_log("collide: Unable to register callbacks");
        exit_collide();
        return 0;
    }
    return 1;
}

/*************************************************************************/

void exit_collide()
{
    rem_ns_timeout(NULL, -1, 1);
    unregister_callback(cb_collide);
}

/*************************************************************************/

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
