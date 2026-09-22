/* MemoServ forwarding module.
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
#include "modules/mail/mail.h"
#include "modules/nickserv/nickserv.h"

#include "memoserv.h"

/*************************************************************************/
/*************************** Local variables *****************************/
/*************************************************************************/

static Module *module_memoserv;
static Module *module_nickserv_mail_auth;
static Module *module_mail;

static int    MSAllowForward = 0;
static time_t MSForwardDelay = 0;

/*************************************************************************/

static void do_forward(User *u);

/* NOTE: {init,exit}_module() assume that FORWARD is the last item in the list,
 *       so don't rearrange things here! */
static Command commands[] = {
    { "SET FORWARD", NULL,     NULL,           MEMO_HELP_SET_FORWARD, -1,-1 },
    { "FORWARD",  do_forward,  NULL,           MEMO_HELP_FORWARD,     -1,-1 },
    { NULL }
};

/*************************************************************************/
/*************************** Command handlers ****************************/
/*************************************************************************/

/* Handle the FORWARD command. */

static int fwd_memo(MemoInfo *mi, int num, const User *u, char **p_body,
                    uint32 *p_bodylen);
static int fwd_memo_callback(int num, va_list args);

static void do_forward(User *u)
{
    MemoInfo *mi;
    char *numstr = strtok_remaining();
    time_t now = time(NULL);

    if (!user_identified(u)) {
        notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
        return;
    }
    mi = &u->ngi->memos;
    if (!numstr || (!isdigit(*numstr) && stricmp(numstr, "ALL") != 0)) {
        syntax_error(s_MemoServ, u, "FORWARD", MEMO_FORWARD_SYNTAX);
    } else if (mi->memos_count == 0) {
        notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
    } else if (MSForwardDelay > 0 && u->lastmemofwd+MSForwardDelay > now) {
        u->lastmemofwd = now;
        notice_lang(s_MemoServ, u, MEMO_FORWARD_PLEASE_WAIT,
                    maketime(u->ngi,MSForwardDelay,MT_SECONDS));
    } else {
        int fwdcount, count, last, i;
        char *body = NULL;
        uint32 bodylen = 0;

        if (isdigit(*numstr)) {
            /* Forward a specific memo or memos. */
            fwdcount = process_numlist(numstr, &count, fwd_memo_callback,
                                       u, mi, &last, &body, &bodylen);
        } else {
            /* Forward all memos. */
            count = 0;
            ARRAY_FOREACH (i, mi->memos) {
                if (fwd_memo(mi, mi->memos[i].number, u, &body,&bodylen) == 1){
                    count++;
                } else {
                    module_log("do_forward(): BUG: memo %d not found for"
                               " ALL (index %d, nick %s, nickgroup %u)",
                               mi->memos[i].number, i, u->nick, u->ngi->id);
                }
            }
            fwdcount = -1;  /* "forwarded all memos" */
        }
        if (body) {
            /* Some/all memos got forwarded.  Send the message out and
             * report success. */
            char subject[BUFSIZE];
            const char *s;
            if (count == 1)
                s = getstring(u->ngi, MEMO_FORWARD_MAIL_SUBJECT);
            else
                s = getstring(u->ngi, MEMO_FORWARD_MULTIPLE_MAIL_SUBJECT);
            snprintf(subject, sizeof(subject), s, u->ni->nick);
            sendmail(u->ngi->email, subject, body,
                     getstring(u->ngi,LANG_CHARSET), NULL, NULL);
            free(body);
            if (fwdcount < 0)
                notice_lang(s_MemoServ, u, MEMO_FORWARDED_ALL);
            else if (fwdcount > 1)
                notice_lang(s_MemoServ, u, MEMO_FORWARDED_SEVERAL, fwdcount);
            else
                notice_lang(s_MemoServ, u, MEMO_FORWARDED_ONE, last);
        } else {
            /* No memos were forwarded. */
            if (count == 1 && fwdcount == 0)
                notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST, atoi(numstr));
            else
                notice_lang(s_MemoServ, u, MEMO_FORWARDED_NONE);
        }
        u->lastmemofwd = now;
    }
}


/* Forward a memo by number.  Return:
 *     1 if the memo was found and delivered
 *     0 if the memo was not found
 */
static int fwd_memo(MemoInfo *mi, int num, const User *u, char **p_body,
                    uint32 *p_bodylen)
{
    int i, len;
    char thisbody[BUFSIZE*2], timebuf[BUFSIZE];

    ARRAY_SEARCH_SCALAR(mi->memos, number, num, i);
    if (i < mi->memos_count) {
        strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                      STRFTIME_DATE_TIME_FORMAT, mi->memos[i].time);
        timebuf[sizeof(timebuf)-1] = 0;
        if (mi->memos[i].channel) {
            len = snprintf(thisbody, sizeof(thisbody)-1,
                           getstring(u->ngi, MEMO_FORWARD_CHANMEMO_MAIL_BODY),
                           mi->memos[i].sender, mi->memos[i].channel, timebuf,
                           mi->memos[i].text);
        } else {
            len = snprintf(thisbody, sizeof(thisbody)-1,
                           getstring(u->ngi, MEMO_FORWARD_MAIL_BODY),
                           mi->memos[i].sender, timebuf, mi->memos[i].text);
        }
        thisbody[len++] = '\n';
        thisbody[len] = 0;
        if (*p_bodylen) {
            int old_bodylen = *p_bodylen;
            char *dest;
            *p_bodylen += strlen(thisbody)+1;
            *p_body = srealloc(*p_body, *p_bodylen+1);
            dest = *p_body + old_bodylen;
            *dest++ = '\n';
            strcpy(dest, thisbody);  /* safe because allocated above */
        } else {
            *p_bodylen = strlen(thisbody);
            *p_body = sstrdup(thisbody);
        }
        return 1;
    } else {
        return 0;
    }
}

/* Forward a single memo from a MemoInfo. */
static int fwd_memo_callback(int num, va_list args)
{
    User *u = va_arg(args, User *);
    MemoInfo *mi = va_arg(args, MemoInfo *);
    int *last = va_arg(args, int *);
    char **p_body = va_arg(args, char **);
    uint32 *p_bodylen = va_arg(args, uint32 *);
    int i;

    i = fwd_memo(mi, num, u, p_body, p_bodylen);
    if (i > 0)
        *last = num;
    return i;
}

/*************************************************************************/

/* Handle the SET FORWARD command. */

static int do_set_forward(User *u, MemoInfo *mi, const char *option,
                          const char *param)
{
    if (stricmp(option, "FORWARD") != 0)
        return 0;
    if (!u->ngi->email) {
        notice_lang(s_MemoServ, u, MEMO_FORWARD_NEED_EMAIL);
    } else if (stricmp(param, "ON") == 0) {
        u->ngi->flags |= NF_MEMO_FWD;
        u->ngi->flags &= ~NF_MEMO_FWDCOPY;
        notice_lang(s_MemoServ, u, MEMO_SET_FORWARD_ON, u->ngi->email);
    } else if (stricmp(param, "COPY") == 0) {
        u->ngi->flags |= NF_MEMO_FWD | NF_MEMO_FWDCOPY;
        notice_lang(s_MemoServ, u, MEMO_SET_FORWARD_COPY, u->ngi->email);
    } else if (stricmp(param, "OFF") == 0) {
        u->ngi->flags &= ~(NF_MEMO_FWD | NF_MEMO_FWDCOPY);
        notice_lang(s_MemoServ, u, MEMO_SET_FORWARD_OFF);
    } else {
        syntax_error(s_MemoServ, u, "SET FORWARD", MEMO_SET_FORWARD_SYNTAX);
    }
    return 1;
}

/*************************************************************************/
/****************************** Callbacks ********************************/
/*************************************************************************/

/* Callback to handle an incoming memo. */

static int do_receive_memo(const User *sender, const char *target,
                           NickGroupInfo *ngi, const char *channel,
                           const char *text)
{
    char subject[BUFSIZE], body[BUFSIZE*2], timebuf[BUFSIZE], tempbuf[BUFSIZE];
    time_t now = time(NULL);
    int i;

    if (!(ngi->flags & NF_MEMO_FWD))
        return 0;
    strftime_lang(timebuf, sizeof(timebuf), ngi,
                  STRFTIME_DATE_TIME_FORMAT, now);
    timebuf[sizeof(timebuf)-1] = 0;
    ARRAY_FOREACH (i, ngi->nicks) {
        if (irc_stricmp(ngi->nicks[i],target) == 0) {
            target = ngi->nicks[i];  /* use the nick owner's capitalization */
            break;
        }
    }
    if (channel) {
        snprintf(tempbuf, sizeof(tempbuf), "%s (%s)", target, channel);
        target = tempbuf;
    }
    snprintf(subject, sizeof(subject),
             getstring(ngi,MEMO_FORWARD_MAIL_SUBJECT), target);
    if (channel) {
        snprintf(body, sizeof(body),
                 getstring(ngi,MEMO_FORWARD_CHANMEMO_MAIL_BODY),
                 sender->nick, channel, timebuf, text);
    } else {
        snprintf(body, sizeof(body), getstring(ngi,MEMO_FORWARD_MAIL_BODY),
                 sender->nick, timebuf, text);
    }
    sendmail(ngi->email, subject, body, getstring(ngi,LANG_CHARSET), NULL,
             NULL);
    if (!(ngi->flags & NF_MEMO_FWDCOPY))
        return 1;
    return 0;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "MSAllowForward",   { { CD_SET, 0, &MSAllowForward } } },
    { "MSForwardDelay",   { { CD_TIME, 0, &MSForwardDelay } } },
    { NULL }
};

/*************************************************************************/

static int do_reconfigure(int after_configure)
{
    if (after_configure) {
        if (MSAllowForward)
            commands[lenof(commands)-2].name = "FORWARD";
        else
            commands[lenof(commands)-2].name = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    module_memoserv = find_module("memoserv/main");
    if (!module_memoserv) {
        module_log("Main MemoServ module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_memoserv);

    module_nickserv_mail_auth = find_module("nickserv/mail-auth");
    if (!module_nickserv_mail_auth) {
        module_log("NickServ AUTH module (mail-auth) required for FORWARD");
        exit_module(0);
        return 0;
    }
    use_module(module_nickserv_mail_auth);

    module_mail = find_module("mail/main");
    if (!module_mail) {
        module_log("Mail module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_mail);

    if (!MSAllowForward)
        commands[lenof(commands)-2].name = NULL;
    if (!register_commands(module_memoserv, commands)) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "reconfigure", do_reconfigure)
     || !add_callback_pri(module_memoserv, "receive memo", do_receive_memo,
                          MS_RECEIVE_PRI_DELIVER)
     || !add_callback(module_memoserv, "SET", do_set_forward)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (module_mail) {
        unuse_module(module_mail);
        module_mail = NULL;
    }
    if (module_nickserv_mail_auth) {
        unuse_module(module_nickserv_mail_auth);
        module_nickserv_mail_auth = NULL;
    }
    if (module_memoserv) {
        remove_callback(module_memoserv, "SET", do_set_forward);
        remove_callback(module_memoserv, "receive memo", do_receive_memo);
        unregister_commands(module_memoserv, commands);
        unuse_module(module_memoserv);
        module_memoserv = NULL;
    }
    remove_callback(NULL, "reconfigure", do_reconfigure);

    commands[lenof(commands)-2].name = "FORWARD";

    return 1;
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
