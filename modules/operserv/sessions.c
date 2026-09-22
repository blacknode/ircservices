/* Session limiting module.
 * Based on code copyright (c) 1999-2000 Andrew Kempe (TheShadow)
 *     E-mail: <andrewk@isdial.net>
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
#include "maskdata.h"
#include "akill.h"

/*************************************************************************/

/* SESSION LIMITING
 *
 * The basic idea of session limiting is to prevent one host from having more
 * than a specified number of sessions (client connections/clones) on the
 * network at any one time. To do this we have a list of sessions and
 * exceptions. Each session structure records information about a single host,
 * including how many clients (sessions) that host has on the network. When a
 * host reaches it's session limit, no more clients from that host will be
 * allowed to connect.
 *
 * When a client connects to the network, we check to see if their host has
 * reached the default session limit per host, and thus whether it is allowed
 * any more. If it has reached the limit, we kill the connecting client; all
 * the other clients are left alone. Otherwise we simply increment the counter
 * within the session structure. When a client disconnects, we decrement the
 * counter. When the counter reaches 0, we free the session.
 *
 * Exceptions allow one to specify custom session limits for a specific host
 * or a range thereof. The first exception that the host matches is the one
 * used.
 *
 * "Session Limiting" is likely to slow down services when there are frequent
 * client connects and disconnects. The size of the exception list can also
 * play a large role in this performance decrease. It is therefore recommened
 * that you keep the number of exceptions to a minimum. A very simple hashing
 * method is currently used to store the list of sessions. I'm sure there is
 * room for improvement and optimisation of this, along with the storage of
 * exceptions. Comments and suggestions are more than welcome!
 *
 * -TheShadow (02 April 1999)
 */

/*************************************************************************/

static Module *module_operserv;
static Module *module_akill;

/* create_kill() imported from operserv/akill */
static void (*p_create_akill)(char *mask, const char *reason, const char *who,
                              time_t expiry);

static int    WallOSException;
static int    WallExceptionExpire;
static int32  DefSessionLimit;
static time_t ExceptionExpiry;
static int32  MaxSessionLimit;
static char * SessionLimitExceeded;
static char * SessionLimitDetailsLoc;
static int    SessionLimitAutokill;
static time_t SessionLimitMinKillTime;
static int32  SessionLimitMaxKillCount;
static time_t SessionLimitAutokillExpiry;
static char * SessionLimitAutokillReason;

/*************************************************************************/

typedef struct session_ Session;
struct session_ {
    Session *prev, *next;
    char *host;
    int count;                  /* Number of clients with this host */
    int killcount;              /* Number of kills for this session */
    time_t lastkill;            /* Time of last kill */
};

#define HASH_STATIC static
#include "hash.h"
#define add_session _add_session
#define del_session _del_session
DEFINE_HASH(session, Session, host)
#undef del_session
#undef add_session

static void do_session(User *u);
static void do_exception(User *u);

static Command cmds[] = {
    {"SESSION",   do_session,   is_services_oper, OPER_HELP_SESSION,   -1,-1},
    {"EXCEPTION", do_exception, is_services_oper, OPER_HELP_EXCEPTION, -1,-1},
    {NULL}
};

/*************************************************************************/
/************************* Session list display **************************/
/*************************************************************************/

/* Syntax: SESSION LIST threshold
 *      Lists all sessions with atleast threshold clients.
 *      The threshold value must be greater than 1. This is to prevent
 *      accidental listing of the large number of single client sessions.
 *
 * Syntax: SESSION VIEW host
 *      Displays detailed session information about the supplied host.
 */

static void do_session(User *u)
{
    Session *session;
    MaskData *exception;
    const char *cmd = strtok(NULL, " ");
    const char *param1 = strtok(NULL, " ");
    int mincount;

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "LIST") == 0) {
        if (!param1) {
            syntax_error(s_OperServ, u, "SESSION", OPER_SESSION_LIST_SYNTAX);

        } else if ((mincount = atoi(param1)) <= 1) {
            notice_lang(s_OperServ, u, OPER_SESSION_INVALID_THRESHOLD);

        } else {
            notice_lang(s_OperServ, u, OPER_SESSION_LIST_HEADER, mincount);
            notice_lang(s_OperServ, u, OPER_SESSION_LIST_COLHEAD);
            for (session = first_session(); session; session = next_session()){
                if (session->count >= mincount)
                    notice_lang(s_OperServ, u, OPER_SESSION_LIST_FORMAT,
                                session->count, session->host);
            }
        }
    } else if (stricmp(cmd, "VIEW") == 0) {
        if (!param1) {
            syntax_error(s_OperServ, u, "SESSION", OPER_SESSION_VIEW_SYNTAX);
        } else {
            session = get_session(param1);
            if (!session) {
                notice_lang(s_OperServ, u, OPER_SESSION_NOT_FOUND, param1);
            } else {
                exception = get_matching_maskdata(MD_EXCEPTION, param1);
                notice_lang(s_OperServ, u, OPER_SESSION_VIEW_FORMAT,
                            param1, session->count,
                            exception ? exception->limit : DefSessionLimit);
                put_maskdata(exception);
            }
        }
    } else {
        syntax_error(s_OperServ, u, "SESSION", OPER_SESSION_SYNTAX);
    }
}

/*************************************************************************/
/********************* Internal session functions ************************/
/*************************************************************************/

/* Free a session structure.  Separate from del_session() because the
 * module cleanup code also uses it.
 */

static inline void free_session(Session *session)
{
    free(session->host);
    free(session);
}

/*************************************************************************/

/* Attempt to add a host to the session list. If the addition of the new host
 * causes the the session limit to be exceeded, kill the connecting user.
 * Returns 1 if the host was added or 0 if the user was killed.
 */

static int add_session(const char *nick, const char *host)
{
    Session *session;
    MaskData *exception;
    int sessionlimit = 0;
    char buf[BUFSIZE];
    time_t now = time(NULL);

    session = get_session(host);

    if (session) {
        exception = get_matching_maskdata(MD_EXCEPTION, host);
        sessionlimit = exception ? exception->limit : DefSessionLimit;
        put_maskdata(exception);

        if (sessionlimit != 0 && session->count >= sessionlimit) {
            if (SessionLimitExceeded)
                notice(s_OperServ, nick, SessionLimitExceeded, host);
            if (SessionLimitDetailsLoc)
                notice(s_OperServ, nick, SessionLimitDetailsLoc);

            if (SessionLimitAutokill && module_akill) {
                if (now <= session->lastkill + SessionLimitMinKillTime) {
                    session->killcount++;
                    if (session->killcount >= SessionLimitMaxKillCount) {
                        snprintf(buf, sizeof(buf), "*@%s", host);
                        p_create_akill(buf,SessionLimitAutokillReason,
                                       s_OperServ,
                                       now + SessionLimitAutokillExpiry);
                        session->killcount = 0;
                    }
                } else {
                    session->killcount = 1;
                }
                session->lastkill = now;
            }

            /* We don't use kill_user() because a user stucture has not yet
             * been created. Simply kill the user. -TheShadow */
            send_cmd(s_OperServ, "KILL %s :%s (Session limit exceeded)",
                     nick, s_OperServ);
            return 0;
        } else {
            session->count++;
            return 1;
        }
        /* not reached */
    }

    /* Session does not exist, so create it */
    session = scalloc(sizeof(Session), 1);
    session->host = sstrdup(host);
    session->count = 1;
    session->killcount = 0;
    session->lastkill = 0;
    _add_session(session);

    return 1;
}

/*************************************************************************/

static void del_session(const char *host)
{
    Session *session;

    module_log_debug(2, "del_session() called");

    session = get_session(host);
    if (!session) {
        wallops(s_OperServ,
                "WARNING: Tried to delete non-existent session: \2%s", host);
        module_log("Tried to delete non-existent session: %s", host);
        return;
    }

    if (session->count > 1) {
        session->count--;
        return;
    }
    _del_session(session);
    module_log_debug(2, "del_session(): free session structure");
    free_session(session);

    module_log_debug(2, "del_session() done");
}


/*************************************************************************/
/************************ Exception manipulation *************************/
/*************************************************************************/

/* Syntax: EXCEPTION ADD [+expiry] mask limit reason
 *      Adds mask to the exception list with limit as the maximum session
 *      limit and +expiry as an optional expiry time.
 *
 * Syntax: EXCEPTION DEL mask
 *      Deletes the first exception that matches mask exactly.
 *
 * Syntax: EXCEPTION MOVE num newnum
 *      Moves the exception with number num to have number newnum.
 *
 * Syntax: EXCEPTION LIST [mask]
 *      Lists all exceptions or those matching mask.
 *
 * Syntax: EXCEPTION VIEW [mask]
 *      Displays detailed information about each exception or those matching
 *      mask.
 *
 * Syntax: EXCEPTION CHECK host
 *      Displays masks which the given host matches.
 *
 * Syntax: EXCEPTION COUNT
 *      Displays the number of entries in the exception list.
 */

static void do_exception_add(User *u);
static void do_exception_del(User *u);
static void do_exception_clear(User *u);
static void do_exception_move(User *u);
static void do_exception_list(User *u, int is_view);
static void do_exception_check(User *u);
static int exception_del_callback(int num, va_list args);
static int exception_list(User *u, MaskData *except, int *count, int skip,
                          int is_view);
static int exception_list_callback(int num, va_list args);

static void do_exception(User *u)
{
    const char *cmd = strtok(NULL, " ");

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "ADD") == 0) {
        do_exception_add(u);
    } else if (stricmp(cmd, "DEL") == 0) {
        do_exception_del(u);
    } else if (stricmp(cmd, "CLEAR") == 0) {
        do_exception_clear(u);
    } else if (stricmp(cmd, "MOVE") == 0) {
        do_exception_move(u);
    } else if (stricmp(cmd, "LIST") == 0 || stricmp(cmd, "VIEW") == 0) {
        do_exception_list(u, stricmp(cmd,"VIEW")==0);
    } else if (stricmp(cmd, "CHECK") == 0) {
        do_exception_check(u);
    } else if (stricmp(cmd, "COUNT") == 0) {
        int count = maskdata_count(MD_EXCEPTION);
        if (count)
            notice_lang(s_OperServ, u, OPER_EXCEPTION_COUNT, count);
        else
            notice_lang(s_OperServ, u, OPER_EXCEPTION_EMPTY);
    } else {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_SYNTAX);
    }
}

/*************************************************************************/

static void do_exception_add(User *u)
{
    char *mask, *reason, *expiry, *limitstr;
    time_t expires;
    int limit, i;
    MaskData *except;
    time_t now = time(NULL);

    if (maskdata_count(MD_EXCEPTION) >= MAX_MASKDATA) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_TOO_MANY);
        return;
    }

    mask = strtok(NULL, " ");
    if (mask && *mask == '+') {
        expiry = mask+1;
        mask = strtok(NULL, " ");
    } else {
        expiry = NULL;
    }
    limitstr = strtok(NULL, " ");
    reason = strtok_remaining();

    if (!mask || !limitstr || !reason) {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_ADD_SYNTAX);
        return;
    }

    expires = expiry ? dotime(expiry) : ExceptionExpiry;
    if (expires < 0) {
        notice_lang(s_OperServ, u, BAD_EXPIRY_TIME);
        return;
    } else if (expires > 0) {
        expires += now;
    }

    limit = (limitstr && isdigit(*limitstr)) ? atoi(limitstr) : -1;

    if (limit < 0 || limit > MaxSessionLimit) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_INVALID_LIMIT,
                    MaxSessionLimit);
        return;

    } else if (strchr(mask, '!') || strchr(mask, '@')) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_INVALID_HOSTMASK);
        return;
    } else if (put_maskdata(get_maskdata(MD_EXCEPTION, strlower(mask)))) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_ALREADY_PRESENT, mask);
    } else {
        /* Get highest maskdata number */
        i = 0;
        for (except = first_maskdata(MD_EXCEPTION); except;
             except = next_maskdata(MD_EXCEPTION)
        ) {
            if (except->num > i)  /* should be in order, but let's be safe */
                i = except->num;
        }
        /* Add exception with next number */
        except = scalloc(1, sizeof(*except));
        except->mask = sstrdup(mask);
        except->limit = limit;
        except->reason = sstrdup(reason);
        except->time = now;
        strbcpy(except->who, u->nick);
        except->expires = expires;
        except->num = i+1;
        add_maskdata(MD_EXCEPTION, except);
        if (WallOSException) {
            char buf[BUFSIZE];
            expires_in_lang(buf, sizeof(buf), NULL, expires);
            wallops(s_OperServ, "%s added a session limit exception of"
                    " \2%d\2 for \2%s\2 (%s)", u->nick, limit, mask, buf);
        }
        notice_lang(s_OperServ, u, OPER_EXCEPTION_ADDED, mask, limit);
        if (readonly)
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
    }
}

/*************************************************************************/

static void do_exception_del(User *u)
{
    char *mask;
    MaskData *except;
    int deleted = 0;

    mask = strtok(NULL, " ");
    if (!mask) {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_DEL_SYNTAX);
        return;
    }
    if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
        int count, last = -1;
        deleted = process_numlist(mask, &count, exception_del_callback, &last);
        if (deleted == 0) {
            if (count == 1) {
                notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_SUCH_ENTRY, last);
            } else {
                notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_MATCH);
            }
        } else if (deleted == 1) {
            notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED_ONE);
        } else {
            notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED_SEVERAL,
                        deleted);
        }
    } else {
        for (except = first_maskdata(MD_EXCEPTION); except;
             except = next_maskdata(MD_EXCEPTION)
        ) {
            if (stricmp(mask, except->mask) == 0) {
                del_maskdata(MD_EXCEPTION, except);
                notice_lang(s_OperServ, u, OPER_EXCEPTION_DELETED, mask);
                deleted = 1;
                break;
            }
        }
        if (deleted == 0)
            notice_lang(s_OperServ, u, OPER_EXCEPTION_NOT_FOUND, mask);
    }
    if (deleted && readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);

    /* Renumber the exception list. I don't believe in having holes in
     * lists - it makes code more complex, harder to debug and we end up
     * with huge index numbers. Imho, fixed numbering is only beneficial
     * when one doesn't have range capable manipulation. -TheShadow */

    /* That works fine up until two people do deletes at the same time
     * and shoot themselves in the collective foot; and just because
     * you have range handling doesn't mean someone won't do "DEL 5"
     * followed by "DEL 7" and, again, shoot themselves in the foot.
     * Besides, there's nothing wrong with complexity if it serves a
     * purpose.  Removed. --AC */
}


static int exception_del_callback(int num, va_list args)
{
    MaskData *except;
    int *last = va_arg(args, int *);

    *last = num;
    if ((except = get_exception_by_num(num)) != NULL) {
        del_maskdata(MD_EXCEPTION, except);
        return 1;
    } else {
        return 0;
    }
}

/*************************************************************************/

static void do_exception_clear(User *u)
{
    char *mask;
    MaskData *except;

    mask = strtok(NULL, " ");
    if (!mask || stricmp(mask,"ALL") != 0) {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_CLEAR_SYNTAX);
        return;
    }
    for (except = first_maskdata(MD_EXCEPTION); except;
         except = next_maskdata(MD_EXCEPTION)
    ) {
        del_maskdata(MD_EXCEPTION, except);
    }
    notice_lang(s_OperServ, u, OPER_EXCEPTION_CLEARED);
    if (readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);
}

/*************************************************************************/

static void do_exception_move(User *u)
{
    MaskData *except;
    char *n1str = strtok(NULL, " ");    /* From index */
    char *n2str = strtok(NULL, " ");    /* To index */
    int n1, n2;

    if (!n1str || !n2str) {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_MOVE_SYNTAX);
        return;
    }
    n1 = atoi(n1str);
    n2 = atoi(n2str);
    if (n1 == n2 || n1 <= 0 || n2 <= 0) {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_MOVE_SYNTAX);
        return;
    }
    if (!(except = get_exception_by_num(n1))) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_NO_SUCH_ENTRY, n1);
        return;
    }
    except = move_exception(except, n2);
    notice_lang(s_OperServ, u, OPER_EXCEPTION_MOVED,
                except->mask, n1, n2);
    put_maskdata(except);
    if (readonly)
        notice_lang(s_OperServ, u, READ_ONLY_MODE);
}

/*************************************************************************/

static void do_exception_list(User *u, int is_view)
{
    char *mask = strtok(NULL, " ");
    char *s;
    MaskData *except;
    int count = 0, skip = 0;
    int noexpire = 0;


    if (mask && *mask == '+') {
        skip = (int)atolsafe(mask+1, 0, INT_MAX);
        if (skip < 0) {
            syntax_error(s_OperServ, u, "EXCEPTION",
                         OPER_EXCEPTION_LIST_SYNTAX);
            return;
        }
        mask = strtok(NULL, " ");
    }

    if (mask)
        strlower(mask);

    if ((s = strtok(NULL," ")) != NULL && stricmp(s,"NOEXPIRE") == 0)
        noexpire = 1;

    if (mask && strspn(mask, "1234567890,-") == strlen(mask)) {
        if (skip) {
            syntax_error(s_OperServ, u, "EXCEPTION",
                         OPER_EXCEPTION_LIST_SYNTAX);
            return;
        }
        process_numlist(mask, NULL, exception_list_callback, u,
                        &count, skip, noexpire, is_view);
    } else {
        for (except = first_maskdata(MD_EXCEPTION); except;
             except = next_maskdata(MD_EXCEPTION)
        ) {
            if ((!mask || match_wild(mask, except->mask))
             && (!noexpire || !except->expires))
                exception_list(u, except, &count, skip, is_view);
        }
    }
    if (!count)
        notice_lang(s_OperServ, u,
                    mask ? OPER_EXCEPTION_NO_MATCH : OPER_EXCEPTION_EMPTY);
}


static int exception_list(User *u, MaskData *except, int *count, int skip,
                          int is_view)
{
    if (!*count) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_HEADER);
        if (!is_view)
            notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_COLHEAD);
    }
    (*count)++;
    if (*count > skip && *count <= skip+ListMax) {
        if (is_view) {
            char timebuf[BUFSIZE], expirebuf[BUFSIZE];
            strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                          STRFTIME_SHORT_DATE_FORMAT, except->time);
            expires_in_lang(expirebuf, sizeof(expirebuf), u->ngi,
                            except->expires);
            notice_lang(s_OperServ, u, OPER_EXCEPTION_VIEW_FORMAT,
                        except->num, except->mask,
                        *except->who ? except->who : "<unknown>",
                        timebuf, expirebuf, except->limit, except->reason);
        } else { /* list */
            notice_lang(s_OperServ, u, OPER_EXCEPTION_LIST_FORMAT,
                        except->num, except->limit, except->mask);
        }
    }
    return 1;
}


static int exception_list_callback(int num, va_list args)
{
    User *u = va_arg(args, User *);
    int *count = va_arg(args, int *);
    int skip = va_arg(args, int);
    int noexpire = va_arg(args, int);
    int is_view = va_arg(args, int);
    MaskData *except;
    int retval = 0;

    if ((except = get_exception_by_num(num)) != NULL) {
        if (!noexpire || !except->expires)
            retval = exception_list(u, except, count, skip, is_view);
        put_maskdata(except);
    }
    return retval;
}

/*************************************************************************/

static void do_exception_check(User *u)
{
    char *host;
    MaskData *except;
    int sent_header = 0, count = 0;

    host = strtok(NULL, " ");
    if (host) {
        strlower(host);
    } else {
        syntax_error(s_OperServ, u, "EXCEPTION", OPER_EXCEPTION_CHECK_SYNTAX);
        return;
    }
    for (except = first_maskdata(MD_EXCEPTION); except;
         except = next_maskdata(MD_EXCEPTION)
    ) {
        if (match_wild(except->mask, host)) {
            if (!sent_header) {
                notice_lang(s_OperServ, u, OPER_EXCEPTION_CHECK_HEADER, host);
                sent_header = 1;
            }
            notice(s_OperServ, u->nick, "    %s", except->mask);
            count++;
        }
    }
    if (sent_header) {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_CHECK_TRAILER, count);
    } else {
        notice_lang(s_OperServ, u, OPER_EXCEPTION_CHECK_NO_MATCH, host);
    }
}

/*************************************************************************/
/*************************** Callback routines ***************************/
/*************************************************************************/

/* Callback to check session limiting for new users. */

static int check_sessions(int ac, char **av)
{
    return !add_session(av[0], av[4]);
}

/*************************************************************************/

/* Callback to remove a quitting user's session. */

static int remove_session(User *u, char *reason_unused)
{
    del_session(u->host);
    return 0;
}

/*************************************************************************/

/* Callback for exception expiration. */

static int do_expire_maskdata(uint32 type, MaskData *md)
{
    if (type == MD_EXCEPTION) {
        if (WallExceptionExpire)
            wallops(s_OperServ, "Session limit exception for %s has expired",
                    md->mask);
    }
    return 0;
}

/*************************************************************************/

/* Callback for OperServ STATS ALL command. */

static int do_stats_all(User *u)
{
    int32 count, mem;
    Session *session;
    MaskData *md;

    count = mem = 0;
    for (session = first_session(); session; session = next_session()) {
        count++;
        mem += sizeof(*session) + strlen(session->host)+1;
    }
    notice_lang(s_OperServ, u, OPER_STATS_ALL_SESSION_MEM,
                count, (mem+512) / 1024);

    for (md = first_maskdata(MD_EXCEPTION); md;
         md = next_maskdata(MD_EXCEPTION)
    ) {
        count++;
        mem += sizeof(*md);
        if (md->mask)
            mem += strlen(md->mask)+1;
        if (md->reason)
            mem += strlen(md->reason)+1;
    }
    notice_lang(s_OperServ, u, OPER_STATS_ALL_EXCEPTION_MEM,
                count, (mem+512) / 1024);

    return 0;
}

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

static void insert_exception(void *md) {
    MaskData *except;
    int num = 0;
    for (except = first_maskdata(MD_EXCEPTION); except;
         except = next_maskdata(MD_EXCEPTION)
    ) {
        if (except->num > num)  /* should be in order, but let's be safe */
            num = except->num;
    }
    ((MaskData *)md)->num = num+1;
    add_maskdata(MD_EXCEPTION, md);
}
static void *first_exception(void)
    { return first_maskdata(MD_EXCEPTION); }
static void *next_exception(void)
    { return next_maskdata(MD_EXCEPTION); }

static DBField exception_dbfields[] = {
    { "mask",     DBTYPE_STRING, offsetof(MaskData,mask) },
    { "limit",    DBTYPE_INT16,  offsetof(MaskData,limit) },
    { "reason",   DBTYPE_STRING, offsetof(MaskData,reason) },
    { "who",      DBTYPE_BUFFER, offsetof(MaskData,who), NICKMAX },
    { "time",     DBTYPE_TIME,   offsetof(MaskData,time) },
    { "expires",  DBTYPE_TIME,   offsetof(MaskData,expires) },
    { "lastused", DBTYPE_TIME,   offsetof(MaskData,lastused) },
    { NULL }
};
static DBTable exception_dbtable = {
    .name    = "exception",
    .newrec  = new_maskdata,
    .freerec = free_maskdata,
    .insert  = insert_exception,
    .first   = first_exception,
    .next    = next_exception,
    .fields  = exception_dbfields,
};

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "DefSessionLimit",  { { CD_INT, 0, &DefSessionLimit } } },
    { "ExceptionExpiry",  { { CD_TIME, 0, &ExceptionExpiry } } },
    { "MaxSessionLimit",  { { CD_POSINT, 0, &MaxSessionLimit } } },
    { "SessionLimitAutokill",{{CD_SET, 0, &SessionLimitAutokill },
                            { CD_TIME, 0, &SessionLimitMinKillTime },
                            { CD_POSINT, 0, &SessionLimitMaxKillCount },
                            { CD_TIME, 0, &SessionLimitAutokillExpiry },
                            { CD_STRING, 0, &SessionLimitAutokillReason } } },
    { "SessionLimitDetailsLoc",{{CD_STRING, 0, &SessionLimitDetailsLoc } } },
    { "SessionLimitExceeded",{{CD_STRING, 0, &SessionLimitExceeded } } },
    { "WallExceptionExpire",{{CD_SET, 0, &WallExceptionExpire } } },
    { "WallOSException",  { { CD_SET, 0, &WallOSException } } },
    { NULL }
};

/*************************************************************************/

static int do_load_module(Module *mod, const char *name)
{
    if (strcmp(name, "operserv/akill") == 0) {
        p_create_akill = get_module_symbol(mod, "create_akill");
        if (p_create_akill)
            module_akill = mod;
        else
            module_log("Symbol `create_akill' not found, automatic autokill"
                       " addition will not be available");
    }
    return 0;
}

/*************************************************************************/

static int do_unload_module(Module *mod)
{
    if (mod == module_akill) {
        p_create_akill = NULL;
        module_akill = NULL;
    }
    return 0;
}

/*************************************************************************/

int init_module()
{
    if (!MaxSessionLimit)
        MaxSessionLimit = MAX_MASKDATA_LIMIT;

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

    /* Add user check callback at priority -10 so it runs after all the
     * autokill/S-line/whatever checks (otherwise we get users added to
     * sessions and then killed by S-lines, leaving the session count
     * jacked up).
     */
    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback_pri(NULL, "user check", check_sessions, -10)
     || !add_callback(NULL, "user delete", remove_session)
     || !add_callback(module_operserv, "expire maskdata", do_expire_maskdata)
     || !add_callback(module_operserv, "STATS ALL", do_stats_all)
    ) {
        module_log("Unable to add callbacks");
        exit_module(0);
        return 0;
    }

    module_akill = find_module("operserv/akill");
    if (module_akill)
        do_load_module(module_akill, "operserv/akill");

    if (!register_dbtable(&exception_dbtable)) {
        module_log("Unable to register database table");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    Session *session;

    unregister_dbtable(&exception_dbtable);

    if (module_akill)
        do_unload_module(module_akill);

    for (session = first_session(); session; session = next_session()) {
        _del_session(session);
        free_session(session);
    }

    remove_callback(NULL, "user delete", remove_session);
    remove_callback(NULL, "user check", check_sessions);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    if (module_operserv) {
        remove_callback(module_operserv, "STATS ALL", do_stats_all);
        remove_callback(module_operserv, "expire maskdata",do_expire_maskdata);
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
