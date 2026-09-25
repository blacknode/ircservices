/* Nickname mail address authentication module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "commands.h"
#include "language.h"
#include "db.h"
#include "timeout.h"
#include "modules/mail/mail.h"
#include "modules/operserv/operserv.h"

#include "modules/nickserv/nickserv.h"
#include "modules/nickserv/ns-local.h"

/*************************************************************************/

/*
 * This module provides the ability to verify the accuracy of E-mail
 * addresses associated with nicknames.  Upon registering a new nickname
 * or changing the E-mail address of a nickname, a random authentication
 * code is generated and mailed to the user, with instructions to use the
 * AUTH command (provided by this module) to verify the nick registration
 * or address change.  Until this is done, the nick may not be identified
 * for, essentially preventing use of the nick.
 *
 * The authentication code generated is a random 9-digit number,
 * lower-bounded to 100,000,000 to avoid potential difficulties with
 * leading zeroes.
 */

/*************************************************************************/
/*************************** Local variables *****************************/
/*************************************************************************/

static Module *module_nickserv;

static time_t NSNoAuthExpire = 0;
static time_t NSSendauthDelay = 0;

static Event* authed_event;


static void do_auth(User *u);
static void do_sendauth(User *u);
static void do_reauth(User *u);
static void do_restoremail(User *u);
static void do_setauth(User *u);
static void do_getauth(User *u);
static void do_clearauth(User *u);

static Command commands[] = {
    { "AUTH",     do_auth,     NULL,              NICK_HELP_AUTH,     -1,-1 },
    { "SENDAUTH", do_sendauth, NULL,              NICK_HELP_SENDAUTH, -1,-1 },
    { "REAUTH",   do_reauth,   NULL,              NICK_HELP_REAUTH,   -1,-1 },
    { "RESTOREMAIL", do_restoremail, NULL,        NICK_HELP_RESTOREMAIL,-1,-1},
    { "SETAUTH",  do_setauth,  is_services_admin,
                -1,-1,NICK_OPER_HELP_SETAUTH   },
    { "GETAUTH",  do_getauth,  is_services_admin,
                -1,-1,NICK_OPER_HELP_GETAUTH   },
    { "CLEARAUTH",do_clearauth,is_services_admin,
                -1,-1,NICK_OPER_HELP_CLEARAUTH },
    { NULL }
};

/*************************************************************************/
/**************************** Local routines *****************************/
/*************************************************************************/

/* Create an authentication code for the given nickname group and store it
 * in the NickGroupInfo structure.  `reason' is the NICKAUTH_* value to
 * store in the `authreason' field.
 */

static void make_auth(NickGroupInfo *ngi, int16 reason)
{
    ngi->authcode = rand()%900000000 + 100000000;
    ngi->authset = time(NULL);
    ngi->authreason = reason;
}

/*************************************************************************/

/* Clear any authentication code from the given nickname group, along with
 * all associated data.
 */

static void clear_auth(NickGroupInfo *ngi)
{
    ngi->authcode = 0;
    ngi->authset = 0;
    ngi->authreason = 0;
    ngi->bad_auths = 0;
    free(ngi->last_email);
    ngi->last_email = NULL;
}

/*************************************************************************/
/*************************************************************************/

/* Send mail to a nick's E-mail address containing the authentication code.
 * `u' is the user causing the mail to be sent; `ngi' is the nickgroup and
 * `nick' is the nickname of the user whose authcode should be sent; `what'
 * is one of the IS_* constants defined below.  Returns 1 on success, 0 on
 * error.
 */

#define send_auth(u,ngi,nick,what) send_auth_(u,ngi,nick,what,__LINE__)

#define IS_REGISTRATION         NICK_AUTH_MAIL_TEXT_REG
#define IS_EMAIL_CHANGE         NICK_AUTH_MAIL_TEXT_EMAIL
#define IS_SENDAUTH             NICK_AUTH_MAIL_TEXT_SENDAUTH
#define IS_REAUTH               NICK_AUTH_MAIL_TEXT_REAUTH
#define IS_SETAUTH              -1

/*************************************************************************/

/* Data list for mail completion callbacks. */

struct sendauth_data {
    struct sendauth_data *next, *prev;
    User *u;
    char nick[NICKMAX];
    char email[BUFSIZE];
    int what;
};
static struct sendauth_data *sendauth_list;
static void send_auth_callback(int status, void *data);

/*************************************************************************/

/* Actual routine to send mail. */

static int send_auth_(User *u, NickGroupInfo *ngi, const char *nick,
                     int what, int caller_line)
{
    char subject[BUFSIZE], body[BUFSIZE];
    const char *text;
    struct sendauth_data *sad;

    if (!u || !ngi || !ngi->email) {
        module_log("send_auth() with %s! (called from line %d)",
                   !u ? "null User"
                      : !ngi ? "null NickGroupInfo"
                             : "NickGroupInfo with no E-mail",
                   caller_line);
        return 0;
    }
    text = what<0 ? "" : getstring(ngi, what);
    snprintf(subject, sizeof(subject), getstring(ngi,NICK_AUTH_MAIL_SUBJECT),
             nick);
    if (what == IS_SETAUTH) {
        snprintf(body, sizeof(body),
                 getstring(ngi,NICK_AUTH_MAIL_BODY_SETAUTH),
                 nick, ngi->authcode, nickserv_service.nick, nickserv_service.nick, ngi->authcode);
    } else {
        snprintf(body, sizeof(body), getstring(ngi,NICK_AUTH_MAIL_BODY),
                 nick, ngi->authcode, nickserv_service.nick, nickserv_service.nick, ngi->authcode,
                 nickserv_service.nick, text, u->username, u->host);
    }
    sad = smalloc(sizeof(*sad));
    LIST_INSERT(sad, sendauth_list);
    sad->u = u;
    strbcpy(sad->nick, nick);
    strbcpy(sad->email, ngi->email);
    sad->what = what;
    sendmail(ngi->email, subject, body, getstring(ngi,LANG_CHARSET),
             send_auth_callback, sad);
    return 1;
}

/*************************************************************************/

/* Completion callback for sendmail().  Clears the SENDAUTH timer on
 * SENDAUTH message failure, and sends a notice to the user (if still
 * online) that the mail was or was not sent.
 */

static void send_auth_callback(int status, void *data)
{
    struct sendauth_data *sad = data;

    if (sad->what == IS_SENDAUTH && status != MAIL_STATUS_SENT) {
        NickInfo *ni = get_nickinfo(sad->nick);
        if (ni) {
            NickGroupInfo *ngi = get_ngi(ni);
            if (ngi)
                ngi->last_sendauth = 0;
            put_nickgroupinfo(ngi);
        }
        put_nickinfo(ni);
    }
    if (sad->u) {  /* If the user has logged off this will be NULL */
        switch (sad->what) {
          case IS_REGISTRATION:
          case IS_EMAIL_CHANGE:
          case IS_SENDAUTH:
          case IS_REAUTH:
            if (status != MAIL_STATUS_SENT) {
                if (status == MAIL_STATUS_NORSRC)
                    notice_lang(nickserv_service.nick, sad->u, NICK_SENDAUTH_NORESOURCES);
                else
                    notice_lang(nickserv_service.nick, sad->u, NICK_SENDAUTH_ERROR);
            }
            break;
          case IS_SETAUTH:
            switch (status) {
              case MAIL_STATUS_SENT:
                /* No message */
                break;
              case MAIL_STATUS_REFUSED:
                notice_lang(nickserv_service.nick, sad->u, NICK_SETAUTH_SEND_REFUSED,
                            sad->email);
                break;
              case MAIL_STATUS_TIMEOUT:
                notice_lang(nickserv_service.nick, sad->u, NICK_SETAUTH_SEND_REFUSED,
                            sad->email);
                break;
              case MAIL_STATUS_NORSRC:
                notice_lang(nickserv_service.nick, sad->u, NICK_SETAUTH_SEND_NORESOURCES,
                            sad->email);
                break;
              default:
                notice_lang(nickserv_service.nick, sad->u, NICK_SETAUTH_SEND_ERROR,
                            sad->email);
                break;
            }
            break;
        }  /* switch (sad->what) */
    }  /* if (sad->u) */
    LIST_REMOVE(sad, sendauth_list);
    free(sad);
}

/*************************************************************************/

/* Routine to clear `u' field from callback data entries for a quitting
 * user.
 */

static int sendauth_userdel(User *user, const char *reason, int is_kill)
{
    struct sendauth_data *sad;

    LIST_FOREACH (sad, sendauth_list) {
        if (sad->u == user)
            sad->u = NULL;
    }
    return 0;
}

/*************************************************************************/
/*************************** Command handlers ****************************/
/*************************************************************************/

/* Handle the AUTH command. */

static void do_auth(User *u)
{
    char *s = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (!s || !*s) {
        syntax_error(nickserv_service.nick, u, "AUTH", NICK_AUTH_SYNTAX);
    } else if (readonly) {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_DISABLED);
    } else if (!(ni = u->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, u->nick);
    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (!ngi->authcode) {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_NOT_NEEDED);
    } else if (!ngi->email) {
        module_log("BUG: do_auth() for %s[%u]: authcode set but no email!",
                   ni->nick, ngi->id);
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else {
        int32 code;
        const char *what = "(unknown)";
        int16 authreason = ngi->authreason;

        code = (int32)atolsafe(s, 0, 0x7FFFFFFF);
        if (code != ngi->authcode) {
            char buf[BUFSIZE];
            snprintf(buf, sizeof(buf), "AUTH for %s", ni->nick);
            notice_lang(nickserv_service.nick, u, NICK_AUTH_FAILED);
            if (bad_password(NULL, u, buf) == 1)
                notice_lang(nickserv_service.nick, u, PASSWORD_WARNING_FOR_AUTH);
            ngi->bad_auths++;
            if (BadPassWarning && ngi->bad_auths >= BadPassWarning) {
                wallops(nickserv_service.nick, "\2Warning:\2 Repeated bad AUTH attempts"
                        " for nick %s", ni->nick);
            }
            return;
        }
        clear_auth(ngi);
        set_identified(u);
        switch (authreason) {
          case NICKAUTH_REGISTER:
            notice_lang(nickserv_service.nick, u, NICK_AUTH_SUCCEEDED_REGISTER);
            what = "REGISTER";
            break;
          case NICKAUTH_SET_EMAIL:
            notice_lang(nickserv_service.nick, u, NICK_AUTH_SUCCEEDED_SET_EMAIL);
            what = "SET EMAIL";
            break;
          case NICKAUTH_REAUTH:
            notice_lang(nickserv_service.nick, u, NICK_AUTH_SUCCEEDED_REAUTH);
            what = "REAUTH";
            break;
          case NICKAUTH_SETAUTH:
            what = "SETAUTH";
            /* fall through */
          default:
            /* "you may now continue using your nick", good for a default */
            notice_lang(nickserv_service.nick, u, NICK_AUTH_SUCCEEDED_SETAUTH);
            break;
        }
        module_log("%s@%s authenticated %s for %s", u->username, u->host,
                   what, ni->nick);
        event_emit(authed_event, u, ni, ngi, authreason);
    }
}

/*************************************************************************/

/* Handle the SENDAUTH command.  To prevent abuse the command is limited to
 * one use per nick per day (reset when Services starts up).
 */

static void do_sendauth(User *u)
{
    char *s = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi;
    time_t now = time(NULL);

    if (s) {
        syntax_error(nickserv_service.nick, u, "SENDAUTH", NICK_SENDAUTH_SYNTAX);
    } else if (!(ni = u->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, u->nick);
    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (!ngi->authcode) {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_NOT_NEEDED);
    } else if (ngi->last_sendauth
               && now - ngi->last_sendauth < NSSendauthDelay) {
        notice_lang(nickserv_service.nick, u, NICK_SENDAUTH_TOO_SOON,
                    maketime(ngi,NSSendauthDelay-(now-ngi->last_sendauth),0));
    } else if (!ngi->email) {
        module_log("BUG: do_sendauth() for %s[%u]: authcode set but no email!",
                   ni->nick, ngi->id);
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_SENDING, ngi->email);
        if (!send_auth(u, ngi, ni->nick, IS_SENDAUTH)) {
            module_log("Valid SENDAUTH by %s!%s@%s failed",
                       u->nick, u->username, u->host);
            notice_lang(nickserv_service.nick, u, NICK_SENDAUTH_ERROR);
        } else {
            ngi->last_sendauth = time(NULL);
        }
    }
}

/*************************************************************************/

/* Handle the REAUTH command. */

static void do_reauth(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "REAUTH", NICK_REAUTH_SYNTAX);
    } else if (!(ni = u->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, u->nick);
    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (ngi->authcode) {
        notice_lang(nickserv_service.nick, u, NICK_REAUTH_HAVE_AUTHCODE);
    } else if (!ngi->email) {
        notice_lang(nickserv_service.nick, u, NICK_REAUTH_NO_EMAIL);
    } else {
        make_auth(ngi, NICKAUTH_REAUTH);
        notice_lang(nickserv_service.nick, u, NICK_REAUTH_AUTHCODE_SET);
        if (!send_auth(u, ngi, ni->nick, IS_REAUTH)) {
            module_log("send_auth() failed for REAUTH by %s", u->nick);
            notice_lang(nickserv_service.nick, u, NICK_SENDAUTH_ERROR);
        }
        ngi->last_sendauth = 0;
    }
}

/*************************************************************************/

/* Handle the RESTOREMAIL command. */

static void do_restoremail(User *u)
{
    char *password = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi;

    if (!password) {
        syntax_error(nickserv_service.nick, u, "RESTOREMAIL", NICK_RESTOREMAIL_SYNTAX);
    } else if (!(ni = u->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, u->nick);
    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (!ngi->last_email || !ngi->authcode
               || ngi->authreason != NICKAUTH_SET_EMAIL) {
        notice_lang(nickserv_service.nick, u, NICK_RESTOREMAIL_NOT_NOW);
    } else if (!nick_check_password(u, u->ni, password, "IDENTIFY",
                                    INTERNAL_ERROR)) {
        /* Nothing, error message has already been sent */
    } else {
        /* Restore previous E-mail address _before_ calling clear_auth() */
        free(ngi->email);
        ngi->email = ngi->last_email;
        ngi->last_email = NULL;
        clear_auth(ngi);
        set_identified(u);
        notice_lang(nickserv_service.nick, u, NICK_RESTOREMAIL_DONE, ngi->email);
    }
}

/*************************************************************************/

/* Handle the SETAUTH command (Services admins only). */

static void do_setauth(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(nickserv_service.nick, u, "SETAUTH", NICK_SETAUTH_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (ngi->authcode && ngi->authreason != NICKAUTH_REAUTH) {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_HAS_AUTHCODE, ni->nick);
    } else if (!ngi->email) {
        notice_lang(nickserv_service.nick, u, NICK_SETAUTH_NO_EMAIL, ni->nick);
    } else {
        int i;
        if (ngi->authcode) {
            /* Convert reason to SETAUTH */
            ngi->authreason = NICKAUTH_SETAUTH;
        } else {
            make_auth(ngi, NICKAUTH_SETAUTH);
        }
        notice_lang(nickserv_service.nick, u, NICK_SETAUTH_AUTHCODE_SET,
                    ngi->authcode, ni->nick);
        if (!send_auth(u, ngi, ni->nick, IS_SETAUTH)) {
            module_log("send_auth() failed for SETAUTH on %s by %s",
                       nick, u->nick);
            notice_lang(nickserv_service.nick, u, NICK_SETAUTH_SEND_ERROR, ngi->email);
        }
        ngi->last_sendauth = 0;
        ARRAY_FOREACH (i, ngi->nicks) {
            NickInfo *ni2 = get_nickinfo(ngi->nicks[i]);
            if (!ni2)
                continue;
            ni2->authstat &= ~NA_IDENTIFIED;
            if (ni2->user) {
                notice_lang(nickserv_service.nick, ni2->user, NICK_SETAUTH_USER_NOTICE,
                            ngi->email, nickserv_service.nick);
            }
            put_nickinfo(ni2);
        }
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

/* Handle the GETAUTH command (Services admins only). */

static void do_getauth(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(nickserv_service.nick, u, "GETAUTH", NICK_GETAUTH_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (!ngi->authcode) {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_NO_AUTHCODE, ni->nick);
    } else {
        if (WallAdminPrivs) {
            wallops(nickserv_service.nick, "\2%s\2 used GETAUTH on \2%s\2",
                    u->nick, nick);
        }
        notice_lang(nickserv_service.nick, u, NICK_GETAUTH_AUTHCODE_IS,
                    ni->nick, ngi->authcode);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

/* Handle the CLEARAUTH command (Services admins only). */

static void do_clearauth(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(nickserv_service.nick, u, "CLEARAUTH", NICK_CLEARAUTH_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (!ngi->authcode) {
        notice_lang(nickserv_service.nick, u, NICK_AUTH_NO_AUTHCODE, ni->nick);
    } else {
        if (WallAdminPrivs) {
            wallops(nickserv_service.nick, "\2%s\2 used CLEARAUTH on \2%s\2",
                    u->nick, nick);
        }
        clear_auth(ngi);
        notice_lang(nickserv_service.nick, u, NICK_CLEARAUTH_CLEARED, ni->nick);
        if (readonly)
            notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/
/*************************** Callback routines ***************************/
/*************************************************************************/

/* Nick-registration handler: clear IDENTIFIED flag, set nick flags
 * appropriately (no kill, secure), send authcode mail.
 */

static int do_registered(User *u, NickInfo *ni, NickGroupInfo *ngi,
                         int *replied)
{
    ni->authstat &= ~NA_IDENTIFIED;
    ngi->last_sendauth = 0;
    make_auth(ngi, NICKAUTH_REGISTER);
    if (!*replied) {
        notice_lang(nickserv_service.nick, u, NICK_REGISTERED, u->nick);
        *replied = 1;
    }
    notice_lang(nickserv_service.nick, u, NICK_AUTH_SENDING, ngi->email);
    notice_lang(nickserv_service.nick, u, NICK_AUTH_FOR_REGISTER, nickserv_service.nick);
    if (!send_auth(u, ngi, ni->nick, IS_REGISTRATION)) {
        module_log("send_auth() failed for registration (%s)", u->nick);
        notice_lang(nickserv_service.nick, u, NICK_SENDAUTH_ERROR);
    }
    return 0;
}

/*************************************************************************/

/* SET EMAIL handler: clear IDENTIFIED flag, send authcode mail. */

static int do_set_email(User *u, NickGroupInfo *ngi, const char *last_email)
{
    /* Note: we assume here that if it's not a servadmin doing the change,
     * it must be the user him/herself (and thus use u->nick and u->ni). */
    if (ngi->email && !is_services_admin(u)) {
        u->ni->authstat &= ~NA_IDENTIFIED;
        if (last_email)
            ngi->last_email = sstrdup(last_email);
        ngi->last_sendauth = 0;
        make_auth(ngi, NICKAUTH_SET_EMAIL);
        notice_lang(nickserv_service.nick, u, NICK_AUTH_SENDING, ngi->email);
        notice_lang(nickserv_service.nick, u, NICK_AUTH_FOR_SET_EMAIL, nickserv_service.nick);
        if (!send_auth(u, ngi, u->nick, IS_EMAIL_CHANGE)) {
            module_log("send_auth() failed for E-mail change (%s)", u->nick);
            notice_lang(nickserv_service.nick, u, NICK_SENDAUTH_ERROR);
        }
    }
    return 0;
}

/*************************************************************************/

/* IDENTIFY check: do not allow identification if nick not authenticated
 * (except for REAUTH). */

static int do_identify_check(const User *u, const char *pass)
{
    if (u->ngi && u->ngi != NICKGROUPINFO_INVALID && ngi_unauthed(u->ngi)) {
        notice_lang(nickserv_service.nick, u, NICK_PLEASE_AUTH, u->ngi->email);
        notice_lang(nickserv_service.nick, u, MORE_INFO, nickserv_service.nick, "AUTH");
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Expiration check handler. */

static int do_check_expire(NickInfo *ni, NickGroupInfo *ngi)
{
    time_t now = time(NULL);

    if (!NSNoAuthExpire)
        return 0;
    if (ngi && ngi->authcode && now - ngi->authset >= NSNoAuthExpire) {
        if (ngi->authreason == NICKAUTH_REAUTH) {
            clear_auth(ngi);
        } else if (ngi->authreason == NICKAUTH_REGISTER) {
            module_log("Expiring unauthenticated nick %s", ni->nick);
            return 1;
        }
    }
    return 0;
}

/* The nick groups whose authentication is overdue are in the database, not
 * in memory: a batch of them is looked for every few minutes, and looking
 * each one up runs the check above (through NickServ's expiration check).
 */

#define NOAUTH_INTERVAL 300
#define NOAUTH_BATCH    500

static Timeout *noauth_timeout;

static void noauth_found(const struct DbResult *res, void *user_unused)
{
    unsigned int i;

    if (res->err.dberr_code != DB_OK) {
        module_log("Cannot look for unauthenticated nicks: %s",
                   res->err.dberr_message);
        return;
    }
    for (i = 0; i < db_rows(res->data); i++) {
        const char *id = db_row_str(res->data, i, "id");
        NickGroupInfo *ngi = get_nickgroupinfo(strtoul(id, NULL, 10));
        if (ngi && ngi->nicks_count > 0)
            put_nickinfo(get_nickinfo(ngi_mainnick(ngi)));
        put_nickgroupinfo(ngi);
    }
}

static void noauth_check(Timeout *t_unused)
{
    char cutoff[32], limit[16];
    struct DbParam p[2] = {
        { DB_TYPE_UNKNOWN, cutoff, DB_FORMAT_TEXT },
        { DB_TYPE_UNKNOWN, limit, DB_FORMAT_TEXT },
    };
    struct DbParam *pl[3] = { &p[0], &p[1], NULL };
    struct DbQuery q = { "select id from nickgroups where authcode <> 0"
                         " and authset <= $1::bigint order by authset"
                         " limit $2::integer", pl };

    if (!NSNoAuthExpire || noexpire || readonly)
        return;
    snprintf(cutoff, sizeof(cutoff), "%lld",
             (long long)(time(NULL) - NSNoAuthExpire));
    snprintf(limit, sizeof(limit), "%d", NOAUTH_BATCH);
    db_query(THIS_MODULE, &q, noauth_found, NULL);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static ConfigDirective mail_auth_config[] = {
    { "NSNoAuthExpire",   { { CD_TIME, 0, &NSNoAuthExpire } } },
    { "NSSendauthDelay",  { { CD_TIME, 0, &NSSendauthDelay } } },
    { NULL }
};

/* Old message number */
static int old_LIST_OPER_SYNTAX = -1;
static int old_HELP_REGISTER_EMAIL = -1;
static int old_OPER_HELP_LIST = -1;

/*************************************************************************/

static int mail_auth_init(Module *module)
{
    module_nickserv = module_find("nickserv/main");

    if (!NSRequireEmail) {
        module_log("NSRequireEmail must be set to use nickname"
                   " authentication");
        return 0;
    }

    if (!register_commands(module_nickserv, commands)) {
        module_log("Unable to register commands");
        return 0;
    }

    authed_event = event_declare(module, MAIL_AUTH_EVENT_AUTHED);
    if (!authed_event) {
        module_log("Unable to declare events");
        return 0;
    }

    if (!event_attach(module, EVENT_USER_DELETE, sendauth_userdel)
     || !event_attach(module, NICKSERV_EVENT_REGISTERED, do_registered)
     || !event_attach(module, NICKSERV_EVENT_SET_EMAIL, do_set_email)
     || !event_attach(module, NICKSERV_EVENT_IDENTIFY_CHECK, do_identify_check)
     || !event_attach(module, NICKSERV_EVENT_CHECK_EXPIRE, do_check_expire)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    noauth_timeout = add_timeout(NOAUTH_INTERVAL, noauth_check, 1);

    old_LIST_OPER_SYNTAX =
        mapstring(NICK_LIST_OPER_SYNTAX, NICK_LIST_OPER_SYNTAX_AUTH);
    old_HELP_REGISTER_EMAIL =
        mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL_AUTH);
    old_OPER_HELP_LIST =
        mapstring(NICK_OPER_HELP_LIST, NICK_OPER_HELP_LIST_AUTH);

    return 1;
}

/*************************************************************************/

static int mail_auth_fini(Module *module, int shutdown)
{
    if (noauth_timeout) {
        del_timeout(noauth_timeout);
        noauth_timeout = NULL;
    }
    if (old_OPER_HELP_LIST >= 0) {
        mapstring(NICK_OPER_HELP_LIST, old_OPER_HELP_LIST);
        old_OPER_HELP_LIST = -1;
    }
    if (old_HELP_REGISTER_EMAIL >= 0) {
        mapstring(NICK_HELP_REGISTER_EMAIL, old_HELP_REGISTER_EMAIL);
        old_HELP_REGISTER_EMAIL = -1;
    }
    if (old_LIST_OPER_SYNTAX >= 0) {
        mapstring(NICK_LIST_OPER_SYNTAX, old_LIST_OPER_SYNTAX);
        old_LIST_OPER_SYNTAX = -1;
    }

    if (module_nickserv) {
        unregister_commands(module_nickserv, commands);
        module_nickserv = NULL;
    }

    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "NickServ AUTH: verify the address of a nick by mail",
    .requires = MODULE_REQUIRES("nickserv/main", "mail/main"),
    .config = mail_auth_config,
    .init = mail_auth_init,
    .fini = mail_auth_fini,
};

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
