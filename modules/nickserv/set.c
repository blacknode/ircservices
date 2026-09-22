/* Routines to handle the NickServ SET command.
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
#include "encrypt.h"
#include "modules/operserv/operserv.h"

#include "nickserv.h"
#include "ns-local.h"

/*************************************************************************/

static int cb_set = -1;
static int cb_set_email = -1;
static int cb_unset = -1;

/*************************************************************************/

static void do_set_password(User *u, NickGroupInfo *ngi, NickInfo *ni,
                            char *param);
static void do_set_language(User *u, NickGroupInfo *ngi, char *param);
static void do_set_url(User *u, NickGroupInfo *ngi, char *param);
static void do_set_email(User *u, NickGroupInfo *ngi, char *param);
static void do_set_info(User *u, NickGroupInfo *ngi, char *param);
static void do_set_kill(User *u, NickGroupInfo *ngi, char *param);
static void do_set_secure(User *u, NickGroupInfo *ngi, char *param);
static void do_set_private(User *u, NickGroupInfo *ngi, char *param);
static void do_set_noop(User *u, NickGroupInfo *ngi, char *param);
static void do_set_hide(User *u, NickGroupInfo *ngi, char *param,
                        char *setting);
static void do_set_timezone(User *u, NickGroupInfo *ngi, char *param);
static void do_set_noexpire(User *u, NickInfo *ni, char *param);

/*************************************************************************/
/*************************************************************************/

void do_set(User *u)
{
    char *cmd   = strtok(NULL, " ");
    char *param = strtok_remaining();
    char *extra = NULL;
    NickInfo *ni;
    NickGroupInfo *ngi = NULL;
    int is_servadmin = is_services_admin(u);
    int used_privs = 0;

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_SET_DISABLED);
        return;
    }

    if (is_servadmin && cmd && *cmd == '!') {
        ni = get_nickinfo(cmd+1);
        if (!ni) {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, cmd+1);
            return;
        }
        cmd = strtok(param, " ");
        param = strtok_remaining();
        used_privs = !(valid_ngi(u) && ni->nickgroup == u->ngi->id);
    } else {
        ni = u->ni;
        if (ni)
            ni->usecount++;
    }
    if (cmd && stricmp(cmd, "INFO") != 0) {
        param = strtok(param, " ");
        extra = strtok(NULL, " ");
    }
    if (!cmd || !param
     || (stricmp(cmd,"HIDE")==0 ? extra==NULL : extra!=NULL)
    ) {
        if (is_oper(u))
            syntax_error(s_NickServ, u, "SET", NICK_SET_OPER_SYNTAX);
        else
            syntax_error(s_NickServ, u, "SET", NICK_SET_SYNTAX);
    } else if (!ni) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, ni->nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else if (!is_servadmin && !user_identified(u)
               && !(stricmp(cmd,"EMAIL")==0 && user_ident_nomail(u))) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (call_callback_5(cb_set, u, ni, ngi, cmd, param) > 0) {
        /* nothing */
    } else if (stricmp(cmd, "PASSWORD") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET PASSWORD as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_password(u, ngi, ni, param);
    } else if (stricmp(cmd, "LANGUAGE") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET LANGUAGE as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_language(u, ngi, param);
    } else if (stricmp(cmd, "URL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET URL as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_url(u, ngi, param);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET EMAIL as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_email(u, ngi, param);
    } else if (stricmp(cmd, "INFO") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET INFO as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_info(u, ngi, param);
    } else if (stricmp(cmd, "KILL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET KILL as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_kill(u, ngi, param);
    } else if (stricmp(cmd, "SECURE") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET SECURE as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_secure(u, ngi, param);
    } else if (stricmp(cmd, "PRIVATE") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET PRIVATE as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_private(u, ngi, param);
    } else if (stricmp(cmd, "NOOP") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET NOOP as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_noop(u, ngi, param);
    } else if (stricmp(cmd, "HIDE") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET HIDE as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_hide(u, ngi, param, extra);
    } else if (stricmp(cmd, "TIMEZONE") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used SET TIMEZONE as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_timezone(u, ngi, param);
    } else if (stricmp(cmd, "NOEXPIRE") == 0) {
        if (WallAdminPrivs && is_servadmin) {
            wallops(s_NickServ, "\2%s\2 used SET NOEXPIRE on \2%s\2",
                    u->nick, ni->nick);
        }
        do_set_noexpire(u, ni, param);
    } else {
        notice_lang(s_NickServ, u, NICK_SET_UNKNOWN_OPTION, strupper(cmd));
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

void do_unset(User *u)
{
    char *cmd   = strtok(NULL, " ");
    char *extra = strtok_remaining();
    NickInfo *ni;
    NickGroupInfo *ngi = NULL;
    int is_servadmin = is_services_admin(u);
    int used_privs = 0;
    int syntax_msg;

    if (readonly) {
        notice_lang(s_NickServ, u, NICK_SET_DISABLED);
        return;
    }

    if (is_oper(u))
        syntax_msg = NSRequireEmail ? NICK_UNSET_OPER_SYNTAX_REQ_EMAIL
                                    : NICK_UNSET_OPER_SYNTAX;
    else
        syntax_msg = NSRequireEmail ? NICK_UNSET_SYNTAX_REQ_EMAIL
                                    : NICK_UNSET_SYNTAX;

    if (is_servadmin && cmd && *cmd == '!') {
        ni = get_nickinfo(cmd+1);
        if (!ni) {
            notice_lang(s_NickServ, u, NICK_X_NOT_REGISTERED, cmd+1);
            return;
        }
        cmd = strtok(extra, " ");
        extra = strtok_remaining();
        used_privs = !(valid_ngi(u) && ni->nickgroup == u->ngi->id);
    } else {
        ni = u->ni;
        if (ni)
            ni->usecount++;
    }
    if (!cmd || extra) {
        syntax_error(s_NickServ, u, "UNSET", syntax_msg);
    } else if (!ni) {
        notice_lang(s_NickServ, u, NICK_NOT_REGISTERED);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(s_NickServ, u, NICK_X_FORBIDDEN, ni->nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(s_NickServ, u, INTERNAL_ERROR);
    } else if (!is_servadmin && !user_identified(u)) {
        notice_lang(s_NickServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
    } else if (call_callback_4(cb_unset, u, ni, ngi, cmd) > 0) {
        /* nothing */
    } else if (stricmp(cmd, "URL") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used UNSET URL as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_url(u, ngi, NULL);
    } else if (stricmp(cmd, "EMAIL") == 0) {
        if (NSRequireEmail) {
            if (ni != u->ni)
                notice_lang(s_NickServ, u, NICK_UNSET_EMAIL_OTHER_BAD);
            else
                notice_lang(s_NickServ, u, NICK_UNSET_EMAIL_BAD);
        } else {
            if (WallAdminPrivs && used_privs) {
                wallops(s_NickServ, "\2%s\2 used UNSET EMAIL as Services admin"
                        " on \2%s\2", u->nick, ni->nick);
            }
            do_set_email(u, ngi, NULL);
        }
    } else if (stricmp(cmd, "INFO") == 0) {
        if (WallAdminPrivs && used_privs) {
            wallops(s_NickServ, "\2%s\2 used UNSET INFO as Services admin"
                    " on \2%s\2", u->nick, ni->nick);
        }
        do_set_info(u, ngi, NULL);
    } else {
        syntax_error(s_NickServ, u, "UNSET", syntax_msg);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_set_password(User *u, NickGroupInfo *ngi, NickInfo *ni,
                            char *param)
{
    Password passbuf;

    if (NSSecureAdmins && u->ni != ni && nick_is_services_admin(ni)
     && !is_services_root(u)
    ) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return;
    } else if (!(NoAdminPasswordCheck && is_services_admin(u))
               && (stricmp(param, ni->nick) == 0
                || (StrictPasswords && strlen(param) < 5))) {
        notice_lang(s_NickServ, u, MORE_OBSCURE_PASSWORD);
        return;
    }

    init_password(&passbuf);
    if (encrypt_password(param, strlen(param), &passbuf) != 0) {
        clear_password(&passbuf);
        memset(param, 0, strlen(param));
        module_log("Failed to encrypt password for %s (set)", ni->nick);
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_FAILED);
        return;
    }
    copy_password(&ngi->pass, &passbuf);
    clear_password(&passbuf);
    if (NSShowPassword)
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED_TO, param);
    else
        notice_lang(s_NickServ, u, NICK_SET_PASSWORD_CHANGED);
    memset(param, 0, strlen(param));
}

/*************************************************************************/

static void do_set_language(User *u, NickGroupInfo *ngi, char *param)
{
    int langnum;

    if (param[strspn(param, "0123456789")] != 0) {  /* i.e. not a number */
        syntax_error(s_NickServ, u, "SET LANGUAGE", NICK_SET_LANGUAGE_SYNTAX);
        return;
    }
    langnum = atoi(param)-1;
    if (langnum < 0 || langnum >= NUM_LANGS || langlist[langnum] < 0) {
        notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_UNKNOWN,
                    langnum+1, s_NickServ);
        return;
    }
    ngi->language = langlist[langnum];
    notice_lang(s_NickServ, u, NICK_SET_LANGUAGE_CHANGED,
                getstring(ngi,LANG_NAME));
}

/*************************************************************************/

static void do_set_url(User *u, NickGroupInfo *ngi, char *param)
{
    const char *nick = ngi_mainnick(ngi);

    if (param && !valid_url(param)) {
        notice_lang(s_NickServ, u, BAD_URL);
        return;
    }

    free(ngi->url);
    if (param) {
        ngi->url = sstrdup(param);
        notice_lang(s_NickServ, u, NICK_SET_URL_CHANGED, nick, param);
    } else {
        ngi->url = NULL;
        notice_lang(s_NickServ, u, NICK_UNSET_URL, nick);
    }
}

/*************************************************************************/

static void do_set_email(User *u, NickGroupInfo *ngi, char *param)
{
    const char *nick = ngi_mainnick(ngi);
    char oldemail[BUFSIZE];

    if (param && !valid_email(param)) {
        notice_lang(s_NickServ, u, BAD_EMAIL);
        return;
    }
    if (param && rejected_email(param)) {
        notice_lang(s_NickServ, u, REJECTED_EMAIL);
        return;
    }

    if (param && !is_services_admin(u)) {
        int n = count_nicks_with_email(param);
        time_t now = time(NULL);
        if (n < 0) {
            notice_lang(s_NickServ, u, NICK_SET_EMAIL_UNAUTHED);
            return;
        } else if (NSRegEmailMax && n >= NSRegEmailMax) {
            notice_lang(s_NickServ, u, NICK_SET_EMAIL_TOO_MANY_NICKS,
                        param, n, NSRegEmailMax);
            return;
        } else if (now < u->last_nick_set_email + NSSetEmailDelay) {
            time_t left = (u->last_nick_set_email + NSSetEmailDelay) - now;
            notice_lang(s_NickServ, u, NICK_SET_EMAIL_PLEASE_WAIT,
                        maketime(u->ngi, left, MT_SECONDS));
            return;
        }
        u->last_nick_set_email = now;
    }

    if (ngi->email) {
        strbcpy(oldemail, ngi->email);
        free(ngi->email);
    } else {
        *oldemail = 0;
    }
    if (param) {
        int i;
        ngi->email = sstrdup(param);
        if (*oldemail) {
            module_log("%s E-mail address changed from %s to %s by %s!%s@%s",
                       nick, oldemail, param, u->nick, u->username, u->host);
        } else {
            module_log("%s E-mail address set to %s by %s!%s@%s",
                       nick, param, u->nick, u->username, u->host);
        }
        notice_lang(s_NickServ, u, NICK_SET_EMAIL_CHANGED, nick, param);
        /* If any nicknames belonging to this group were IDENT_NOMAIL,
         * set them IDENTIFIED */
        ARRAY_FOREACH (i, ngi->nicks) {
            NickInfo *ni = get_nickinfo(ngi->nicks[i]);
            if (ni && nick_ident_nomail(ni)) {
                ni->authstat &= ~NA_IDENT_NOMAIL;
                ni->authstat |= NA_IDENTIFIED;
            }
            put_nickinfo(ni);
        }
    } else {
        ngi->email = NULL;
        if (*oldemail) {
            module_log("%s E-mail address cleared by %s!%s@%s (was %s)",
                       nick, u->nick, u->username, u->host, oldemail);
        }
        notice_lang(s_NickServ, u, NICK_UNSET_EMAIL, nick);
    }
    call_callback_3(cb_set_email, u, ngi, *oldemail ? oldemail : NULL);
}

/*************************************************************************/

static void do_set_info(User *u, NickGroupInfo *ngi, char *param)
{
    const char *nick = ngi_mainnick(ngi);

    free(ngi->info);
    if (param) {
        ngi->info = sstrdup(param);
        notice_lang(s_NickServ, u, NICK_SET_INFO_CHANGED, nick);
    } else {
        ngi->info = NULL;
        notice_lang(s_NickServ, u, NICK_UNSET_INFO, nick);
    }
}

/*************************************************************************/

static void do_set_kill(User *u, NickGroupInfo *ngi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ngi->flags |= NF_KILLPROTECT;
        ngi->flags &= ~(NF_KILL_QUICK | NF_KILL_IMMED);
        notice_lang(s_NickServ, u, NICK_SET_KILL_ON);
    } else if (stricmp(param, "QUICK") == 0) {
        ngi->flags |= NF_KILLPROTECT | NF_KILL_QUICK;
        ngi->flags &= ~NF_KILL_IMMED;
        notice_lang(s_NickServ, u, NICK_SET_KILL_QUICK);
    } else if (stricmp(param, "IMMED") == 0) {
        if (NSAllowKillImmed) {
            ngi->flags |= NF_KILLPROTECT | NF_KILL_QUICK | NF_KILL_IMMED;
            notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED);
        } else {
            notice_lang(s_NickServ, u, NICK_SET_KILL_IMMED_DISABLED);
            return;
        }
    } else if (stricmp(param, "OFF") == 0) {
        ngi->flags &= ~(NF_KILLPROTECT | NF_KILL_QUICK | NF_KILL_IMMED);
        notice_lang(s_NickServ, u, NICK_SET_KILL_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET KILL",
                     NSAllowKillImmed ? NICK_SET_KILL_IMMED_SYNTAX
                                      : NICK_SET_KILL_SYNTAX);
        return;
    }
}

/*************************************************************************/

static void do_set_secure(User *u, NickGroupInfo *ngi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ngi->flags |= NF_SECURE;
        notice_lang(s_NickServ, u, NICK_SET_SECURE_ON);
    } else if (stricmp(param, "OFF") == 0) {
        ngi->flags &= ~NF_SECURE;
        notice_lang(s_NickServ, u, NICK_SET_SECURE_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET SECURE", NICK_SET_SECURE_SYNTAX);
        return;
    }
}

/*************************************************************************/

static void do_set_private(User *u, NickGroupInfo *ngi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ngi->flags |= NF_PRIVATE;
        notice_lang(s_NickServ, u, NICK_SET_PRIVATE_ON);
    } else if (stricmp(param, "OFF") == 0) {
        ngi->flags &= ~NF_PRIVATE;
        notice_lang(s_NickServ, u, NICK_SET_PRIVATE_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET PRIVATE", NICK_SET_PRIVATE_SYNTAX);
        return;
    }
}

/*************************************************************************/

static void do_set_noop(User *u, NickGroupInfo *ngi, char *param)
{
    if (stricmp(param, "ON") == 0) {
        ngi->flags |= NF_NOOP;
        notice_lang(s_NickServ, u, NICK_SET_NOOP_ON);
    } else if (stricmp(param, "OFF") == 0) {
        ngi->flags &= ~NF_NOOP;
        notice_lang(s_NickServ, u, NICK_SET_NOOP_OFF);
    } else {
        syntax_error(s_NickServ, u, "SET NOOP", NICK_SET_NOOP_SYNTAX);
        return;
    }
}

/*************************************************************************/

static void do_set_hide(User *u, NickGroupInfo *ngi, char *param,
                        char *setting)
{
    int flag, onmsg, offmsg;

    if (stricmp(param, "EMAIL") == 0) {
        flag = NF_HIDE_EMAIL;
        onmsg = NICK_SET_HIDE_EMAIL_ON;
        offmsg = NICK_SET_HIDE_EMAIL_OFF;
    } else if (stricmp(param, "USERMASK") == 0) {
        flag = NF_HIDE_MASK;
        onmsg = NICK_SET_HIDE_MASK_ON;
        offmsg = NICK_SET_HIDE_MASK_OFF;
    } else if (stricmp(param, "QUIT") == 0) {
        flag = NF_HIDE_QUIT;
        onmsg = NICK_SET_HIDE_QUIT_ON;
        offmsg = NICK_SET_HIDE_QUIT_OFF;
    } else {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
        return;
    }
    if (stricmp(setting, "ON") == 0) {
        ngi->flags |= flag;
        notice_lang(s_NickServ, u, onmsg, s_NickServ);
    } else if (stricmp(setting, "OFF") == 0) {
        ngi->flags &= ~flag;
        notice_lang(s_NickServ, u, offmsg, s_NickServ);
    } else {
        syntax_error(s_NickServ, u, "SET HIDE", NICK_SET_HIDE_SYNTAX);
        return;
    }
}

/*************************************************************************/

/* Timezone definitions. */
static struct {
    const char *name;
    int16 offset;
} timezones[] = {
    /* GMT-12 */
    /* GMT-11 */
    /* GMT-10 */ {"HST",-600},
    /* GMT-9  */
    /* GMT-8  */ {"PST",-480},
    /* GMT-7  */ {"MST",-420}, {"PDT",-420},
    /* GMT-6  */ {"CST",-360}, {"MDT",-360},
    /* GMT-5  */ {"EST",-300}, {"CDT",-300},
    /* GMT-4  */ {"AST",-240}, {"EDT",-240},
    /* GMT-3  */ {"BRT",-180}, {"ADT",-180},
    /* GMT-2  */ {"BRST",-120},
    /* GMT-1  */
    /* GMT+0  */ {"GMT",   0}, {"UTC",   0}, {"WET",   0},
    /* GMT+1  */ {"MET",  60}, {"BST",  60}, {"IST",  60},
    /* GMT+2  */ {"EET", 120},
    /* GMT+3  */ {"MSK", 180},
    /* GMT+4  */ {"MSD", 240},
    /* GMT+5  */
    /* GMT+6  */
    /* GMT+7  */
    /* GMT+8  */
    /* GMT+9  */ {"JST", 540}, {"KST", 540},
    /* GMT+10 */
    /* GMT+11 */
    /* GMT+12 */ {"NZST",720},
    /* GMT+13 */ {"NZDT",780},
    { NULL }
};

static void do_set_timezone(User *u, NickGroupInfo *ngi, char *param)
{
    char *s;
    int i, j;
    char timebuf[BUFSIZE];

    if (stricmp(param, "DEFAULT") == 0) {
        ngi->timezone = TIMEZONE_DEFAULT;
        notice_lang(s_NickServ, u, NICK_SET_TIMEZONE_DEFAULT);
        return;
    }
    if (strnicmp(param, "GMT+", 4) == 0 || strnicmp(param, "GMT-", 4) == 0
     || strnicmp(param, "UTC+", 4) == 0 || strnicmp(param, "UTC-", 4) == 0
    ) {
        /* Treat GMT+n, UTC-n, etc. as just +n or -n */
        param += 3;
    }
    if (*param == '+' || *param == '-') {
        i = strtol(param+1, &s, 10);
        if (*s == ':') {
            if (s[1]>='0' && s[1]<='5' && s[2]>='0' && s[2]<='9')
                j = strtol(s+1, &s, 10);
            else
                j = -1;
        } else {
            j = 0;
        }
        if (i < 0 || i > 23 || j < 0 || j > 59 || *s) {
            syntax_error(s_NickServ, u, "SET TIMEZONE",
                         NICK_SET_TIMEZONE_SYNTAX);
            return;
        }
        ngi->timezone = i*60 + j;
        if (*param == '-')
            ngi->timezone = -ngi->timezone;
    } else {
        for (i = 0; timezones[i].name; i++) {
            if (stricmp(param, timezones[i].name) == 0)
                break;
        }
        if (!timezones[i].name) {
            syntax_error(s_NickServ, u, "SET TIMEZONE",
                         NICK_SET_TIMEZONE_SYNTAX);
            return;
        }
        ngi->timezone = timezones[i].offset;
    }
    /* This is tricky, because we want the calling user's language but the
     * target user's timezone. */
    if (ngi->timezone != TIMEZONE_DEFAULT) {
        j = ngi->timezone * 60;
    } else {
        time_t tmp = 0;
        struct tm *tm = localtime(&tmp);
        j = tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec;
        if (tm->tm_year < 70)
            j -= 86400;
    }
    if (valid_ngi(u) && u->ngi->timezone != TIMEZONE_DEFAULT) {
        j -= u->ngi->timezone * 60;
    } else {
        time_t tmp = 0;
        struct tm *tm = localtime(&tmp);
        tmp = tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec;
        if (tm->tm_year < 70)
            tmp -= 86400;
        j -= tmp;
    }
    strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                  STRFTIME_DATE_TIME_FORMAT, time(NULL) + j);
    if (ngi->timezone < 0)
        i = -ngi->timezone;
    else
        i = ngi->timezone;
    notice_lang(s_NickServ, u, NICK_SET_TIMEZONE_TO,
                ngi->timezone<0 ? '-' : '+', i/60, i%60, timebuf);
}

/*************************************************************************/

static void do_set_noexpire(User *u, NickInfo *ni, char *param)
{
    if (!is_services_admin(u)) {
        notice_lang(s_NickServ, u, PERMISSION_DENIED);
        return;
    }
    if (stricmp(param, "ON") == 0) {
        ni->status |= NS_NOEXPIRE;
        notice_lang(s_NickServ, u, NICK_SET_NOEXPIRE_ON, ni->nick);
    } else if (stricmp(param, "OFF") == 0) {
        ni->status &= ~NS_NOEXPIRE;
        notice_lang(s_NickServ, u, NICK_SET_NOEXPIRE_OFF, ni->nick);
    } else {
        syntax_error(s_NickServ, u, "SET NOEXPIRE", NICK_SET_NOEXPIRE_SYNTAX);
        return;
    }
}

/*************************************************************************/
/*************************************************************************/

int init_set(void)
{
    cb_set = register_callback("SET");
    cb_set_email = register_callback("SET EMAIL");
    cb_unset = register_callback("UNSET");
    if (cb_set < 0 || cb_set_email < 0 || cb_unset < 0) {
        module_log("set: Unable to register callbacks");
        exit_set();
        return 0;
    }
    return 1;
}

/*************************************************************************/

void exit_set()
{
    unregister_callback(cb_unset);
    unregister_callback(cb_set_email);
    unregister_callback(cb_set);
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
