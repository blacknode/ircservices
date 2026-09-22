/* S-line module.
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
#include "databases.h"
#include "language.h"

#include "operserv.h"
#define NEED_MAKE_REASON
#include "maskdata.h"
#include "sline.h"

/*************************************************************************/

static Module *module_operserv;
static Module *module_nickserv;

static int cb_send_sgline   = -1;
static int cb_send_sqline   = -1;
static int cb_send_szline   = -1;
static int cb_cancel_sgline = -1;
static int cb_cancel_sqline = -1;
static int cb_cancel_szline = -1;

static char * SGlineReason;
static char * SQlineReason;
static char * SZlineReason;
static int    ImmediatelySendSline;
static time_t SGlineExpiry;
static time_t SQlineExpiry;
static time_t SZlineExpiry;
static int    WallOSSline;
static int    WallSlineExpire;
static int    SQlineIgnoreOpers;
static int    SQlineKill;

/* Set nonzero if we don't have client IP info:
 *    -1 = no IP info and no SZLINE-like command either
 *    +1 = no IP info, but SZLINE command available
 */
static int no_szline = 0;

static void do_sgline(User *u);
static void do_sqline(User *u);
static void do_szline(User *u);

static uint8 sline_types[3] = {MD_SGLINE, MD_SQLINE, MD_SZLINE};

static Command cmds[] = {
    {"SGLINE",    do_sgline,    is_services_oper, OPER_HELP_SGLINE,    -1,-1},
    {"SQLINE",    do_sqline,    is_services_oper, OPER_HELP_SQLINE,    -1,-1},
    {"SZLINE",    do_szline,    is_services_oper, OPER_HELP_SZLINE,    -1,-1},
    { NULL }
};

/*************************************************************************/
/************************** Internal functions ***************************/
/*************************************************************************/

/* Send an S-line to the uplink server. */

static void send_sline(uint8 type, const MaskData *sline)
{
    int cb;
    const char *reason;

    if (type == MD_SGLINE) {
        cb = cb_send_sgline;
        reason = SGlineReason;
    } else if (type == MD_SQLINE && !SQlineKill) {
        cb = cb_send_sqline;
        reason = SQlineReason;
    } else if (type == MD_SZLINE) {
        cb = cb_send_szline;
        reason = SZlineReason;
    } else {
        return;
    }
    call_callback_4(cb, sline->mask, sline->expires, sline->who,
                    make_reason(reason, sline));
}

/*************************************************************************/

/* Remove an S-line from the uplink server. */

static void cancel_sline(uint8 type, char *mask)
{
    int cb;

    if (type == MD_SGLINE) {
        cb = cb_cancel_sgline;
    } else if (type == MD_SQLINE) {
        cb = cb_cancel_sqline;
    } else if (type == MD_SZLINE) {
        cb = cb_cancel_szline;
    } else {
        return;
    }
    call_callback_1(cb, mask);
}

/*************************************************************************/

/* SQline checker, shared by do_user_check() and do_user_nickchange_after().
 * Returns the reason string to be used if the user is to be killed, else
 * NULL.  `new_oper' indicates whether a new user (who doesn't have a User
 * record yet) is an oper.
 */

static char *check_sqline(const char *nick, int new_oper)
{
    User *u;
    MaskData *sline;

    if (SQlineIgnoreOpers && (new_oper || ((u=get_user(nick)) && is_oper(u))))
        return NULL;

    sline = get_matching_maskdata(MD_SQLINE, nick);
    if (sline) {
        char *retval = NULL;
        /* Don't kill/nickchange users if they just got changed to a guest
         * nick */
        if (!is_guest_nick(nick)) {
            char *reason = make_reason(SQlineReason, sline);
            if (!SQlineKill && (protocol_features & PF_CHANGENICK)) {
                send_cmd(ServerName, "432 %s %s Invalid nickname (%s)",
                         nick, nick, reason);
                send_nickchange_remote(nick, make_guest_nick());
            } else {
                /* User is to be killed */
                retval = reason;
            }
        }
        send_sline(MD_SQLINE, sline);
        time(&sline->lastused);
        put_maskdata(sline);
        return retval;
    }

    return NULL;
}

/*************************************************************************/
/************************** Callback functions ***************************/
/*************************************************************************/

/* Does the user match any S-lines?  Return 1 (and kill the user) if so,
 * else 0.  Note that if a Q:line (and no G:line or Z:line) is matched, and
 * SQlineKill is not set, we send a 432 (erroneous nickname) reply to the
 * client and change their nick if the ircd supports forced nick changing.
 */

static int do_user_check(int ac, char **av)
{
    const char *nick = av[0], *name = av[6], *ip = (ac>=9 ? av[8] : NULL);
    int new_oper = (ac>=10 && av[9] ? strchr(av[9],'o') != NULL : 0);
    MaskData *sline;
    char *reason;

    if (noakill)
        return 0;

    if (ip) {
        sline = get_matching_maskdata(MD_SZLINE, ip);
        if (sline) {
            send_cmd(s_OperServ, "KILL %s :%s (%s)", nick, s_OperServ,
                     make_reason(SZlineReason, sline));
            send_sline(MD_SZLINE, sline);
            time(&sline->lastused);
            put_maskdata(sline);
            return 1;
        }
    } else {
        if (!no_szline) {
            if (protocol_features & PF_SZLINE) {
                if (!ImmediatelySendSline) {
                    wallops(s_OperServ,
                            "\2WARNING\2: Client IP addresses are not"
                            " available with this IRC server; SZLINEs"
                            " cannot be used unless ImmediatelySendSline"
                            " is enabled in %s.", MODULES_CONF);
                    no_szline = -1;
                } else {
                    no_szline = 1;
                }
            } else {
                wallops(s_OperServ,
                        "\2WARNING:\2 Client IP addresses are not available"
                        " with this IRC server; SZLINEs cannot be used.");
                no_szline = -1;
            }
            module_log("warning: client IP addresses not available with"
                       " this IRC server");
        }
    }

    sline = get_matching_maskdata(MD_SGLINE, name);
    if (sline) {
        /* Don't use kill_user(); that's for people who have already signed
         * on.  This is called before the User structure is created. */
        send_cmd(s_OperServ, "KILL %s :%s (%s)", nick, s_OperServ,
                 make_reason(SGlineReason, sline));
        send_sline(MD_SGLINE, sline);
        time(&sline->lastused);
        put_maskdata(sline);
        return 1;
    }
    put_maskdata(sline);

    reason = check_sqline(nick, new_oper);
    if (reason) {
        send_cmd(s_OperServ, "KILL %s :%s (%s)", nick, s_OperServ, reason);
        return 1;
    }

    return 0;
}

/*************************************************************************/

/* Does the user (who has just changed their nick) match any SQlines?  If
 * so, send them a 432 and, on servers supporting forced nick changing,
 * change their nick (if !SQlineKill).
 */

static int do_user_nickchange_after(User *u, const char *oldnick)
{
    char *reason = check_sqline(u->nick, 0);
    if (reason) {
        kill_user(s_OperServ, u->nick, reason);
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Callback for NickServ REGISTER/LINK check; we disallow
 * registration/linking of SQlined nicknames.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    MaskData *sline = get_matching_maskdata(MD_SQLINE, nick);
    put_maskdata(sline);
    return sline != NULL;
}

/*************************************************************************/
/************************** S-line list editing **************************/
/*************************************************************************/

/* Note that all string parameters are assumed to be non-NULL; expiry must
 * be set to the time when the S-line should expire (0 for none).  Mask
 * is converted to lowercase on return.
 */

EXPORT_FUNC(create_sline)
void create_sline(uint8 type, char *mask, const char *reason,
                  const char *who, time_t expiry)
{
    MaskData *sline;

    strlower(mask);
    if (maskdata_count(type) >= MAX_MASKDATA) {
        module_log("Attempt to add S%cLINE to full list!", type);
        return;
    }
    sline = scalloc(1, sizeof(*sline));
    sline->mask = sstrdup(mask);
    sline->reason = sstrdup(reason);
    sline->time = time(NULL);
    sline->expires = expiry;
    strbcpy(sline->who, who);
    sline = add_maskdata(type, sline);
    if (ImmediatelySendSline)
        send_sline(type, sline);
}

/*************************************************************************/

/* Handle an OperServ S*LINE command. */

static void do_sline(uint8 type, User *u);
static void do_sgline(User *u) { do_sline(MD_SGLINE, u); }
static void do_sqline(User *u) { do_sline(MD_SQLINE, u); }
static void do_szline(User *u) {
    if (no_szline < 0)
        notice_lang(s_OperServ, u, OPER_SZLINE_NOT_AVAIL);
    else
        do_sline(MD_SZLINE, u);
}

static int check_add_sline(const User *u, uint8 type, char *mask,
                           time_t *expiry_ptr);
static void do_add_sline(const User *u, uint8 type, MaskData *md);
static void do_del_sline(const User *u, uint8 type, MaskData *md);

static MaskDataCmdInfo sline_cmd_info = {
    /* Name, type, and expiry time pointer are overwritten with data for
     * the particular command used right before calling do_maskdata_cmd() */
    .name               = "SxLINE",
    .md_type            = 0,
    .def_expiry_ptr     = NULL,

    .msg_add_too_many   = OPER_TOO_MANY_SLINES,
    .msg_add_exists     = OPER_SLINE_EXISTS,
    .msg_added          = OPER_SLINE_ADDED,
    .msg_del_not_found  = OPER_SLINE_NOT_FOUND,
    .msg_deleted        = OPER_SLINE_REMOVED,
    .msg_cleared        = OPER_SLINE_CLEARED,
    .msg_list_header    = OPER_SLINE_LIST_HEADER,
    .msg_list_empty     = OPER_SLINE_LIST_EMPTY,
    .msg_list_no_match  = OPER_SLINE_LIST_NO_MATCH,
    .msg_check_no_match = OPER_SLINE_CHECK_NO_MATCH,
    .msg_check_header   = OPER_SLINE_CHECK_HEADER,
    .msg_check_count    = OPER_SLINE_CHECK_TRAILER,
    .msg_count          = OPER_SLINE_COUNT,

    .mangle_mask        = NULL,
    .check_add_mask     = check_add_sline,
    .do_add_mask        = do_add_sline,
    .do_del_mask        = do_del_sline,
    .do_unknown_cmd     = NULL,
};

static void do_sline(uint8 type, User *u)
{
    char sxline[7];

    sprintf(sxline, "S%cLINE", type);  /* safe, for obvious reasons */
    sline_cmd_info.name = sxline;
    sline_cmd_info.md_type = type;
    switch (type) {
      case MD_SGLINE:
        sline_cmd_info.def_expiry_ptr = &SGlineExpiry;
        break;
      case MD_SQLINE:
        sline_cmd_info.def_expiry_ptr = &SQlineExpiry;
        break;
      case MD_SZLINE:
        sline_cmd_info.def_expiry_ptr = &SZlineExpiry;
        break;
      default:
        module_log("do_sline(): bad type value (%u)", type);
        notice_lang(s_OperServ, u, INTERNAL_ERROR);
        return;
    }
    do_maskdata_cmd(&sline_cmd_info, u);
}

/*************************************************************************/

static int check_add_sline(const User *u, uint8 type, char *mask,
                           time_t *expiry_ptr)
{
    char *t;

    /* Make sure mask is not too general. */
    if (strchr(mask,'*') != NULL && mask[strspn(mask,"*?")] == 0
     && ((t = strchr(mask,'?')) == NULL || strchr(t+1,'?') == NULL)
    ) {
        char cmdname[7];
        snprintf(cmdname, sizeof(cmdname), "S%cLINE", (char)type);
        notice_lang(s_OperServ, u, OPER_SLINE_MASK_TOO_GENERAL, cmdname);
        return 0;
    }

    return 1;
}

/*************************************************************************/

static void do_add_sline(const User *u, uint8 type, MaskData *md)
{
    if (WallOSSline) {
        char buf[128];
        expires_in_lang(buf, sizeof(buf), NULL, md->expires);
        wallops(s_OperServ, "%s added an S%cLINE for \2%s\2 (%s)",
                u->nick, type, md->mask, buf);
    }
    if (ImmediatelySendSline)
        send_sline(type, md);
}

/*************************************************************************/

static void do_del_sline(const User *u, uint8 type, MaskData *md)
{
    cancel_sline(type, md->mask);
}

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

/* Callback on connection to uplink server. */

static int do_connect(void)
{
    if (ImmediatelySendSline) {
        MaskData *sline;
        static uint8 types[] = {MD_SGLINE, MD_SQLINE, MD_SZLINE};
        int i;
        for (i = 0; i < lenof(types); i++) {
            for (sline = first_maskdata(types[i]); sline;
                 sline = next_maskdata(types[i])
            ) {
                send_sline(types[i], sline);
            }
        }
    }
    return 0;
}

/*************************************************************************/

/* Callback for S-line expiration. */

static int do_expire_maskdata(uint32 type, MaskData *md)
{
    int i;
    for (i = 0; i < lenof(sline_types); i++) {
        if (type == sline_types[i]) {
            if (WallSlineExpire)
                wallops(s_OperServ, "S%cLINE on %s has expired",
                        sline_types[i], md->mask);
            cancel_sline((uint8)type, md->mask);
        }
    }
    return 0;
}

/*************************************************************************/

/* OperServ HELP callback, to handle HELP SQLINE (complex). */

static int do_help(User *u, char *param)
{
    /* param should always be non-NULL here, but let's be paranoid */
    if (param && stricmp(param,"SQLINE") == 0) {
        notice_help(s_OperServ, u, OPER_HELP_SQLINE);
        if (SQlineKill)
            notice_help(s_OperServ, u, OPER_HELP_SQLINE_KILL);
        else
            notice_help(s_OperServ, u, OPER_HELP_SQLINE_NOKILL);
        if (SQlineIgnoreOpers)
            notice_help(s_OperServ, u, OPER_HELP_SQLINE_IGNOREOPERS);
        notice_help(s_OperServ, u, OPER_HELP_SQLINE_END);
        return 1;
    }
    return 0;
}

/*************************************************************************/

static int do_stats_all(User *user, const char *s_OperServ)
{
    int32 count, mem;
    MaskData *md;

    count = mem = 0;
    for (md = first_maskdata(MD_SGLINE); md; md = next_maskdata(MD_SGLINE)) {
        count++;
        mem += sizeof(*md);
        if (md->mask)
            mem += strlen(md->mask)+1;
        if (md->reason)
            mem += strlen(md->reason)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_SGLINE_MEM,
                count, (mem+512) / 1024);

    count = mem = 0;
    for (md = first_maskdata(MD_SQLINE); md; md = next_maskdata(MD_SQLINE)) {
        count++;
        mem += sizeof(*md);
        if (md->mask)
            mem += strlen(md->mask)+1;
        if (md->reason)
            mem += strlen(md->reason)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_SQLINE_MEM,
                count, (mem+512) / 1024);

    count = mem = 0;
    for (md = first_maskdata(MD_SZLINE); md; md = next_maskdata(MD_SZLINE)) {
        count++;
        mem += sizeof(*md);
        if (md->mask)
            mem += strlen(md->mask)+1;
        if (md->reason)
            mem += strlen(md->reason)+1;
    }
    notice_lang(s_OperServ, user, OPER_STATS_ALL_SZLINE_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

static void insert_sgline(void *md) { add_maskdata(MD_SGLINE, md); }
static void *first_sgline(void)     { return first_maskdata(MD_SGLINE); }
static void *next_sgline(void)      { return next_maskdata(MD_SGLINE); }

static void insert_sqline(void *md) { add_maskdata(MD_SQLINE, md); }
static void *first_sqline(void)     { return first_maskdata(MD_SQLINE); }
static void *next_sqline(void)      { return next_maskdata(MD_SQLINE); }

static void insert_szline(void *md) { add_maskdata(MD_SZLINE, md); }
static void *first_szline(void)     { return first_maskdata(MD_SZLINE); }
static void *next_szline(void)      { return next_maskdata(MD_SZLINE); }

static DBField sline_dbfields[] = {
    { "mask",     DBTYPE_STRING, offsetof(MaskData,mask) },
    { "reason",   DBTYPE_STRING, offsetof(MaskData,reason) },
    { "who",      DBTYPE_BUFFER, offsetof(MaskData,who), NICKMAX },
    { "time",     DBTYPE_TIME,   offsetof(MaskData,time) },
    { "expires",  DBTYPE_TIME,   offsetof(MaskData,expires) },
    { "lastused", DBTYPE_TIME,   offsetof(MaskData,lastused) },
    { NULL }
};
static DBTable sgline_dbtable = {
    .name    = "sgline",
    .newrec  = new_maskdata,
    .freerec = free_maskdata,
    .insert  = insert_sgline,
    .first   = first_sgline,
    .next    = next_sgline,
    .fields  = sline_dbfields,
};
static DBTable sqline_dbtable = {
    .name    = "sqline",
    .newrec  = new_maskdata,
    .freerec = free_maskdata,
    .insert  = insert_sqline,
    .first   = first_sqline,
    .next    = next_sqline,
    .fields  = sline_dbfields,
};
static DBTable szline_dbtable = {
    .name    = "szline",
    .newrec  = new_maskdata,
    .freerec = free_maskdata,
    .insert  = insert_szline,
    .first   = first_szline,
    .next    = next_szline,
    .fields  = sline_dbfields,
};

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "ImmediatelySendSline",{{CD_SET, 0, &ImmediatelySendSline } } },
    { "SGlineExpiry",     { { CD_TIME, 0, &SGlineExpiry } } },
    { "SGlineReason",     { { CD_STRING, CF_DIRREQ, &SGlineReason } } },
    { "SQlineExpiry",     { { CD_TIME, 0, &SGlineExpiry } } },
    { "SQlineIgnoreOpers",{ { CD_SET, 0, &SQlineIgnoreOpers } } },
    { "SQlineKill",       { { CD_SET, 0, &SQlineKill } } },
    { "SQlineReason",     { { CD_STRING, CF_DIRREQ, &SQlineReason } } },
    { "SZlineExpiry",     { { CD_TIME, 0, &SGlineExpiry } } },
    { "SZlineReason",     { { CD_STRING, CF_DIRREQ, &SZlineReason } } },
    { "WallSlineExpire",  { { CD_SET, 0, &WallSlineExpire } } },
    { "WallOSSline",      { { CD_SET, 0, &WallOSSline } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "nickserv/main") == 0) {
        module_nickserv = mod;
        if (!add_callback(mod, "REGISTER/LINK check", do_reglink_check))
            module_log("Unable to register NickServ REGISTER/LINK check"
                       " callback");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_nickserv) {
        module_nickserv = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    module_operserv = find_module("operserv/main");
    if (!module_operserv) {
        module_log("Main OperServ module not loaded");
        return 0;
    }
    use_module(module_operserv);

    if (!register_commands(module_operserv, cmds)) {
        module_log("Unable to register commands");
        exit_module(0);
        return 0;
    }

    cb_send_sgline   = register_callback("send_sgline");
    cb_send_sqline   = register_callback("send_sqline");
    cb_send_szline   = register_callback("send_szline");
    cb_cancel_sgline = register_callback("cancel_sgline");
    cb_cancel_sqline = register_callback("cancel_sqline");
    cb_cancel_szline = register_callback("cancel_szline");
    if (cb_send_sgline < 0 || cb_send_sqline < 0 || cb_send_szline < 0
     || cb_cancel_sgline < 0 || cb_cancel_sqline < 0 || cb_cancel_szline < 0
    ) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "connect", do_connect)
     || !add_callback(NULL, "user check", do_user_check)
     || !add_callback(NULL, "user nickchange (after)", do_user_nickchange_after)
     || !add_callback(module_operserv, "expire maskdata", do_expire_maskdata)
     || !add_callback(module_operserv, "HELP", do_help)
     || !add_callback(module_operserv, "STATS ALL", do_stats_all)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    if (!register_dbtable(&sgline_dbtable)
     || !register_dbtable(&sqline_dbtable)
     || !register_dbtable(&szline_dbtable)
    ) {
        module_log("Unable to register database tables");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{

    unregister_dbtable(&szline_dbtable);
    unregister_dbtable(&sqline_dbtable);
    unregister_dbtable(&sgline_dbtable);

    if (module_nickserv)
        do_unload_module(module_nickserv);

    remove_callback(NULL, "user nickchange (after)", do_user_nickchange_after);
    remove_callback(NULL, "user check", do_user_check);
    remove_callback(NULL, "connect", do_connect);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_callback(cb_cancel_szline);
    unregister_callback(cb_cancel_sqline);
    unregister_callback(cb_cancel_sgline);
    unregister_callback(cb_send_szline);
    unregister_callback(cb_send_sqline);
    unregister_callback(cb_send_sgline);

    if (module_operserv) {
        remove_callback(module_operserv, "STATS ALL", do_stats_all);
        remove_callback(module_operserv, "HELP", do_help);
        remove_callback(module_operserv,"expire maskdata",do_expire_maskdata);
        unregister_commands(module_operserv, cmds);
        unuse_module(module_operserv);
        module_operserv = NULL;
    }

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
