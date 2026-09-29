/* Main NickServ module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include <jansson.h>

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "commands.h"
#include "databases.h"
#include "db.h"
#include "encrypt.h"
#include "migration.h"
#include "store.h"
#include "timeout.h"
#include "language.h"
#include "modules/operserv/operserv.h"

#include "modules/nickserv/nickserv.h"
#include "modules/nickserv/ns-local.h"

/*************************************************************************/


static Event* check_expire_event;
static Event* command_event;
static Event* help_event;
static Event* help_cmds_event;
       Event* reglink_check_event;  /* emitted from util.c */
static Event* registered_event;
static Event* id_check_event;
static Event* identified_event;

       int32  NSRegEmailMax;
       int    NSRequireEmail;
       int    NSRegDenyIfSuspended;
       time_t NSRegDelay;
       time_t NSInitialRegDelay;
       time_t NSSetEmailDelay;
       int32  NSDefFlags;
       time_t NSExpire;
       time_t NSExpireWarning;
       int    NSShowPassword;
       char * NSEnforcerUser;
       char * NSEnforcerHost;
       int    NSForceNickChange;
       time_t NSReleaseTimeout;
       int    NSAllowKillImmed;
       int    NSListOpersOnly;
       int    NSSecureAdmins;
       time_t NSSuspendExpire;
       time_t NSSuspendGrace;
static int    NSHelpWarning;
static int    NSEnableDropEmail;
static time_t NSDropEmailExpire;

/*************************************************************************/

/* The channels a nick group founded (ngi->channels) are kept by ChanServ,
 * in the database, and read when they are needed.  Resolved at each call:
 * ChanServ comes and goes independently of us. */

static void refresh_owned_channels(NickGroupInfo *ngi)
{
    Module *mod = module_find("chanserv/main");
    void (*p_update)(NickGroupInfo *);

    if (!mod) {
        free(ngi->channels);
        ngi->channels = NULL;
        ngi->channels_count = 0;
        return;
    }
    p_update = (void (*)(NickGroupInfo *))
        module_symbol(mod, "update_owned_channels");
    if (p_update)
        p_update(ngi);
}

/*************************************************************************/

static void do_help(User *u);
static void do_register(User *u);
static void do_identify(User *u);
static void do_drop(User *u);
static void do_dropnick(User *u);
static void do_dropemail(User *u);
static void do_dropemail_confirm(User *u);
static void do_info(User *u);
static void do_listchans(User *u);
static void do_list(User *u);
static void do_listemail(User *u);
static void do_recover(User *u);
static void do_release(User *u);
static void do_ghost(User *u);
static void do_status(User *u);
static void do_getpass(User *u);
static void do_forbid(User *u);
static void do_suspend(User *u);
static void do_unsuspend(User *u);
#ifdef DEBUG_COMMANDS
static void do_listnick(User *u);
#endif

/*************************************************************************/

static Command cmds[] = {
    { "HELP",     do_help,     NULL,  -1,                     -1,-1 },
    { "REGISTER", do_register, NULL,  NICK_HELP_REGISTER,     -1,-1 },
    { "IDENTIFY", do_identify, NULL,  NICK_HELP_IDENTIFY,     -1,-1 },
    { "SIDENTIFY",do_identify, NULL,  -1,                     -1,-1 },
    { "DROP",     do_drop,     NULL,  NICK_HELP_DROP,         -1,-1 },
    { "SET",      do_set,      NULL,  NICK_HELP_SET,
                -1, NICK_OPER_HELP_SET },
    { "SET PASSWORD", NULL,    NULL,  NICK_HELP_SET_PASSWORD, -1,-1 },
    { "SET URL",      NULL,    NULL,  NICK_HELP_SET_URL,      -1,-1 },
    { "SET EMAIL",    NULL,    NULL,  NICK_HELP_SET_EMAIL,    -1,-1 },
    { "SET INFO",     NULL,    NULL,  NICK_HELP_SET_INFO,     -1,-1 },
    { "SET KILL",     NULL,    NULL,  NICK_HELP_SET_KILL,     -1,-1 },
    { "SET SECURE",   NULL,    NULL,  NICK_HELP_SET_SECURE,   -1,-1 },
    { "SET PRIVATE",  NULL,    NULL,  NICK_HELP_SET_PRIVATE,  -1,-1 },
    { "SET NOOP",     NULL,    NULL,  NICK_HELP_SET_NOOP,     -1,-1 },
    { "SET HIDE",     NULL,    NULL,  NICK_HELP_SET_HIDE,     -1,-1 },
    { "SET TIMEZONE", NULL,    NULL,  NICK_HELP_SET_TIMEZONE, -1,-1 },
    { "SET NOEXPIRE", NULL,    NULL,  -1, -1,
                NICK_OPER_HELP_SET_NOEXPIRE },
    { "UNSET",    do_unset,    NULL,  NICK_HELP_UNSET,
                -1, NICK_OPER_HELP_UNSET },
    { "RECOVER",  do_recover,  NULL,  NICK_HELP_RECOVER,      -1,-1 },
    { "RELEASE",  do_release,  NULL,  NICK_HELP_RELEASE,      -1,-1 },
    { "GHOST",    do_ghost,    NULL,  NICK_HELP_GHOST,        -1,-1 },
    { "INFO",     do_info,     NULL,  NICK_HELP_INFO,
                -1, NICK_OPER_HELP_INFO },
    { "LIST",     do_list,     NULL,  -1,
                NICK_HELP_LIST, NICK_OPER_HELP_LIST },
    { "LISTEMAIL",do_listemail,NULL,  NICK_HELP_LISTEMAIL,    -1,-1 },
    { "STATUS",   do_status,   NULL,  NICK_HELP_STATUS,       -1,-1 },
    { "LISTCHANS",do_listchans,NULL,  NICK_HELP_LISTCHANS,
                -1, NICK_OPER_HELP_LISTCHANS },

    { "DROPNICK", do_dropnick, is_services_admin, -1,
                -1, NICK_OPER_HELP_DROPNICK },
    { "DROPEMAIL",do_dropemail,is_services_admin, -1,
                -1, NICK_OPER_HELP_DROPEMAIL },
    { "DROPEMAIL-CONFIRM", do_dropemail_confirm, is_services_admin, -1,
                -1, NICK_OPER_HELP_DROPEMAIL },
    { "GETPASS",  do_getpass,  is_services_admin, -1,
                -1, NICK_OPER_HELP_GETPASS },
    { "FORBID",   do_forbid,   is_services_admin, -1,
                -1, NICK_OPER_HELP_FORBID },
    { "SUSPEND",  do_suspend,  is_services_admin, -1,
                -1, NICK_OPER_HELP_SUSPEND },
    { "UNSUSPEND",do_unsuspend,is_services_admin, -1,
                -1, NICK_OPER_HELP_UNSUSPEND },
#ifdef DEBUG_COMMANDS
    { "LISTNICK", do_listnick, is_services_root, -1, -1, -1 },
#endif
    { NULL }
};

/* Command alias type and array */
typedef struct {
    char *alias, *command;
} Alias;
static Alias *aliases;
static int aliases_count;

/*************************************************************************/
/**************************** Database stuff *****************************/
/*************************************************************************/

/* Check whether the given nickname (or its suspension) should be expired,
 * and do the expiration if so.  Return 1 if the nickname was deleted, else
 * 0.  Note that we do last-seen-time updates here as well.
 */

static int check_expire_nick(NickInfo *ni)
{
    User *u = ni->user;
    NickGroupInfo *ngi;
    time_t now = time(NULL);

    /* Not on every lookup: a last-seen time that moved every second would
     * be a database write every time the record is looked at.  The time is
     * set exactly when the user leaves (cancel_user()). */
    if (u && user_id_or_rec(u) && now - ni->last_seen >= 300) {
        module_log_debug(2, "updating last seen time for %s", u->nick);
        ni->last_seen = now;
    }
    ngi = ni->nickgroup ? get_ngi_id(ni->nickgroup) : NULL;
    if (!ServicesRoot || irc_stricmp(ni->nick, ServicesRoot) != 0) {
        if (event_emit(check_expire_event, ni, ngi) > 0) {
            put_nickgroupinfo(ngi);
            if (u)
                notice_lang(nickserv_service.nick, u, NICK_EXPIRED);
            delnick(ni);
            return 1;
        }
        if (NSExpire
         && now >= ni->last_seen + NSExpire
         && !(ni->status & (NS_VERBOTEN | NS_NOEXPIRE))
         && (!ngi || !(ngi->flags & NF_SUSPENDED))
        ) {
            put_nickgroupinfo(ngi);
            module_log("Expiring nickname %s", ni->nick);
            if (u)
                notice_lang(nickserv_service.nick, u, NICK_EXPIRED);
            delnick(ni);
            return 1;
        }
    }
    if (ngi && (ngi->flags & NF_SUSPENDED)
     && ngi->suspend_expires
     && now >= ngi->suspend_expires
    ) {
        module_log("Expiring suspension for %s (nick group %u)",
                   ni->nick, ngi->id);
        unsuspend_nick(ngi, 1);
    }
    put_nickgroupinfo(ngi);
    return 0;
}

/*************************************************************************/

/* NickServ's records live in the entity store (include/store.h): the
 * database is the truth, Redis a copy, and memory holds only the nicks and
 * groups somebody is using.  get_*() pins a record, put_*() lets it go;
 * there is no longer any way to go through every record in memory, and
 * the few things that must look at many records ask the database
 * (store_foreach(), store_count()).  The tables are created by this
 * module's migrations (migrations/), applied when it is loaded
 * (MODULE_APPLY_MIGRATIONS in module_info below). */

/* How often expired nicknames and suspensions are looked for, and how many
 * are handled per round. */
#define EXPIRE_INTERVAL  600
#define EXPIRE_BATCH     500

/* Database load/save helpers */

static void db_get_mainnick(const void *record, void **value_ret)
{
    NickGroupInfo *ngi = (NickGroupInfo *)record;
    memset((char *)value_ret, 0, NICKMAX);
    if (ngi->nicks_count > 0 && ngi->mainnick < ngi->nicks_count)
        strscpy((char *)value_ret, ngi->nicks[ngi->mainnick], NICKMAX);
}

static void db_put_mainnick(void *record, const void *value)
{
    NickGroupInfo *ngi = (NickGroupInfo *)record;
    int i;
    ARRAY_FOREACH (i, ngi->nicks) {
        if (irc_stricmp((const char *)value, ngi->nicks[i]) == 0) {
            ngi->mainnick = i;
            return;
        }
    }
    ARRAY_EXTEND(ngi->nicks);
    ngi->mainnick = ngi->nicks_count - 1;
    strbcpy(ngi->nicks[ngi->mainnick], (const char *)value);
}

/* Nickgroup fields (the nickgroups table) */

#define FIELD(name,type,...) \
    { #name, type, offsetof(NickGroupInfo,name) , ##__VA_ARGS__ }
static DBField nickgroup_dbfields[] = {
    FIELD(id,              DBTYPE_UINT32),
    FIELD(mainnick,        DBTYPE_BUFFER, NICKMAX,
          .get = db_get_mainnick, .put = db_put_mainnick),
    FIELD(pass,            DBTYPE_PASSWORD),
    FIELD(url,             DBTYPE_STRING),
    FIELD(email,           DBTYPE_STRING),
    FIELD(last_email,      DBTYPE_STRING),
    FIELD(info,            DBTYPE_STRING),
    FIELD(flags,           DBTYPE_INT32),
    FIELD(os_priv,         DBTYPE_INT16),
    FIELD(authcode,        DBTYPE_INT32),
    FIELD(authset,         DBTYPE_TIME),
    FIELD(authreason,      DBTYPE_INT16),
    FIELD(suspend_who,     DBTYPE_BUFFER, NICKMAX),
    FIELD(suspend_reason,  DBTYPE_STRING),
    FIELD(suspend_time,    DBTYPE_TIME),
    FIELD(suspend_expires, DBTYPE_TIME),
    FIELD(language,        DBTYPE_INT16),
    FIELD(timezone,        DBTYPE_INT16),
    FIELD(channelmax,      DBTYPE_INT16),
    { "memos.memomax",     DBTYPE_INT16,
      offsetof(NickGroupInfo,memos) + offsetof(MemoInfo,memomax) },
    { NULL }
};
#undef FIELD

/* Nickname fields (the nicks table) */

#define FIELD(name,type,...) \
    { #name, type, offsetof(NickInfo,name) , ##__VA_ARGS__ }
static DBField nick_dbfields[] = {
    FIELD(nick,            DBTYPE_BUFFER, NICKMAX),
    FIELD(status,          DBTYPE_INT16),
    FIELD(last_usermask,   DBTYPE_STRING),
    FIELD(last_realmask,   DBTYPE_STRING),
    FIELD(last_realname,   DBTYPE_STRING),
    FIELD(last_quit,       DBTYPE_STRING),
    FIELD(time_registered, DBTYPE_TIME),
    FIELD(last_seen,       DBTYPE_TIME),
    FIELD(nickgroup,       DBTYPE_UINT32),
    FIELD(id_stamp,        DBTYPE_UINT32),
    { NULL }
};
#undef FIELD

/* Memo fields (the nickgroup_memos table) */

#define FIELD(name,type,...) \
    { #name, type, offsetof(Memo,name) , ##__VA_ARGS__ }
static DBField memo_dbfields[] = {
    FIELD(number,    DBTYPE_UINT32),
    FIELD(flags,     DBTYPE_INT16),
    FIELD(time,      DBTYPE_TIME),
    FIELD(firstread, DBTYPE_TIME),
    FIELD(sender,    DBTYPE_BUFFER, NICKMAX),
    FIELD(channel,   DBTYPE_STRING),
    FIELD(text,      DBTYPE_STRING),
    { NULL }
};
#undef FIELD

/*************************************************************************/

/* Keys: a nick is looked up IRC-case-insensitively, so its key is the
 * nick in IRC lower case; a group's key is its ID. */

static void nick_normalize(const char *key, char *buf, size_t size)
{
    size_t n = 0;

    for (; *key && n + 1 < size; key++)
        buf[n++] = irc_tolower(*key);
    buf[n] = 0;
}

static void nick_keyof(const void *record, char *buf, size_t size)
{
    nick_normalize(((const NickInfo *)record)->nick, buf, size);
}

static void ngi_keyof(const void *record, char *buf, size_t size)
{
    snprintf(buf, size, "%u", ((const NickGroupInfo *)record)->id);
}

/* A nick: the one row. */

static json_t *nick_encode(const void *record)
{
    const NickInfo *ni = record;
    json_t *bundle = json_object(), *row;
    char key[NICKMAX*2];

    row = store_encode_fields(ni, nick_dbfields);
    nick_normalize(ni->nick, key, sizeof(key));
    json_object_set_new(row, "nick_key", store_json_string(key));
    json_object_set_new(bundle, "main", row);
    return bundle;
}

static void *nick_decode(json_t *bundle)
{
    NickInfo *ni = new_nickinfo();

    store_decode_fields(json_object_get(bundle, "main"), ni, nick_dbfields);
    if (!*ni->nick) {
        free_nickinfo(ni);
        return NULL;
    }
    return ni;
}

static void nick_release(void *record)
{
    free_nickinfo(record);
}

/* A group: its row, the names of its nicks (read-only here: they belong to
 * the nick records), and its lists -- access masks, autojoin channels,
 * memos, memo ignores. */

/* A list of strings as rows {nickgroup, idx, <column>: string}. */
static json_t *strings_encode(uint32 id, char **list, int count,
                              const char *column)
{
    json_t *rows = json_array();
    int i;

    for (i = 0; i < count; i++) {
        json_t *row = json_object();
        json_object_set_new(row, "nickgroup", json_integer(id));
        json_object_set_new(row, "idx", json_integer(i));
        json_object_set_new(row, column, store_json_string(list[i]));
        json_array_append_new(rows, row);
    }
    return rows;
}

static void strings_decode(json_t *rows, const char *column,
                           char ***list_ret, int16 *count_ret)
{
    size_t i;
    json_t *row;
    char **list = NULL;
    int16 count = 0;

    json_array_foreach(rows, i, row) {
        char *s = store_dup_string(json_object_get(row, column));
        if (!s)
            continue;
        list = srealloc(list, sizeof(*list) * (count+1));
        list[count++] = s;
    }
    *list_ret = list;
    *count_ret = count;
}

static json_t *ngi_encode(const void *record)
{
    const NickGroupInfo *ngi = record;
    json_t *bundle = json_object(), *nicks = json_array(), *memos;
    int i;

    json_object_set_new(bundle, "main",
                        store_encode_fields(ngi, nickgroup_dbfields));
    ARRAY_FOREACH (i, ngi->nicks) {
        json_t *row = json_object();
        char key[NICKMAX*2];
        nick_normalize(ngi->nicks[i], key, sizeof(key));
        json_object_set_new(row, "nick", store_json_string(ngi->nicks[i]));
        json_object_set_new(row, "nick_key", store_json_string(key));
        json_array_append_new(nicks, row);
    }
    json_object_set_new(bundle, "nicks", nicks);
    json_object_set_new(bundle, "access",
                        strings_encode(ngi->id, ngi->access,
                                       ngi->access_count, "mask"));
    json_object_set_new(bundle, "ajoin",
                        strings_encode(ngi->id, ngi->ajoin,
                                       ngi->ajoin_count, "channel"));
    json_object_set_new(bundle, "memo_ignore",
                        strings_encode(ngi->id, ngi->ignore,
                                       ngi->ignore_count, "mask"));
    memos = json_array();
    ARRAY_FOREACH (i, ngi->memos.memos) {
        json_t *row = store_encode_fields(&ngi->memos.memos[i],
                                          memo_dbfields);
        json_object_set_new(row, "nickgroup", json_integer(ngi->id));
        json_object_set_new(row, "idx", json_integer(i));
        json_array_append_new(memos, row);
    }
    json_object_set_new(bundle, "memos", memos);
    return bundle;
}

static void *ngi_decode(json_t *bundle)
{
    NickGroupInfo *ngi = new_nickgroupinfo(NULL);
    json_t *rows, *row;
    size_t i;

    /* The nicks first: the main row's `mainnick' is looked up in them. */
    json_array_foreach(json_object_get(bundle, "nicks"), i, row) {
        const char *nick = json_string_value(json_object_get(row, "nick"));
        if (!nick)
            continue;
        ARRAY_EXTEND(ngi->nicks);
        strbcpy(ngi->nicks[ngi->nicks_count-1], nick);
    }
    store_decode_fields(json_object_get(bundle, "main"), ngi,
                        nickgroup_dbfields);
    if (!ngi->id) {
        free_nickgroupinfo(ngi);
        return NULL;
    }
    strings_decode(json_object_get(bundle, "access"), "mask",
                   &ngi->access, &ngi->access_count);
    strings_decode(json_object_get(bundle, "ajoin"), "channel",
                   &ngi->ajoin, &ngi->ajoin_count);
    strings_decode(json_object_get(bundle, "memo_ignore"), "mask",
                   &ngi->ignore, &ngi->ignore_count);
    rows = json_object_get(bundle, "memos");
    json_array_foreach(rows, i, row) {
        Memo *m;
        ARRAY_EXTEND(ngi->memos.memos);
        m = &ngi->memos.memos[ngi->memos.memos_count-1];
        memset(m, 0, sizeof(*m));
        store_decode_fields(row, m, memo_dbfields);
    }
    return ngi;
}

static void ngi_release(void *record)
{
    free_nickgroupinfo(record);
}

static const StoreChild ngi_children[] = {
    { "nicks",       "nicks",                 "nickgroup", "nick_key",
      "nick, nick_key", 1 },
    { "access",      "nickgroup_access",      "nickgroup", "idx" },
    { "ajoin",       "nickgroup_ajoin",       "nickgroup", "idx" },
    { "memos",       "nickgroup_memos",       "nickgroup", "idx" },
    { "memo_ignore", "nickgroup_memo_ignore", "nickgroup", "idx" },
    { NULL }
};

static StoreType nick_type = {
    .name       = "nick",
    .table      = "nicks",
    .key_column = "nick_key",
    .key_type   = "text",
    .decode     = nick_decode,
    .encode     = nick_encode,
    .release    = nick_release,
    .keyof      = nick_keyof,
    .normalize  = nick_normalize,
};

static StoreType ngi_type = {
    .name       = "nickgroup",
    .table      = "nickgroups",
    .key_column = "id",
    .key_type   = "bigint",
    .children   = ngi_children,
    .decode     = ngi_decode,
    .encode     = ngi_encode,
    .release    = ngi_release,
    .keyof      = ngi_keyof,
};

/*************************************************************************/

NickInfo *add_nickinfo(NickInfo *ni)
{
    if (!store_add(&nick_type, ni))
        return NULL;
    return ni;
}

void del_nickinfo(NickInfo *ni)
{
    store_delete(&nick_type, ni);
}

/* The nick, without the expiration check. */
NickInfo *get_nickinfo_noexpire(const char *nick)
{
    return store_get(&nick_type, nick);
}

NickInfo *get_nickinfo(const char *nick)
{
    NickInfo *ni = store_get(&nick_type, nick);

    /* Checked on every lookup, as the in-memory tables used to do: an
     * expired nick is deleted rather than returned. */
    if (ni && !noexpire && check_expire_nick(ni))
        return NULL;
    return ni;
}

NickInfo *put_nickinfo(NickInfo *ni)
{
    store_put(&nick_type, ni);
    return ni;
}

NickInfo *hold_nickinfo(NickInfo *ni)
{
    store_hold(&nick_type, ni);
    return ni;
}

/*************************************************************************/

NickGroupInfo *add_nickgroupinfo(NickGroupInfo *ngi)
{
    if (!store_add(&ngi_type, ngi))
        return NULL;
    return ngi;
}

void del_nickgroupinfo(NickGroupInfo *ngi)
{
    store_delete(&ngi_type, ngi);
}

NickGroupInfo *get_nickgroupinfo(uint32 id)
{
    char key[16];

    if (!id)
        return NULL;
    snprintf(key, sizeof(key), "%u", id);
    return store_get(&ngi_type, key);
}

NickGroupInfo *put_nickgroupinfo(NickGroupInfo *ngi)
{
    if (ngi && ngi != NICKGROUPINFO_INVALID)
        store_put(&ngi_type, ngi);
    return ngi;
}

NickGroupInfo *hold_nickgroupinfo(NickGroupInfo *ngi)
{
    if (ngi && ngi != NICKGROUPINFO_INVALID)
        store_hold(&ngi_type, ngi);
    return ngi;
}

/*************************************************************************/

/* Going through many records: for the few operator commands that must.
 * `where' is SQL over the nicks (or nickgroups) table, alias t; its
 * values are $2, $3... (see store_foreach()). */

int foreach_nickinfo(const char *where, const char *const *params,
                     int nparams, int (*fn)(NickInfo *ni, void *arg),
                     void *arg)
{
    return store_foreach(&nick_type, where, params, nparams,
                         (StoreEachFn)fn, arg);
}

int foreach_nickgroupinfo(const char *where, const char *const *params,
                          int nparams,
                          int (*fn)(NickGroupInfo *ngi, void *arg),
                          void *arg)
{
    return store_foreach(&ngi_type, where, params, nparams,
                         (StoreEachFn)fn, arg);
}

long count_nickinfo(const char *where, const char *const *params, int nparams)
{
    return store_count(&nick_type, where, params, nparams);
}

long count_nickgroupinfo(const char *where, const char *const *params,
                         int nparams)
{
    return store_count(&ngi_type, where, params, nparams);
}

/* Fetch in the background the records of `nicks', and call `done' when
 * they are ready: for the paths that see the whole network go by. */
int prefetch_nickinfo(Module *owner, const char **nicks, int count,
                      void (*done)(void *arg), void *arg)
{
    return store_prefetch(owner, &nick_type, nicks, count, done, arg);
}

int prefetch_nickgroupinfo(Module *owner, const uint32 *ids, int count,
                           void (*done)(void *arg), void *arg)
{
    char (*buf)[16];
    const char **keys;
    int i, res;

    if (count <= 0)
        return store_prefetch(owner, &ngi_type, NULL, 0, done, arg);
    buf = smalloc(sizeof(*buf) * count);
    keys = smalloc(sizeof(*keys) * count);
    for (i = 0; i < count; i++) {
        snprintf(buf[i], sizeof(buf[i]), "%u", ids[i]);
        keys[i] = buf[i];
    }
    res = store_prefetch(owner, &ngi_type, keys, count, done, arg);
    free(keys);
    free(buf);
    return res;
}

/*************************************************************************/

/* Expiration.  Nothing goes through every record any more, so expired
 * nicknames (and suspensions) are looked for in the database: a batch at
 * a time, every EXPIRE_INTERVAL seconds.  Each one found is simply looked
 * up, and the lookup does the rest (check_expire_nick()). */

static Timeout *expire_timeout;

static void expire_found(const struct DbResult *res, void *user)
{
    unsigned int i;
    const char *column = user;

    if (res->err.dberr_code != DB_OK) {
        module_log("expire: cannot read the database: %s",
                   res->err.dberr_message);
        return;
    }
    for (i = 0; i < db_rows(res->data); i++) {
        const char *key = db_row_str(res->data, i, column);
        if (strcmp(column, "nick_key") == 0) {
            put_nickinfo(get_nickinfo(key));
        } else {
            NickGroupInfo *ngi = get_nickgroupinfo(strtoul(key, NULL, 10));
            if (ngi && ngi->nicks_count > 0)
                put_nickinfo(get_nickinfo(ngi_mainnick(ngi)));
            put_nickgroupinfo(ngi);
        }
    }
}

static void expire_check(Timeout *t)
{
    char cutoff[32], now[32], mask[16], limit[16];
    const char *expire_params[4], *suspend_params[2];
    struct DbParam p[4];
    struct DbParam *pl[5];
    struct DbQuery q;
    int i;

    if (noexpire || readonly)
        return;
    snprintf(now, sizeof(now), "%lld", (long long)time(NULL));
    snprintf(limit, sizeof(limit), "%d", EXPIRE_BATCH);
    if (NSExpire) {
        snprintf(cutoff, sizeof(cutoff), "%lld",
                 (long long)(time(NULL) - NSExpire));
        snprintf(mask, sizeof(mask), "%d", NS_VERBOTEN | NS_NOEXPIRE);
        expire_params[0] = cutoff;
        expire_params[1] = mask;
        expire_params[2] = limit;
        for (i = 0; i < 3; i++) {
            p[i].type = DB_TYPE_UNKNOWN;
            p[i].value = expire_params[i];
            p[i].format = DB_FORMAT_TEXT;
            pl[i] = &p[i];
        }
        pl[3] = NULL;
        q.sql = "select nick_key from nicks where last_seen < $1::bigint"
                " and (status & $2::smallint) = 0 order by last_seen"
                " limit $3::integer";
        q.params = pl;
        db_query(THIS_MODULE, &q, expire_found, "nick_key");
    }
    suspend_params[0] = now;
    suspend_params[1] = limit;
    for (i = 0; i < 2; i++) {
        p[i].type = DB_TYPE_UNKNOWN;
        p[i].value = suspend_params[i];
        p[i].format = DB_FORMAT_TEXT;
        pl[i] = &p[i];
    }
    pl[2] = NULL;
    q.sql = "select id from nickgroups where suspend_expires > 0 and"
            " suspend_expires <= $1::bigint limit $2::integer";
    q.params = pl;
    db_query(THIS_MODULE, &q, expire_found, "id");
}

/*************************************************************************/

/*************************************************************************/
/************************ Main NickServ routines *************************/
/*************************************************************************/

static void nickserv_message(struct Service *service, User *u, char *buf);

/* Run a command again once its password is ready (see encrypt.h). */

static void nickserv_replay(User *u, char *line)
{
    nickserv_message(&nickserv_service, u, line);
}

/*************************************************************************/

/* Main NickServ routine: a PRIVMSG to NickServ. */

static void nickserv_message(struct Service *service, User *u, char *buf)
{
    char *cmd;

    password_command_begin(THIS_MODULE, u, nickserv_replay, buf);
    cmd = strtok(buf, " ");

    if (cmd) {
        int i;
        ARRAY_FOREACH (i, aliases) {
            if (stricmp(cmd, aliases[i].alias) == 0) {
                cmd = aliases[i].command;
                break;
            }
        }
        if (event_emit(command_event, u, cmd) <= 0)
            run_cmd(nickserv_service.nick, u, THIS_MODULE, cmd);
    }
    password_command_end();
}

/*************************************************************************/

/*************************************************************************/

/* Handler for users connecting to the network. */

/* Validating users in the background.  A user connecting or changing nick
 * needs the nick's record (and its group's); on a network of any size these
 * come by the thousand in a burst, so they are fetched in batches by the
 * store's threads (store_prefetch()), and the user is validated when they
 * are in memory.  Until then user->ns_validate points here; a user who
 * talks to Services meanwhile is validated on the spot (validate_now()). */

typedef struct {
    User *user;         /* NULL once the user is gone or validated */
    NickInfo *ni;       /* Held between the two fetches */
    int nickchange;     /* After a nick change: set the registered mode */
    uint32 old_group;   /* Nick group of the old nick (nick change) */
} ValidateArg;

/* "nickserv.user_validated" (User *user, int nickchange,
 * uint32 old_nickgroup): the user's nick is known (user->ni, user->ngi),
 * after the connection or nick change that triggered it.  Replaces
 * "user.create" and "user.nick_change_after" for anything that needs the
 * nick's record. */
static Event* validated_event;

/* The nick group of a user's nick before a nick change: from the "before"
 * handler to the "after" one, which run back to back. */
static User *nickchange_user;
static uint32 nickchange_old_group;

/* What a nick change adds to validate_user(). */
static void validate_after_nickchange(User *user)
{
    if (usermode_reg) {
        if (user_identified(user)) {
            send_cmd(nickserv_service.nick, "SVSMODE %s :+%s", user->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
            user->mode |= usermode_reg;
        } else {
            send_cmd(nickserv_service.nick, "SVSMODE %s :-%s", user->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
            user->mode &= ~usermode_reg;
        }
    }
}

static void validated(User *user, int nickchange, uint32 old_group)
{
    validate_user(user);
    if (nickchange)
        validate_after_nickchange(user);
    event_emit(validated_event, user, nickchange, old_group);
}

static void validate_finish(ValidateArg *arg)
{
    User *user = arg->user;

    if (user) {
        user->ns_validate = NULL;
        validated(user, arg->nickchange, arg->old_group);
    }
    put_nickinfo(arg->ni);
    free(arg);
}

static void validate_ngi_ready(void *arg_)
{
    validate_finish(arg_);
}

static void validate_nick_ready(void *arg_)
{
    ValidateArg *arg = arg_;
    const char *keys[1];
    char idbuf[16];

    if (!arg->user) {
        validate_finish(arg);
        return;
    }
    /* In memory now (or known missing): no I/O.  Without the expiration
     * check, which needs the group: validate_user() does it once the group
     * is here too. */
    arg->ni = get_nickinfo_noexpire(arg->user->nick);
    if (!arg->ni || !arg->ni->nickgroup) {
        validate_finish(arg);
        return;
    }
    snprintf(idbuf, sizeof(idbuf), "%u", arg->ni->nickgroup);
    keys[0] = idbuf;
    if (!store_prefetch(THIS_MODULE, &ngi_type, keys, 1, validate_ngi_ready,
                        arg))
        validate_finish(arg);
}

static void validate_later(User *user, int nickchange)
{
    ValidateArg *arg;
    const char *keys[1];

    if (user->ns_validate)
        ((ValidateArg *)user->ns_validate)->user = NULL;
    arg = scalloc(1, sizeof(*arg));
    arg->user = user;
    arg->nickchange = nickchange;
    if (nickchange && nickchange_user == user)
        arg->old_group = nickchange_old_group;
    nickchange_user = NULL;
    user->ns_validate = arg;
    keys[0] = user->nick;
    if (!prefetch_nickinfo(THIS_MODULE, keys, 1, validate_nick_ready, arg)) {
        arg->user = NULL;
        user->ns_validate = NULL;
        validated(user, nickchange, arg->old_group);
        free(arg);
    }
}

/* The user is gone, or changing nick: forget the validation in flight. */
static void validate_cancel(User *user)
{
    if (user->ns_validate) {
        ((ValidateArg *)user->ns_validate)->user = NULL;
        user->ns_validate = NULL;
    }
}

/* The user talks to Services before their validation is done: do it now
 * (synchronously; the fetch then finds nothing to do). */
static void validate_now(User *user)
{
    ValidateArg *arg = user->ns_validate;

    if (!arg)
        return;
    arg->user = NULL;
    user->ns_validate = NULL;
    validated(user, arg->nickchange, arg->old_group);
}

/* Every message to any of the pseudoclients goes through here first. */
static int validate_before_privmsg(const char *source, const char *target,
                                   char *buf)
{
    User *u = get_user(source);

    if (u && u->ns_validate)
        validate_now(u);
    return 0;
}

static int do_user_create(User *user, int ac, char **av)
{
    validate_later(user, 0);
    return 0;
}

/*************************************************************************/

/* Callbacks for users changing nicknames (before and after). */

static int do_user_nickchange_before(User *user, const char *newnick)
{
    /* Changing nickname case isn't a real change; pop out immediately
     * in that case. */
    if (irc_stricmp(newnick, user->nick) == 0)
        return 0;

    validate_cancel(user);
    nickchange_user = user;
    nickchange_old_group = user->ngi && user->ngi != NICKGROUPINFO_INVALID
                         ? user->ngi->id : 0;
    cancel_user(user);
    return 0;
}

static int do_user_nickchange_after(User *user, const char *oldnick)
{
    /* Changing nickname case isn't a real change; pop out immediately
     * in that case. */
    if (irc_stricmp(oldnick, user->nick) == 0)
        return 0;

    user->my_signon = time(NULL);
    validate_later(user, 1);
    return 0;
}

/*************************************************************************/

/* `user' is no longer identified for group `id' (they left): take them off
 * the group's list, and let go of the pin set_identified() took, which
 * kept the group in memory while they were identified for it. */

static void release_identified(User *user, uint32 id)
{
    NickGroupInfo *ngi = get_nickgroupinfo(id);
    int j;

    if (!ngi)
        return;
    ARRAY_SEARCH_PLAIN_SCALAR(ngi->id_users, user, j);
    if (j < ngi->id_users_count) {
        ARRAY_REMOVE(ngi->id_users, j);
        put_nickgroupinfo(ngi);  /* set_identified()'s pin */
    } else {
        module_log("BUG: nickgroup %u listed in id_nicks for user %p (%s),"
                   " but user not in id_users!", ngi->id, user, user->nick);
    }
    put_nickgroupinfo(ngi);
}

/************************************/

/* Handler for users disconnecting from the network. */

static int do_user_delete(User *user, const char *reason)
{
    NickInfo *ni = user->ni;
    int i;

    validate_cancel(user);
    if (nickchange_user == user)
        nickchange_user = NULL;
    if (user_recognized(user)) {
        free(ni->last_quit);
        ni->last_quit = *reason ? sstrdup(reason) : NULL;
    }
    ARRAY_FOREACH (i, user->id_nicks)
        release_identified(user, user->id_nicks[i]);
    cancel_user(user);
    return 0;
}

/*************************************************************************/

/* Handler for REGISTER/LINK check; we disallow registration/linking of
 * the NickServ pseudoclient nickname or guest nicks.  This is done here
 * instead of in the routines themselves to avoid duplication of code at an
 * insignificant performance cost.
 */

static int do_reglink_check(const User *u, const char *nick,
                            const char *pass, const char *email)
{
    if ((protocol_features & PF_CHANGENICK) && is_guest_nick(nick)) {
        /* Don't allow guest nicks to be registered or linked.  This check
         * has to be done regardless of the state of NSForceNickChange
         * because other modules might take advantage of forced nick
         * changing. */
        return 1;
    }
    /* Nor the nick of any pseudo-client. */
    return service_find(nick) != NULL;
}

/*************************************************************************/

/* Handler for OperServ STATS ALL. */

static int do_stats_all(User *user, const char *operserv_nick)
{
    /* The records are in the database; memory holds only those in use.
     * The counts are the database's, the sizes those of the records in
     * memory. */
    notice_lang(operserv_nick, user, OPER_STATS_ALL_NICKINFO_MEM,
                (int)count_nickinfo(NULL, NULL, 0),
                (int)((store_resident(&nick_type)*sizeof(NickInfo)+512)
                      / 1024));
    notice_lang(operserv_nick, user, OPER_STATS_ALL_RESIDENT,
                (int)store_resident(&nick_type));
    notice_lang(operserv_nick, user, OPER_STATS_ALL_NICKGROUPINFO_MEM,
                (int)count_nickgroupinfo(NULL, NULL, 0),
                (int)((store_resident(&ngi_type)*sizeof(NickGroupInfo)+512)
                      / 1024));
    notice_lang(operserv_nick, user, OPER_STATS_ALL_RESIDENT,
                (int)store_resident(&ngi_type));
    return 0;
}

/*************************************************************************/
/*********************** NickServ command routines ***********************/
/*************************************************************************/

/* Return a help message. */

static void do_help(User *u)
{
    char *cmd = strtok_remaining();

    if (!cmd) {
        notice_help(nickserv_service.nick, u, NICK_HELP);
        if (NSExpire)
            notice_help(nickserv_service.nick, u, NICK_HELP_EXPIRES,
                        maketime(u->ngi,NSExpire,0));
        if (NSHelpWarning)
            notice_help(nickserv_service.nick, u, NICK_HELP_WARNING);
    } else if (event_emit(help_event, u, cmd) > 0) {
        return;
    } else if (stricmp(cmd, "COMMANDS") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS);
        if (module_find("nickserv/mail-auth"))
            notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_AUTH);
        if (module_find("nickserv/link"))
            notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_LINK);
        if (module_find("nickserv/access"))
            notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_ACCESS);
        if (module_find("nickserv/autojoin"))
            notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_AJOIN);
        notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_SET);
        if (!NSListOpersOnly)
            notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_LIST);
        notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_LISTCHANS);
        event_emit(help_cmds_event, u, 0);
        if (is_oper(u)) {
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_COMMANDS);
            if (NSEnableDropEmail)
                notice_help(nickserv_service.nick, u, NICK_OPER_HELP_COMMANDS_DROPEMAIL);
            if (EnableGetpass)
                notice_help(nickserv_service.nick, u, NICK_OPER_HELP_COMMANDS_GETPASS);
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_COMMANDS_FORBID);
            if (NSListOpersOnly)
                notice_help(nickserv_service.nick, u, NICK_HELP_COMMANDS_LIST);
            if (module_find("nickserv/mail-auth"))
                notice_help(nickserv_service.nick, u, NICK_OPER_HELP_COMMANDS_SETAUTH);
            event_emit(help_cmds_event, u, 1);
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_COMMANDS_END);
        }
    } else if (stricmp(cmd, "REGISTER") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_REGISTER,
                    getstring(u->ngi,NICK_REGISTER_SYNTAX));
        notice_help(nickserv_service.nick, u, NICK_HELP_REGISTER_EMAIL);
        notice_help(nickserv_service.nick, u, NICK_HELP_REGISTER_END);
    } else if (stricmp(cmd, "DROP") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_DROP);
        if (module_find("nickserv/link"))
            notice_help(nickserv_service.nick, u, NICK_HELP_DROP_LINK);
        notice_help(nickserv_service.nick, u, NICK_HELP_DROP_END);
    } else if ((stricmp(cmd, "DROPEMAIL") == 0
                || stricmp(cmd, "DROPEMAIL-CONFIRM") == 0)
               && NSEnableDropEmail
               && is_oper(u)
    ) {
        notice_help(nickserv_service.nick, u, NICK_OPER_HELP_DROPEMAIL,
                    maketime(u->ngi,NSDropEmailExpire,0));
    } else if (stricmp(cmd, "SET") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_SET);
        if (module_find("nickserv/link"))
            notice_help(nickserv_service.nick, u, NICK_HELP_SET_OPTION_MAINNICK);
        notice_help(nickserv_service.nick, u, NICK_HELP_SET_END);
        if (is_oper(u))
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_SET);
    } else if (strnicmp(cmd, "SET", 3) == 0
               && isspace(cmd[3])
               && stricmp(cmd+4+strspn(cmd+4," \t"), "LANGUAGE") == 0) {
        int i;
        notice_help(nickserv_service.nick, u, NICK_HELP_SET_LANGUAGE);
        for (i = 0; i < NUM_LANGS && langlist[i] >= 0; i++) {
            notice(nickserv_service.nick, u->nick, "    %2d) %s",
                   i+1, getstring_lang(langlist[i],LANG_NAME));
        }
    } else if (stricmp(cmd, "INFO") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_INFO);
        if (module_find("nickserv/mail-auth"))
            notice_help(nickserv_service.nick, u, NICK_HELP_INFO_AUTH);
        if (is_oper(u))
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_INFO);
    } else if (stricmp(cmd, "LIST") == 0) {
        if (is_oper(u)) {
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_LIST);
            notice_help(nickserv_service.nick, u, NICK_OPER_HELP_LIST_END);
        } else {
            notice_help(nickserv_service.nick, u, NICK_HELP_LIST);
        }
        if (NSListOpersOnly)
            notice_help(nickserv_service.nick, u, NICK_HELP_LIST_OPERSONLY);
    } else if (stricmp(cmd, "LISTEMAIL") == 0) {
        char buf[BUFSIZE];
        int msg = is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX;
        snprintf(buf, sizeof(buf), getstring(u->ngi,msg), "LISTEMAIL");
        notice_help(nickserv_service.nick, u, NICK_HELP_LISTEMAIL, buf);
        if (NSListOpersOnly)
            notice_help(nickserv_service.nick, u, NICK_HELP_LIST_OPERSONLY);
    } else if (stricmp(cmd, "RECOVER") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_RECOVER,
                    maketime(u->ngi,NSReleaseTimeout,MT_SECONDS));
    } else if (stricmp(cmd, "RELEASE") == 0) {
        notice_help(nickserv_service.nick, u, NICK_HELP_RELEASE,
                    maketime(u->ngi,NSReleaseTimeout,MT_SECONDS));
    } else if (stricmp(cmd, "SUSPEND") == 0 && is_oper(u)) {
        notice_help(nickserv_service.nick, u, NICK_OPER_HELP_SUSPEND, operserv_service.nick);
    } else {
        help_cmd(nickserv_service.nick, u, THIS_MODULE, cmd);
    }
}

/*************************************************************************/

/* Register a nick. */

static void do_register(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *pass = strtok(NULL, " ");
    char *email = strtok(NULL, " ");
    int n, res;
    time_t now = time(NULL);

    if (readonly) {
        notice_lang(nickserv_service.nick, u, NICK_REGISTRATION_DISABLED);
        return;
    }

    if (now < u->lastnickreg + NSRegDelay) {
        time_t left = (u->lastnickreg + NSRegDelay) - now;
        notice_lang(nickserv_service.nick, u, NICK_REG_PLEASE_WAIT,
                    maketime(u->ngi, left, MT_SECONDS));

    } else if (time(NULL) < u->my_signon + NSInitialRegDelay) {
        time_t left = (u->my_signon + NSInitialRegDelay) - now;
        notice_lang(nickserv_service.nick, u, NICK_REG_PLEASE_WAIT_FIRST,
                    maketime(u->ngi, left, MT_SECONDS));

    } else if (!pass || (NSRequireEmail && !email)
               || (stricmp(pass, u->nick) == 0
                   && (strtok(NULL, "")
                       || (email && (!strchr(email,'@')
                                     || !strchr(email,'.')))))
    ) {
        /* No password/email, or they (apparently) tried to include the nick
         * in the command. */
        syntax_error(nickserv_service.nick, u, "REGISTER", NICK_REGISTER_SYNTAX);

    } else if (!reglink_check(u, u->nick, pass, email)) {
        /* Denied by a nickserv.register_check handler. */
        notice_lang(nickserv_service.nick, u, NICK_CANNOT_BE_REGISTERED, u->nick);
        return;

    } else if (u->ni) {  /* i.e. there's already such a nick regged */
        if (u->ni->status & NS_VERBOTEN) {
            module_log("%s@%s tried to register forbidden nick %s",
                       u->username, u->host, u->nick);
            notice_lang(nickserv_service.nick, u, NICK_CANNOT_BE_REGISTERED, u->nick);
        } else {
            if (u->ngi->flags & NF_SUSPENDED)
                module_log("%s@%s tried to register suspended nick %s",
                           u->username, u->host, u->nick);
            notice_lang(nickserv_service.nick, u, NICK_X_ALREADY_REGISTERED, u->nick);
        }

    } else if (u->ngi == NICKGROUPINFO_INVALID) {
        module_log("%s@%s tried to register nick %s with missing nick group",
                   u->username, u->host, u->nick);
        notice_lang(nickserv_service.nick, u, NICK_REGISTRATION_FAILED);

    } else if (put_nickinfo(get_nickinfo(u->nick))) {
        /* Theoretically impossible if the previous tests were false, but
         * just in case */
        module_log("REGISTER %s: u->ni is NULL but nick is registered!",
                   u->nick);
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);

    } else if (stricmp(pass, u->nick) == 0
               || (StrictPasswords && strlen(pass) < 5)
    ) {
        notice_lang(nickserv_service.nick, u, MORE_OBSCURE_PASSWORD);

    } else if (email && !valid_email(email)) {
        /* Display the syntax as well in case the user just got E-mail and
         * password backwards.  Don't use syntax_error(), because that also
         * prints a "for more help" message which might just confuse the
         * user more. */
        char buf[BUFSIZE];
        snprintf(buf, sizeof(buf), getstring(u->ngi,NICK_REGISTER_SYNTAX),
                 "REGISTER");
        notice_lang(nickserv_service.nick, u, SYNTAX_ERROR, buf);
        notice_lang(nickserv_service.nick, u, BAD_EMAIL);

    } else if (email && rejected_email(email)) {
        notice_lang(nickserv_service.nick, u, REJECTED_EMAIL);
        return;

    } else if (NSRegEmailMax && email && !is_services_admin(u)
               && ((n = count_nicks_with_email(email)) < 0
                   || n >= NSRegEmailMax)) {
        if (n < 0) {
            notice_lang(nickserv_service.nick, u, NICK_REGISTER_EMAIL_UNAUTHED);
        } else {
            notice_lang(nickserv_service.nick, u, NICK_REGISTER_TOO_MANY_NICKS, n,
                        NSRegEmailMax);
        }

    } else {
        int replied = 0;
        Password passbuf;

        /* Check for E-mail addresses used in suspended nicks, if requested */
        if (NSRegDenyIfSuspended && email) {
            char flag[16];
            const char *params[2];
            snprintf(flag, sizeof(flag), "%d", NF_SUSPENDED);
            params[0] = flag;
            params[1] = email;
            if (count_nickgroupinfo("(t.flags & $2::integer) <> 0"
                                    " and lower(t.email) = lower($3)",
                                    params, 2) > 0) {
                module_log("REGISTER from %s!%s@%s denied because E-mail"
                           " address %s is used by a suspended nick",
                           u->nick, u->username, u->host, email);
                notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
                return;
            }
        }

        /* Make sure the password can be encrypted first */
        init_password(&passbuf);
        if ((res = encrypt_password(pass, strlen(pass), &passbuf)) != 0) {
            clear_password(&passbuf);
            if (res == PASSWORD_PENDING)
                return;  /* the command will be run again */
            memset(pass, 0, strlen(pass));
            module_log("Failed to encrypt password for %s (register)",
                       u->nick);
            notice_lang(nickserv_service.nick, u, NICK_REGISTRATION_FAILED);
            return;
        }
        /* Do nick setup stuff */
        ni = makenick(u->nick, &ngi);
        if (!ni) {
            clear_password(&passbuf);
            module_log("makenick(%s) failed", u->nick);
            notice_lang(nickserv_service.nick, u, NICK_REGISTRATION_FAILED);
            return;
        }
        copy_password(&ngi->pass, &passbuf);
        clear_password(&passbuf);
        ni->time_registered = ni->last_seen = time(NULL);
        ni->authstat = NA_IDENTIFIED | NA_RECOGNIZED;
        if (email)
            ngi->email = sstrdup(email);
        ngi->flags = NSDefFlags;
        ngi->memos.memomax = MEMOMAX_DEFAULT;
        ngi->channelmax = CHANMAX_DEFAULT;
        ngi->language = LANG_DEFAULT;
        ngi->timezone = TIMEZONE_DEFAULT;
        event_emit(registered_event, u, ni, ngi, &replied);
        /* If the IDENTIFIED flag is still set (a module might have
         * cleared it, e.g. mail-auth), record the ID stamp */
        if (nick_identified(ni))
            ni->id_stamp = u->servicestamp;
        /* Link back and forth to user record and store modified data */
        u->ni = ni;
        u->ngi = ngi;
        ni->user = u;
        update_userinfo(u);
        /* Tell people about it */
        if (email) {
            module_log("%s registered by %s@%s (%s)",
                       u->nick, u->username, u->host, email);
        } else {
            module_log("%s registered by %s@%s",
                       u->nick, u->username, u->host);
        }
        if (!replied)
            notice_lang(nickserv_service.nick, u, NICK_REGISTERED, u->nick);
        if (NSShowPassword)
            notice_lang(nickserv_service.nick, u, NICK_PASSWORD_IS, pass);
        /* Clear password from memory and other last-minute things */
        memset(pass, 0, strlen(pass));
        /* Note time REGISTER command was used */
        u->lastnickreg = time(NULL);
        /* Set +r (or other registered-nick mode) if IDENTIFIED is still
         * set. */
        if (nick_identified(ni) && usermode_reg) {
            send_cmd(nickserv_service.nick, "SVSMODE %s :+%s", u->nick,
                     mode_flags_to_string(usermode_reg, MODE_USER));
        }

    }

}

/*************************************************************************/

static void do_identify(User *u)
{
    char *pass = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!pass || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "IDENTIFY", NICK_IDENTIFY_SYNTAX);

    } else if (!(ni = u->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);

    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, u->nick);

    } else if (!(ngi = u->ngi) || ngi == NICKGROUPINFO_INVALID) {
        module_log("IDENTIFY: missing NickGroupInfo for %s", u->nick);
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);

    } else if (ngi->flags & NF_SUSPENDED) {
        notice_lang(nickserv_service.nick, u, NICK_X_SUSPENDED, u->nick);

    } else if (!nick_check_password(u, u->ni, pass, "IDENTIFY",
                                    NICK_IDENTIFY_FAILED)) {
        /* nothing */

    } else if (NSRequireEmail && !ngi->email) {
        ni->authstat |= NA_IDENT_NOMAIL;
        notice_lang(nickserv_service.nick, u, NICK_IDENTIFY_EMAIL_MISSING, nickserv_service.nick);

    } else if (event_emit(id_check_event, u, pass) <= 0) {
        int old_authstat = ni->authstat;
        set_identified(u);
        if (!(old_authstat & NA_IDENTIFIED)) {
            /* Only log if the user wasn't previously identified */
            module_log("%s!%s@%s identified for nick %s",
                       u->nick, u->username, u->host, u->nick);
        }
        notice_lang(nickserv_service.nick, u, NICK_IDENTIFY_SUCCEEDED);
        event_emit(identified_event, u, old_authstat);
    }
}

/*************************************************************************/

static void do_drop(User *u)
{
    char *pass = strtok(NULL, " ");
    NickInfo *ni = u->ni;
    NickGroupInfo *ngi = (u->ngi==NICKGROUPINFO_INVALID ? NULL : u->ngi);

    if (readonly && !is_services_admin(u)) {
        notice_lang(nickserv_service.nick, u, NICK_DROP_DISABLED);
        return;
    }

    if (!pass || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "DROP", NICK_DROP_SYNTAX);
        if (module_find("nickserv/link"))
            notice_lang(nickserv_service.nick, u, NICK_DROP_WARNING);
    } else if (!ni || !ngi) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);
    } else if (ngi->flags & NF_SUSPENDED) {
        notice_lang(nickserv_service.nick, u, NICK_X_SUSPENDED, u->nick);
    } else if (!nick_check_password(u, u->ni, pass, "DROP",
                                    NICK_DROP_FAILED)) {
        /* nothing */
    } else {
        if (readonly)  /* they must be a servadmin in this case */
            notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
        drop_nickgroup(ngi, u, NULL);
        notice_lang(nickserv_service.nick, u, NICK_DROPPED);
    }
}

/*************************************************************************/

/* Services admin function to drop another user's nickname.  Privileges
 * assumed to be pre-checked.
 */

static void do_dropnick(User *u)
{
    char *nick = strtok(NULL, " ");
    NickInfo *ni;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(nickserv_service.nick, u, "DROPNICK", NICK_DROPNICK_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->nickgroup && !(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
        put_nickinfo(ni);
    } else if (NSSecureAdmins && nick_is_services_admin(ni)
               && !is_services_root(u)
    ) {
        notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
        put_nickinfo(ni);
        put_nickgroupinfo(ngi);
    } else {
        if (WallAdminPrivs) {
            wallops(nickserv_service.nick, "\2%s\2 used DROPNICK on \2%s\2",
                    u->nick, ni->nick);
        }
        if (readonly)
            notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
        if (ngi) {
            drop_nickgroup(ngi, u, PTR_INVALID);
        } else {
            module_log("%s!%s@%s dropped forbidden nick %s",
                       u->nick, u->username, u->host, ni->nick);
            delnick(ni);
        }
        notice_lang(nickserv_service.nick, u, NICK_X_DROPPED, nick);
    }
}

/*************************************************************************/

/* Services admin function to drop all nicknames whose E-mail address
 * matches the given mask.  Privileges assumed to be pre-checked.
 */

/* List of recent DROPEMAILs for CONFIRM */
static struct {
    char sender[NICKMAX];       /* Who sent the command (empty = no entry) */
    char mask[BUFSIZE];         /* What mask was used */
    int count;
    time_t sent;                /* When the command was sent */
} dropemail_buffer[DROPEMAIL_BUFSIZE];

static void do_dropemail(User *u)
{
    char *mask = strtok(NULL, " ");
    int count, i, found;

    /* Parameter check */
    if (!mask || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "DROPEMAIL", NICK_DROPEMAIL_SYNTAX);
        return;
    }
    if (strlen(mask) > sizeof(dropemail_buffer[0].mask)-1) {
        notice_lang(nickserv_service.nick, u, NICK_DROPEMAIL_PATTERN_TOO_LONG,
                    sizeof(dropemail_buffer[0].mask)-1);
        return;
    }

    /* Count nicks matching this mask; exit if none found */
    if (strcmp(mask,"-") == 0)
        mask = NULL;
    if (mask) {
        char like[BUFSIZE];
        const char *params[1];
        params[0] = store_like_pattern(mask, like, sizeof(like));
        count = count_nickinfo(
            "exists (select 1 from nickgroups g where g.id = t.nickgroup"
            " and lower(g.email) like lower($2))", params, 1);
    } else {
        count = count_nickinfo(
            "t.nickgroup <> 0 and exists (select 1 from nickgroups g"
            " where g.id = t.nickgroup and g.email is null)", NULL, 0);
    }
    if (count <= 0) {
        notice_lang(nickserv_service.nick, u, NICK_DROPEMAIL_NONE);
        return;
    }
    if (mask == NULL)
        mask = "-";

    /* Clear out any previous entries for this sender/mask */
    for (i = 0; i < DROPEMAIL_BUFSIZE; i++) {
        if (irc_stricmp(u->nick, dropemail_buffer[i].sender) == 0
         && stricmp(mask, dropemail_buffer[i].mask) == 0
        ) {
            memset(&dropemail_buffer[i], 0, sizeof(dropemail_buffer[i]));
        }
    }

    /* Register command in buffer */
    found = -1;
    for (i = 0; i < DROPEMAIL_BUFSIZE; i++) {
        if (!*dropemail_buffer[i].sender) {
            found = i;
            break;
        }
    }
    if (found < 0) {
        found = 0;
        for (i = 1; i < DROPEMAIL_BUFSIZE; i++) {
            if (dropemail_buffer[i].sent < dropemail_buffer[found].sent)
                found = i;
        }
    }
    memset(&dropemail_buffer[found], 0, sizeof(dropemail_buffer[found]));
    strbcpy(dropemail_buffer[found].sender, u->nick);
    strbcpy(dropemail_buffer[found].mask, mask);
    dropemail_buffer[found].sent = time(NULL);
    dropemail_buffer[found].count = count;

    /* Send count and prompt for confirmation */
    notice_lang(nickserv_service.nick, u, NICK_DROPEMAIL_COUNT, count, nickserv_service.nick, mask);
}


/* DROPEMAIL-CONFIRM: one matching group (the SQL condition has already
 * matched; match_wild_nocase() is what the command has always meant). */
typedef struct {
    User *u;
    const char *mask;
} DropEmailArg;

static int dropemail_one(NickGroupInfo *ngi, void *arg_)
{
    DropEmailArg *arg = arg_;

    if ((arg->mask && ngi->email && match_wild_nocase(arg->mask, ngi->email))
     || (!arg->mask && !ngi->email))
        drop_nickgroup(ngi, arg->u, arg->mask ? arg->mask : "-");
    return 0;
}

static void do_dropemail_confirm(User *u)
{
    char *mask = strtok(NULL, " ");
    int i;

    /* Parameter check */
    if (!mask || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "DROPEMAIL-CONFIRM",
                     NICK_DROPEMAIL_CONFIRM_SYNTAX);
        return;
    }

    /* Make sure this is a DROPEMAIL that (1) we've seen and (2) hasn't
     * expired */
    for (i = 0; i < DROPEMAIL_BUFSIZE; i++) {
        if (irc_stricmp(u->nick, dropemail_buffer[i].sender) == 0
         && stricmp(mask, dropemail_buffer[i].mask) == 0
         && time(NULL) - dropemail_buffer[i].sent < NSDropEmailExpire
        ) {
            break;
        }
    }
    if (i >= DROPEMAIL_BUFSIZE) {
        notice_lang(nickserv_service.nick, u, NICK_DROPEMAIL_CONFIRM_UNKNOWN);
        return;
    }

    /* Okay, go ahead and delete */
    notice_lang(nickserv_service.nick, u, NICK_DROPEMAIL_CONFIRM_DROPPING,
                dropemail_buffer[i].count);
    if (readonly)
        notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
    *dropemail_buffer[i].mask = 0;  /* clear out the entry */
    if (strcmp(mask,"-") == 0)
        mask = NULL;
    {
        DropEmailArg arg;
        char like[BUFSIZE];
        const char *params[1];
        arg.u = u;
        arg.mask = mask;
        if (mask) {
            params[0] = store_like_pattern(mask, like, sizeof(like));
            foreach_nickgroupinfo("lower(t.email) like lower($2)", params, 1,
                                  dropemail_one, &arg);
        } else {
            foreach_nickgroupinfo("t.email is null", NULL, 0,
                                  dropemail_one, &arg);
        }
    }
    notice_lang(nickserv_service.nick, u, NICK_DROPEMAIL_CONFIRM_DROPPED);
    if (WallAdminPrivs) {
        wallops(nickserv_service.nick, "\2%s\2 used DROPEMAIL for \2%s\2 (%d nicks"
                " dropped)", u->nick, mask, dropemail_buffer[i].count);
    }
}

/*************************************************************************/

/* Show hidden info to nick owners and sadmins when the "ALL" parameter is
 * supplied. If a nick is online, the "Last seen address" changes to "Is
 * online from".
 * Syntax: INFO <nick> {ALL}
 * -TheShadow (13 Mar 1999)
 */

/* Check the status of show_all and make a note of having done so.  This is
 * used at the end, to see whether we should print a "use ALL for more info"
 * message.  Note that this should be the last test in a boolean expression,
 * to ensure that used_all isn't set inappropriately. */
#define CHECK_SHOW_ALL (used_all = 1, show_all)

static void do_info(User *u)
{
    char *nick = strtok(NULL, " ");
    char *param = strtok(NULL, " ");
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;

    if (!nick) {
        syntax_error(nickserv_service.nick, u, "INFO", NICK_INFO_SYNTAX);

    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);

    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);

    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);

    } else {
        char buf[BUFSIZE], *end;
        const char *commastr = getstring(u->ngi, COMMA_SPACE);
        int need_comma = 0;
        int nick_online = 0;
        const char *linked_nick_online = NULL;
        int can_show_all = 0, show_all = 0, used_all = 0;
        int i;

        /* Is the real owner of the nick we're looking up online? */
        if (ni->user && nick_id_or_rec(ni))
            nick_online = 1;

        /* Is any nick in the group in use?  Give preference to the main
         * nickname if multiple nicks are in use. */
        ARRAY_FOREACH (i, ngi->nicks) {
            NickInfo *ni2 = get_nickinfo(ngi->nicks[i]);
            if (!ni2 || ni2->nickgroup != ngi->id) {
                module_log("nick %s in nickgroup %u %s, clearing",
                           ngi->nicks[i], ngi->id,
                           !ni2 ? "does not exist" : "has wrong nickgroup ID");
                ARRAY_REMOVE(ngi->nicks, i);
                i--;
                if (ngi->nicks_count == 0) {  /* Should be impossible */
                    module_log("... nickgroup %u is now empty, dropping",
                               ngi->id);
                    notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
                    put_nickgroupinfo(ngi);
                    put_nickinfo(ni);
                    delgroup(ngi);
                    return;
                }
            } else if (ni2->user && nick_id_or_rec(ni2)) {
                if (!linked_nick_online || i == ngi->mainnick)
                    linked_nick_online = ngi->nicks[i];
            }
            put_nickinfo(ni2);
        }

        /* Only show hidden fields to owner and sadmins and only when the ALL
         * parameter is used. */
        can_show_all = ((u==ni->user && nick_online) || is_services_admin(u));

        if (can_show_all && (param && stricmp(param, "ALL") == 0))
            show_all = 1;

        notice_lang(nickserv_service.nick, u, NICK_INFO_REALNAME,
                    nick, ni->last_realname);

        /* Ignore HIDE and show the real hostmask to anyone who can use
         * INFO ALL. */
        if (nick_online) {
            if (!(ngi->flags & NF_HIDE_MASK) || can_show_all)
                notice_lang(nickserv_service.nick, u, NICK_INFO_ADDRESS_ONLINE,
                        can_show_all ? ni->last_realmask : ni->last_usermask);
            else
                notice_lang(nickserv_service.nick, u, NICK_INFO_ADDRESS_ONLINE_NOHOST,
                            ni->nick);
        } else {
            if (linked_nick_online
             && (!(ngi->flags & NF_PRIVATE) || can_show_all)
            ) {
                notice_lang(nickserv_service.nick, u, NICK_INFO_ADDRESS_OTHER_NICK,
                            linked_nick_online);
            }
            if (!(ngi->flags & NF_HIDE_MASK) || can_show_all)
                notice_lang(nickserv_service.nick, u, NICK_INFO_ADDRESS,
                        can_show_all ? ni->last_realmask : ni->last_usermask);
            strftime_lang(buf, sizeof(buf), u->ngi,
                          STRFTIME_DATE_TIME_FORMAT, ni->last_seen);
            notice_lang(nickserv_service.nick, u, NICK_INFO_LAST_SEEN, buf);
        }

        strftime_lang(buf, sizeof(buf), u->ngi, STRFTIME_DATE_TIME_FORMAT,
                      ni->time_registered);
        notice_lang(nickserv_service.nick, u, NICK_INFO_TIME_REGGED, buf);
        if (ni->last_quit && (!(ngi->flags & NF_HIDE_QUIT) || CHECK_SHOW_ALL))
            notice_lang(nickserv_service.nick, u, NICK_INFO_LAST_QUIT, ni->last_quit);
        if (ngi->url)
            notice_lang(nickserv_service.nick, u, NICK_INFO_URL, ngi->url);
        if (ngi->email && (!(ngi->flags & NF_HIDE_EMAIL) || CHECK_SHOW_ALL)) {
            if (ngi_unauthed(ngi)) {
                if (can_show_all) {
                    notice_lang(nickserv_service.nick, u, NICK_INFO_EMAIL_UNAUTHED,
                                ngi->email);
                }
            } else {
                notice_lang(nickserv_service.nick, u, NICK_INFO_EMAIL, ngi->email);
            }
        }
        if (ngi->info)
            notice_lang(nickserv_service.nick, u, NICK_INFO_INFO, ngi->info);
        *buf = 0;
        end = buf;
        if (ngi->flags & NF_KILLPROTECT) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s",
                            getstring(u->ngi, NICK_INFO_OPT_KILL));
            need_comma = 1;
        }
        if (ngi->flags & NF_SECURE) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->ngi, NICK_INFO_OPT_SECURE));
            need_comma = 1;
        }
        if (ngi->flags & NF_PRIVATE) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->ngi, NICK_INFO_OPT_PRIVATE));
            need_comma = 1;
        }
        if (ngi->flags & NF_NOOP) {
            end += snprintf(end, sizeof(buf)-(end-buf), "%s%s",
                            need_comma ? commastr : "",
                            getstring(u->ngi, NICK_INFO_OPT_NOOP));
            need_comma = 1;
        }
        notice_lang(nickserv_service.nick, u, NICK_INFO_OPTIONS,
                    *buf ? buf : getstring(u->ngi, NICK_INFO_OPT_NONE));

        if ((ni->status & NS_NOEXPIRE) && CHECK_SHOW_ALL)
            notice_lang(nickserv_service.nick, u, NICK_INFO_NO_EXPIRE);

        if (ngi->flags & NF_SUSPENDED) {
            notice_lang(nickserv_service.nick, u, NICK_X_SUSPENDED, nick);
            if (CHECK_SHOW_ALL) {
                char timebuf[BUFSIZE], expirebuf[BUFSIZE];
                strftime_lang(timebuf, sizeof(timebuf), u->ngi,
                              STRFTIME_DATE_TIME_FORMAT, ngi->suspend_time);
                expires_in_lang(expirebuf, sizeof(expirebuf), u->ngi,
                                ngi->suspend_expires);
                notice_lang(nickserv_service.nick, u, NICK_INFO_SUSPEND_DETAILS,
                            ngi->suspend_who, timebuf, expirebuf);
                notice_lang(nickserv_service.nick, u, NICK_INFO_SUSPEND_REASON,
                            ngi->suspend_reason);
            }
        }

        if (can_show_all && !show_all && used_all)
            notice_lang(nickserv_service.nick, u, NICK_INFO_SHOW_ALL, nickserv_service.nick,
                        ni->nick);
    }

    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_listchans(User *u)
{
    NickInfo *ni = u->ni;
    NickGroupInfo *ngi = NULL;

    if (ni)
        hold_nickinfo(ni);
    if (is_oper(u)) {
        char *nick = strtok(NULL, " ");
        if (nick) {
            NickInfo *ni2 = get_nickinfo(nick);
            if (!ni2) {
                notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
                return;
            } else if (ni2 == ni) {
                /* Let the command through even for non-servadmins if they
                 * gave their own nick; it's less confusing than a
                 * "Permission denied" error */
            } else if (!is_services_admin(u)) {
                notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
                put_nickinfo(ni2);
                return;
            } else {
                put_nickinfo(ni);
                ni = ni2;
            }
        }
    } else if (strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "LISTCHANS", NICK_LISTCHANS_SYNTAX);
        return;
    }
    if (!ni) {
        notice_lang(nickserv_service.nick, u, NICK_NOT_REGISTERED);
        return;
    }
    if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, ni->nick);
    } else if (!user_identified(u)) {
        notice_lang(nickserv_service.nick, u, NICK_IDENTIFY_REQUIRED, nickserv_service.nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (refresh_owned_channels(ngi), !ngi->channels_count) {
        notice_lang(nickserv_service.nick, u, NICK_LISTCHANS_NONE, ni->nick);
    } else {
        int i;
        notice_lang(nickserv_service.nick, u, NICK_LISTCHANS_HEADER, ni->nick);
        ARRAY_FOREACH (i, ngi->channels)
            notice(nickserv_service.nick, u->nick, "    %s", ngi->channels[i]);
        notice_lang(nickserv_service.nick, u, NICK_LISTCHANS_END, ngi->channels_count);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

/* LIST and LISTEMAIL.  The nicks are in the database, so the pattern is
 * given to it (as a LIKE pattern, over the IRC-lowercased nick, the user
 * masks or the address), and only the candidates it returns are looked at
 * here -- and only until a page of results has been shown.  The total is
 * then the database's count of candidates. */

typedef struct {
    User *u;
    const char *pattern;
    int email;           /* LISTEMAIL */
    int mask_has_at;
    int is_servadmin;
    int16 match_NS;
    int32 match_NF;
    int match_auth;
    int have_auth_module;
    int skip;
    int nnicks;
    const char *nonestr;
} ListArg;

static int list_one(NickInfo *ni, void *arg_)
{
    ListArg *a = arg_;
    User *u = a->u;
    NickGroupInfo *ngi = get_nickgroupinfo(ni->nickgroup);
    char buf[BUFSIZE];
    int can_see = 0;
    const char *mask = NULL;
    int matched;

    if (!a->is_servadmin && ((ngi && (ngi->flags & NF_PRIVATE))
                             || (ni->status & NS_VERBOTEN))) {
        put_nickgroupinfo(ngi);
        return 0;
    }
    if (a->match_NS || a->match_NF || a->match_auth) {
        /* We have flags, now see if they match */
        if (!((ni->status & a->match_NS)
           || (ngi && (ngi->flags & a->match_NF))
           || (ngi && ngi_unauthed(ngi) && a->match_auth)
        )) {
            put_nickgroupinfo(ngi);
            return 0;
        }
    }
    if (!a->email) {
        if (u == ni->user || a->is_servadmin)
            mask = ni->last_realmask;
        else
            mask = ni->last_usermask;
        if (!a->is_servadmin && ngi && (ngi->flags & NF_HIDE_MASK)) {
            snprintf(buf, sizeof(buf), "%-20s  [Hidden]", ni->nick);
        } else if (ni->status & NS_VERBOTEN) {
            snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", ni->nick);
        } else {
            can_see = 1;
            snprintf(buf, sizeof(buf), "%-20s  %s", ni->nick,
                     mask ? mask : "[Never online]");
        }
        matched = (!a->mask_has_at && match_wild_nocase(a->pattern, ni->nick))
               || (a->mask_has_at && can_see && mask
                   && match_wild_nocase(a->pattern, mask));
    } else {
        if (!a->is_servadmin && ngi && (ngi->flags & NF_HIDE_EMAIL)
         && (!valid_ngi(u) || ngi->id!=u->ngi->id || !user_identified(u))){
            snprintf(buf, sizeof(buf), "%-20s  [Hidden]", ni->nick);
        } else if (ni->status & NS_VERBOTEN) {
            snprintf(buf, sizeof(buf), "%-20s  [Forbidden]", ni->nick);
        } else {
            can_see = 1;
            snprintf(buf, sizeof(buf), "%-20s  %s", ni->nick,
                     ngi && ngi->email ? ngi->email : a->nonestr);
        }
        matched = (!a->mask_has_at && match_wild_nocase(a->pattern, ni->nick))
               || (a->mask_has_at && can_see && ngi && ngi->email
                   && match_wild_nocase(a->pattern, ngi->email));
    }
    if (matched) {
        a->nnicks++;
        if (a->nnicks > a->skip && a->nnicks <= a->skip+ListMax) {
            char suspended_char = ' ';
            char noexpire_char = ' ';
            const char *auth_char = a->have_auth_module ? " " : "";
            if (a->is_servadmin) {
                if (ngi && (ngi->flags & NF_SUSPENDED))
                    suspended_char = '*';
                if (ni->status & NS_NOEXPIRE)
                    noexpire_char = '!';
                if (a->have_auth_module && ngi && ngi_unauthed(ngi))
                    auth_char = "?";
            }
            if (a->nnicks == 1)  /* display header before first result */
                notice_lang(nickserv_service.nick, u, NICK_LIST_HEADER, a->pattern);
            notice(nickserv_service.nick, u->nick, "   %c%c%s %s",
                   suspended_char, noexpire_char, auth_char, buf);
        }
    }
    put_nickgroupinfo(ngi);
    /* A page is enough: the total comes from the database. */
    return a->nnicks >= a->skip + ListMax;
}

static void do_list_common(User *u, const char *cmdname, int email)
{
    char *pattern = strtok(NULL, " ");
    char *keyword;
    ListArg a;
    char like[BUFSIZE], lowered[BUFSIZE];
    const char *params[1];
    const char *where;
    int seen;

    memset(&a, 0, sizeof(a));
    a.u = u;
    a.email = email;
    a.is_servadmin = is_services_admin(u);

    if (NSListOpersOnly && !is_oper(u)) {
        notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
        return;
    }

    a.have_auth_module = (module_find("nickserv/mail-auth") != NULL);

    if (pattern && *pattern == '+') {
        a.skip = (int)atolsafe(pattern+1, 0, INT_MAX);
        if (a.skip < 0) {
            syntax_error(nickserv_service.nick, u, cmdname,
                         is_oper(u)? NICK_LIST_OPER_SYNTAX: NICK_LIST_SYNTAX);
            return;
        }
        pattern = strtok(NULL, " ");
    }

    if (!pattern) {
        syntax_error(nickserv_service.nick, u, cmdname,
                     is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX);
        return;
    }
    a.pattern = pattern;
    a.mask_has_at = (strchr(pattern,'@') != 0);
    a.nonestr = getstring(u->ngi, NICK_LISTEMAIL_NONE);

    while (a.is_servadmin && (keyword = strtok(NULL, " "))) {
        if (stricmp(keyword, "FORBIDDEN") == 0) {
            a.match_NS |= NS_VERBOTEN;
        } else if (stricmp(keyword, "NOEXPIRE") == 0) {
            a.match_NS |= NS_NOEXPIRE;
        } else if (stricmp(keyword, "SUSPENDED") == 0) {
            a.match_NF |= NF_SUSPENDED;
        } else if (stricmp(keyword, "NOAUTH") == 0 && a.have_auth_module) {
            a.match_auth = 1;
        } else {
            syntax_error(nickserv_service.nick, u, cmdname,
                 is_oper(u) ? NICK_LIST_OPER_SYNTAX : NICK_LIST_SYNTAX);
        }
    }

    if (!a.mask_has_at) {
        nick_normalize(pattern, lowered, sizeof(lowered));
        params[0] = store_like_pattern(lowered, like, sizeof(like));
        where = "t.nick_key like $2";
    } else {
        params[0] = store_like_pattern(pattern, like, sizeof(like));
        where = email
            ? "exists (select 1 from nickgroups g where g.id = t.nickgroup"
              " and lower(g.email) like lower($2))"
            : "(lower(t.last_usermask) like lower($2)"
              " or lower(t.last_realmask) like lower($2))";
    }
    seen = foreach_nickinfo(where, params, 1, list_one, &a);
    if (seen < 0) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (a.nnicks) {
        int count = a.nnicks - a.skip;
        long total = a.nnicks;
        if (count < 0)
            count = 0;
        else if (count > ListMax)
            count = ListMax;
        /* Stopped at a page: the rest is only counted. */
        if (a.nnicks >= a.skip + ListMax) {
            long candidates = count_nickinfo(where, params, 1);
            if (candidates > total)
                total = candidates;
        }
        notice_lang(nickserv_service.nick, u, LIST_RESULTS, count, (int)total);
    } else {
        notice_lang(nickserv_service.nick, u, NICK_LIST_NO_MATCH);
    }
}

static void do_list(User *u)
{
    do_list_common(u, "LIST", 0);
}

/*************************************************************************/

static void do_listemail(User *u)
{
    do_list_common(u, "LISTEMAIL", 1);
}

/*************************************************************************/

static void do_recover(User *u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickInfo *ni;
    User *u2;

    if (!nick || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "RECOVER", NICK_RECOVER_SYNTAX);
    } else if (!(u2 = get_user(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(ni = u2->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_GUESTED) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_IN_USE, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (irc_stricmp(nick, u->nick) == 0) {
        notice_lang(nickserv_service.nick, u, NICK_NO_RECOVER_SELF);
    } else {
        if (pass) {
            if (!nick_check_password(u, ni, pass, "RECOVER", ACCESS_DENIED))
                return;
        } else if (!has_identified_nick(u, ni->nickgroup)) {
            notice_lang(nickserv_service.nick, u, ACCESS_DENIED);
            return;
        }
        collide_nick(ni, 0);
        notice_lang(nickserv_service.nick, u, NICK_RECOVERED, nickserv_service.nick, nick);
    }
}

/*************************************************************************/

static void do_release(User *u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickInfo *ni = NULL;

    if (!nick || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "RELEASE", NICK_RELEASE_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ni->status & NS_KILL_HELD)) {
        notice_lang(nickserv_service.nick, u, NICK_RELEASE_NOT_HELD, nick);
    } else {
        if (pass) {
            if (!nick_check_password(u, ni, pass, "RELEASE", ACCESS_DENIED))
                return;
        } else if (!has_identified_nick(u, ni->nickgroup)) {
            notice_lang(nickserv_service.nick, u, ACCESS_DENIED);
            return;
        }
        release_nick(ni, 0);
        notice_lang(nickserv_service.nick, u, NICK_RELEASED);
    }
    put_nickinfo(ni);
}

/*************************************************************************/

static void do_ghost(User *u)
{
    char *nick = strtok(NULL, " ");
    char *pass = strtok(NULL, " ");
    NickInfo *ni;
    User *u2;

    if (!nick || strtok_remaining()) {
        syntax_error(nickserv_service.nick, u, "GHOST", NICK_GHOST_SYNTAX);
    } else if (!(u2 = get_user(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_IN_USE, nick);
    } else if (!(ni = u2->ni)) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_GUESTED) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_IN_USE, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (irc_stricmp(nick, u->nick) == 0) {
        notice_lang(nickserv_service.nick, u, NICK_NO_GHOST_SELF);
    } else {
        char buf[NICKMAX+32];
        if (pass) {
            if (!nick_check_password(u, ni, pass, "GHOST", ACCESS_DENIED))
                return;
        } else if (!has_identified_nick(u, ni->nickgroup)) {
            notice_lang(nickserv_service.nick, u, ACCESS_DENIED);
            return;
        }
        snprintf(buf, sizeof(buf), "GHOST command used by %s", u->nick);
        kill_user(nickserv_service.nick, nick, buf);
        notice_lang(nickserv_service.nick, u, NICK_GHOST_KILLED);
    }
}

/*************************************************************************/

static void do_status(User *u)
{
    char *nick;
    User *u2;
    int i = 0;

    while ((nick = strtok(NULL, " ")) && (i++ < 16)) {
        if (!(u2 = get_user(nick)) || !u2->ni)
            notice(nickserv_service.nick, u->nick, "STATUS %s 0", nick);
        else if (user_identified(u2))
            notice(nickserv_service.nick, u->nick, "STATUS %s 3", nick);
        else if (user_recognized(u2))
            notice(nickserv_service.nick, u->nick, "STATUS %s 2", nick);
        else
            notice(nickserv_service.nick, u->nick, "STATUS %s 1", nick);
    }
}

/*************************************************************************/

static void do_getpass(User *u)
{
    char *nick = strtok(NULL, " ");
    char pass[PASSMAX];
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    int i;

    /* Assumes that permission checking has already been done. */
    if (!nick) {
        syntax_error(nickserv_service.nick, u, "GETPASS", NICK_GETPASS_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (NSSecureAdmins && nick_is_services_admin(ni)
               && !is_services_root(u)) {
        notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
    } else if ((i = decrypt_password(&ngi->pass, pass, PASSMAX)) == -2) {
        notice_lang(nickserv_service.nick, u, NICK_GETPASS_UNAVAILABLE, nick);
    } else if (i != 0) {
        module_log("decrypt_password() failed for GETPASS on %s", nick);
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else {
        module_log("%s!%s@%s used GETPASS on %s",
                   u->nick, u->username, u->host, ni->nick);
        if (WallAdminPrivs) {
            wallops(nickserv_service.nick, "\2%s\2 used GETPASS on \2%s\2",
                    u->nick, ni->nick);
        }
        notice_lang(nickserv_service.nick, u, NICK_GETPASS_PASSWORD_IS, nick, pass);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_forbid(User *u)
{
    NickInfo *ni;
    char *nick = strtok(NULL, " ");
    User *u2;

    /* Assumes that permission checking has already been done. */
    if (!nick) {
        syntax_error(nickserv_service.nick, u, "FORBID", NICK_FORBID_SYNTAX);
        return;
    }
    u2 = get_user(nick);
    if ((ni = get_nickinfo(nick)) != NULL) {
        if (NSSecureAdmins && nick_is_services_admin(ni)
            && !is_services_root(u)
        ) {
            notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
            return;
        }
        if (u2) {
            put_nickinfo(u2->ni);
            put_nickgroupinfo(u2->ngi);
            u2->ni = NULL;
            u2->ngi = NULL;
        }
        delnick(ni);
    }

    if (readonly)
        notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
    ni = makenick(nick, NULL);
    if (ni) {
        ni->status |= NS_VERBOTEN;
        ni->time_registered = time(NULL);
        put_nickinfo(ni);
        module_log("%s!%s@%s set FORBID for nick %s",
                   u->nick, u->username, u->host, nick);
        notice_lang(nickserv_service.nick, u, NICK_FORBID_SUCCEEDED, nick);
        if (WallAdminPrivs)
            wallops(nickserv_service.nick, "\2%s\2 used FORBID on \2%s\2", u->nick, nick);
        /* If someone is using the nick, make them stop */
        if (u2)
            validate_user(u2);
    } else {
        module_log("Valid FORBID for %s by %s!%s@%s failed",
                   nick, u->nick, u->username, u->host);
        notice_lang(nickserv_service.nick, u, NICK_FORBID_FAILED, nick);
    }
}

/*************************************************************************/

static void do_suspend(User *u)
{
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    char *expiry, *nick, *reason;
    time_t expires;

    nick = strtok(NULL, " ");
    if (nick && *nick == '+') {
        expiry = nick+1;
        nick = strtok(NULL, " ");
    } else {
        expiry = NULL;
    }
    reason = strtok_remaining();

    if (!nick || !reason) {
        syntax_error(nickserv_service.nick, u, "SUSPEND", NICK_SUSPEND_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (ngi->flags & NF_SUSPENDED) {
        notice_lang(nickserv_service.nick, u, NICK_SUSPEND_ALREADY_SUSPENDED, nick);
    } else if (NSSecureAdmins && nick_is_services_admin(ni)
               && !is_services_root(u)
    ) {
        notice_lang(nickserv_service.nick, u, PERMISSION_DENIED);
    } else {
        if (expiry)
            expires = dotime(expiry);
        else
            expires = NSSuspendExpire;
        if (expires < 0) {
            notice_lang(nickserv_service.nick, u, BAD_EXPIRY_TIME);
            return;
        } else if (expires > 0) {
            expires += time(NULL);      /* Set an absolute time */
        }
        module_log("%s!%s@%s suspended %s",
                   u->nick, u->username, u->host, ni->nick);
        suspend_nick(ngi, reason, u->nick, expires);
        notice_lang(nickserv_service.nick, u, NICK_SUSPEND_SUCCEEDED, nick);
        if (readonly)
            notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
        if (WallAdminPrivs) {
            wallops(nickserv_service.nick, "\2%s\2 used SUSPEND on \2%s\2",
                    u->nick, ni->nick);
        }
        /* If someone is using the nick, make them stop */
        if (ni->user)
            validate_user(ni->user);
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

static void do_unsuspend(User *u)
{
    NickInfo *ni = NULL;
    NickGroupInfo *ngi = NULL;
    char *nick = strtok(NULL, " ");

    if (!nick) {
        syntax_error(nickserv_service.nick, u, "UNSUSPEND", NICK_UNSUSPEND_SYNTAX);
    } else if (!(ni = get_nickinfo(nick))) {
        notice_lang(nickserv_service.nick, u, NICK_X_NOT_REGISTERED, nick);
    } else if (ni->status & NS_VERBOTEN) {
        notice_lang(nickserv_service.nick, u, NICK_X_FORBIDDEN, nick);
    } else if (!(ngi = get_ngi(ni))) {
        notice_lang(nickserv_service.nick, u, INTERNAL_ERROR);
    } else if (!(ngi->flags & NF_SUSPENDED)) {
        notice_lang(nickserv_service.nick, u, NICK_UNSUSPEND_NOT_SUSPENDED, nick);
    } else {
        module_log("%s!%s@%s unsuspended %s",
                   u->nick, u->username, u->host, ni->nick);
        unsuspend_nick(ngi, 1);
        notice_lang(nickserv_service.nick, u, NICK_UNSUSPEND_SUCCEEDED, nick);
        if (readonly)
            notice_lang(nickserv_service.nick, u, READ_ONLY_MODE);
        if (WallAdminPrivs) {
            wallops(nickserv_service.nick, "\2%s\2 used UNSUSPEND on \2%s\2",
                    u->nick, ni->nick);
        }
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

/*************************************************************************/

#ifdef DEBUG_COMMANDS

/* Return all the fields in the NickInfo structure. */

static void do_listnick(User *u)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *nick = strtok(NULL, " ");
    char buf1[BUFSIZE], buf2[BUFSIZE];
    char *s;
    int i;

    if (!nick)
        return;
    ni = get_nickinfo(nick);
    if (!ni) {
        notice(nickserv_service.nick, u->nick, "%s", nick);
        notice(nickserv_service.nick, u->nick, ":");
        return;
    }
    ngi = get_nickgroupinfo(ni->nickgroup);
    notice(nickserv_service.nick, u->nick, "%s group:%u usermask:%s realmask:%s"
           " reg:%d seen:%d stat:%04X auth:%04X idstamp:%d badpass:%d :%s;%s",
           ni->nick, (int)ni->nickgroup, ni->last_usermask, ni->last_realmask,
           (int)ni->time_registered, (int)ni->last_seen, ni->status & 0xFFFF,
           ni->authstat & 0xFFFF, ni->id_stamp, ni->bad_passwords,
           ni->last_realname, (ni->last_quit ? ni->last_quit : "-"));
    if (ngi) {
        if (ngi->authcode) {
            snprintf(buf1, sizeof(buf1), "%d.%d",
                     (int)ngi->authcode, (int)ngi->authset);
        } else {
            *buf1 = 0;
        }
        if (ngi->flags & NF_SUSPENDED) {
            snprintf(buf2, sizeof(buf2), "%s.%d.%d.%s",
                     ngi->suspend_who, (int)ngi->suspend_time,
                     (int)ngi->suspend_expires,
                     ngi->suspend_reason ? ngi->suspend_reason : "-");
            strnrepl(buf2, sizeof(buf2), " ", "_");
        } else {
            *buf2 = 0;
        }
        notice(nickserv_service.nick, u->nick, "+ flags:%08X ospriv:%04X authcode:%s"
               " susp:%s chancnt:%d chanmax:%d lang:%d tz:%d acccnt:%d"
               " ajoincnt:%d memocnt:%d memomax:%d igncnt:%d",
               ngi->flags, ngi->os_priv, buf1, buf2, ngi->channels_count,
               ngi->channelmax, ngi->language, ngi->timezone,
               ngi->access_count, ngi->ajoin_count, ngi->memos.memos_count,
               ngi->memos.memomax, ngi->ignore_count);
        notice(nickserv_service.nick, u->nick, "+ url:%s", ngi->url ? ngi->url : "");
        notice(nickserv_service.nick, u->nick, "+ email:%s", ngi->email?ngi->email:"");
        notice(nickserv_service.nick, u->nick, "+ info:%s", ngi->info ? ngi->info : "");
        s = buf1;
        *buf1 = 0;
        ARRAY_FOREACH (i, ngi->access)
            s += snprintf(s, sizeof(buf1)-(s-buf1), "%s%s",
                          *buf1 ? "," : "", ngi->access[i]);
        strnrepl(buf1, sizeof(buf1), " ", "_");
        notice(nickserv_service.nick, u->nick, "+ acc:%s", buf1);
        s = buf1;
        *buf1 = 0;
        ARRAY_FOREACH (i, ngi->ajoin)
            s += snprintf(s, sizeof(buf1)-(s-buf1), "%s%s",
                          *buf1 ? "," : "", ngi->ajoin[i]);
        strnrepl(buf1, sizeof(buf1), " ", "_");
        notice(nickserv_service.nick, u->nick, "+ ajoin:%s", buf1);
        s = buf1;
        *buf1 = 0;
        ARRAY_FOREACH (i, ngi->ignore)
            s += snprintf(s, sizeof(buf1)-(s-buf1), "%s%s",
                          *buf1 ? "," : "", ngi->ignore[i]);
        strnrepl(buf1, sizeof(buf1), " ", "_");
        notice(nickserv_service.nick, u->nick, "+ ign:%s", buf1);
    } else {
        notice(nickserv_service.nick, u->nick, ":");
    }
    put_nickinfo(ni);
    put_nickgroupinfo(ngi);
}

#endif /* DEBUG_COMMANDS */

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int NSDefKill;
static int NSDefKillQuick;
static int NSDefSecure;
static int NSDefPrivate;
static int NSDefNoOp;
static int NSDefHideEmail;
static int NSDefHideUsermask;
static int NSDefHideQuit;
static int NSDefMemoSignon;
static int NSDefMemoReceive;
static int NSEnableRegister;
static char *temp_nsuserhost;

static int do_NSAlias(const char *filename, int linenum, char *param);

static ConfigDirective nickserv_config[] = {
    { "NSAlias",          { { CD_FUNC, 0, do_NSAlias } } },
    { "NSAllowKillImmed", { { CD_SET, 0, &NSAllowKillImmed } } },
    { "NSDefHideEmail",   { { CD_SET, 0, &NSDefHideEmail } } },
    { "NSDefHideQuit",    { { CD_SET, 0, &NSDefHideQuit } } },
    { "NSDefHideUsermask",{ { CD_SET, 0, &NSDefHideUsermask } } },
    { "NSDefKill",        { { CD_SET, 0, &NSDefKill } } },
    { "NSDefKillQuick",   { { CD_SET, 0, &NSDefKillQuick } } },
    { "NSDefMemoReceive", { { CD_SET, 0, &NSDefMemoReceive } } },
    { "NSDefMemoSignon",  { { CD_SET, 0, &NSDefMemoSignon } } },
    { "NSDefNoOp",        { { CD_SET, 0, &NSDefNoOp } } },
    { "NSDefPrivate",     { { CD_SET, 0, &NSDefPrivate } } },
    { "NSDefSecure",      { { CD_SET, 0, &NSDefSecure } } },
    { "NSDropEmailExpire",{ { CD_TIME, CF_DIRREQ, &NSDropEmailExpire } } },
    { "NSEnableDropEmail",{ { CD_SET, 0, &NSEnableDropEmail } } },
    { "NSEnableRegister", { { CD_SET, 0, &NSEnableRegister } } },
    { "NSEnforcerUser",   { { CD_STRING, CF_DIRREQ, &temp_nsuserhost } } },
    { "NSExpire",         { { CD_TIME, 0, &NSExpire } } },
    { "NSExpireWarning",  { { CD_TIME, 0, &NSExpireWarning } } },
    { "NSForceNickChange",{ { CD_SET, 0, &NSForceNickChange } } },
    { "NSHelpWarning",    { { CD_SET, 0, &NSHelpWarning } } },
    { "NSInitialRegDelay",{ { CD_TIME, 0, &NSInitialRegDelay } } },
    { "NSListOpersOnly",  { { CD_SET, 0, &NSListOpersOnly } } },
    { "NSRegDelay",       { { CD_TIME, 0, &NSRegDelay } } },
    { "NSRegDenyIfSuspended",{{CD_SET, 0, &NSRegDenyIfSuspended } } },
    { "NSRegEmailMax",    { { CD_POSINT, 0, &NSRegEmailMax } } },
    { "NSRequireEmail",   { { CD_SET, 0, &NSRequireEmail } } },
    { "NSReleaseTimeout", { { CD_TIME, CF_DIRREQ, &NSReleaseTimeout } } },
    { "NSSecureAdmins",   { { CD_SET, 0, &NSSecureAdmins } } },
    { "NSSetEmailDelay",  { { CD_TIME, 0, &NSSetEmailDelay } } },
    { "NSShowPassword",   { { CD_SET, 0, &NSShowPassword } } },
    { "NSSuspendExpire",  { { CD_TIME, 0 , &NSSuspendExpire },
                            { CD_TIME, 0 , &NSSuspendGrace } } },
    { NULL }
};

/* Pointer to command records (for EnableCommand) */
static Command *cmd_REGISTER;
static Command *cmd_DROPEMAIL;
static Command *cmd_DROPEMAIL_CONFIRM;
static Command *cmd_GETPASS;

/* Old message numbers */
static int old_REGISTER_SYNTAX          = -1;
static int old_HELP_REGISTER_EMAIL      = -1;
static int old_HELP_UNSET               = -1;
static int old_DISCONNECT_IN_1_MINUTE   = -1;
static int old_DISCONNECT_IN_20_SECONDS = -1;

/*************************************************************************/

/* NSAlias handling (array maintenance) */

static int do_NSAlias(const char *filename, int linenum, char *param)
{
    static Alias *new_aliases = NULL;
    static int new_aliases_count = 0;
    int i;
    char *s;

    if (!filename) {
        switch (linenum) {
          case CDFUNC_INIT:
            /* Prepare for reading config file: clear out "new" array */
            ARRAY_FOREACH (i, new_aliases) {
                free(new_aliases[i].alias);
                free(new_aliases[i].command);
            }
            free(new_aliases);
            new_aliases = NULL;
            new_aliases_count = 0;
            break;
          case CDFUNC_SET:
            /* Copy data to config variables */
            ARRAY_FOREACH (i, aliases) {
                free(aliases[i].alias);
                free(aliases[i].command);
            }
            free(aliases);
            aliases = new_aliases;
            aliases_count = new_aliases_count;
            new_aliases = NULL;
            new_aliases_count = 0;
            break;
          case CDFUNC_DECONFIG:
            /* Clear out config variables */
            ARRAY_FOREACH (i, aliases) {
                free(aliases[i].alias);
                free(aliases[i].command);
            }
            free(aliases);
            aliases = NULL;
            aliases_count = 0;
            break;
        }
        return 1;
    } /* if (!filename) */

    s = strchr(param, '=');
    if (!s) {
        config_error(filename, linenum, "Missing = in NSAlias parameter");
        return 0;
    }
    *s++ = 0;
    ARRAY_EXTEND(new_aliases);
    new_aliases[new_aliases_count-1].alias = sstrdup(param);
    new_aliases[new_aliases_count-1].command = sstrdup(s);
    return 1;
}

/*************************************************************************/

static void handle_config(void)
{
    char *s;

    if (temp_nsuserhost) {
        NSEnforcerUser = temp_nsuserhost;
        if (!(s = strchr(temp_nsuserhost, '@'))) {
            NSEnforcerHost = ServiceHost;
        } else {
            *s++ = 0;
            NSEnforcerHost = s;
        }
    }

    NSDefFlags = 0;
    if (NSDefKill)
        NSDefFlags |= NF_KILLPROTECT;
    if (NSDefKillQuick)
        NSDefFlags |= NF_KILL_QUICK;
    if (NSDefSecure)
        NSDefFlags |= NF_SECURE;
    if (NSDefPrivate)
        NSDefFlags |= NF_PRIVATE;
    if (NSDefNoOp)
        NSDefFlags |= NF_NOOP;
    if (NSDefHideEmail)
        NSDefFlags |= NF_HIDE_EMAIL;
    if (NSDefHideUsermask)
        NSDefFlags |= NF_HIDE_MASK;
    if (NSDefHideQuit)
        NSDefFlags |= NF_HIDE_QUIT;
    if (NSDefMemoSignon)
        NSDefFlags |= NF_MEMO_SIGNON;
    if (NSDefMemoReceive)
        NSDefFlags |= NF_MEMO_RECEIVE;

    if (NSForceNickChange && !(protocol_features & PF_CHANGENICK)) {
        module_log("warning: forced nick changing not supported by IRC"
                   " server, disabling NSForceNickChange");
        NSForceNickChange = 0;
    }
}

/*************************************************************************/

/* -clear-nick-email: one group. */
static int clear_email_one(NickGroupInfo *ngi, void *arg)
{
    free(ngi->email);
    ngi->email = NULL;
    return 0;
}

static int do_command_line(const char *option, const char *value)
{
    if (!option || strcmp(option, "clear-nick-email") != 0)
        return 0;
    if (value) {
        fprintf(stderr, "-clear-nick-email takes no options\n");
        return 2;
    }
    module_log("Clearing all E-mail addresses (-clear-nick-email specified"
               " on command line)");
    foreach_nickgroupinfo("t.email is not null", NULL, 0, clear_email_one,
                          NULL);
    return 1;
}

/*************************************************************************/

static void nickserv_rehash(Module *module)
{
    handle_config();
    if (NSEnableRegister)
        cmd_REGISTER->name = "REGISTER";
    else
        cmd_REGISTER->name = "";
    if (NSEnableDropEmail) {
        cmd_DROPEMAIL->name = "DROPEMAIL";
        cmd_DROPEMAIL_CONFIRM->name = "DROPEMAIL-CONFIRM";
    } else {
        cmd_DROPEMAIL->name = "";
        cmd_DROPEMAIL_CONFIRM->name = "";
    }
    if (EnableGetpass)
        cmd_GETPASS->name = "GETPASS";
    else
        cmd_GETPASS->name = "";
    if (NSRequireEmail) {
        mapstring(NICK_REGISTER_SYNTAX, NICK_REGISTER_REQ_EMAIL_SYNTAX);
        mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL_REQ);
        mapstring(NICK_HELP_UNSET, NICK_HELP_UNSET_REQ_EMAIL);
    } else {
        mapstring(NICK_REGISTER_SYNTAX, old_REGISTER_SYNTAX);
        mapstring(NICK_HELP_REGISTER_EMAIL, old_HELP_REGISTER_EMAIL);
        mapstring(NICK_HELP_UNSET, old_HELP_UNSET);
    }
    if (NSForceNickChange) {
        mapstring(DISCONNECT_IN_1_MINUTE, FORCENICKCHANGE_IN_1_MINUTE);
        mapstring(DISCONNECT_IN_20_SECONDS, FORCENICKCHANGE_IN_20_SECONDS);
    } else {
        mapstring(DISCONNECT_IN_1_MINUTE, old_DISCONNECT_IN_1_MINUTE);
        mapstring(DISCONNECT_IN_20_SECONDS, old_DISCONNECT_IN_20_SECONDS);
    }
}

/*************************************************************************/

/* EVENT_SERVER_EOB_ACK: the uplink's burst is over, so join the
 * serverinfo channel (see service.h). */
static int do_eob_ack(void)
{
    service_join_channel(&nickserv_service);
    return 0;
}

static int nickserv_init(Module *module)
{
    handle_config();

    if (!new_commandlist(module) || !register_commands(module, cmds)) {
        module_log("Unable to register commands");
        return 0;
    }
    cmd_REGISTER = lookup_cmd(module, "REGISTER");
    if (!cmd_REGISTER) {
        module_log("BUG: unable to find REGISTER command entry");
        return 0;
    }
    cmd_DROPEMAIL = lookup_cmd(module, "DROPEMAIL");
    if (!cmd_DROPEMAIL) {
        module_log("BUG: unable to find DROPEMAIL command entry");
        return 0;
    }
    cmd_DROPEMAIL_CONFIRM = lookup_cmd(module, "DROPEMAIL-CONFIRM");
    if (!cmd_DROPEMAIL_CONFIRM) {
        module_log("BUG: unable to find DROPEMAIL-CONFIRM command entry");
        return 0;
    }
    cmd_GETPASS = lookup_cmd(module, "GETPASS");
    if (!cmd_GETPASS) {
        module_log("BUG: unable to find GETPASS command entry");
        return 0;
    }
    if (!NSEnableRegister)
        cmd_REGISTER->name = "";
    if (!NSEnableDropEmail) {
        cmd_DROPEMAIL->name = "";
        cmd_DROPEMAIL_CONFIRM->name = "";
    }
    if (!EnableGetpass)
        cmd_GETPASS->name = "";

    check_expire_event = event_declare(module, NICKSERV_EVENT_CHECK_EXPIRE);
    command_event = event_declare(module, NICKSERV_EVENT_COMMAND);
    help_event = event_declare(module, NICKSERV_EVENT_HELP);
    help_cmds_event = event_declare(module, NICKSERV_EVENT_HELP_COMMANDS);
    reglink_check_event = event_declare(module, NICKSERV_EVENT_REGISTER_CHECK);
    registered_event = event_declare(module, NICKSERV_EVENT_REGISTERED);
    id_check_event = event_declare(module, NICKSERV_EVENT_IDENTIFY_CHECK);
    identified_event = event_declare(module, NICKSERV_EVENT_IDENTIFIED);
    validated_event = event_declare(module, NICKSERV_EVENT_USER_VALIDATED);
    if (!check_expire_event || !command_event || !help_event
     || !help_cmds_event || !reglink_check_event || !registered_event
     || !id_check_event || !identified_event || !validated_event
    ) {
        module_log("Unable to declare events");
        return 0;
    }

    /* A user talking to any pseudo-client is validated first. */
    if (!event_attach(module, EVENT_COMMAND_LINE, do_command_line)
     || !event_attach_priority(module, EVENT_MESSAGE_PRIVMSG,
                               validate_before_privmsg, EVENT_PRIORITY_FIRST)
     || !event_attach(module, EVENT_USER_CREATE, do_user_create)
     || !event_attach(module, EVENT_USER_NICK_CHANGE_BEFORE,
                      do_user_nickchange_before)
     || !event_attach(module, EVENT_USER_NICK_CHANGE_AFTER,
                      do_user_nickchange_after)
     || !event_attach(module, EVENT_USER_DELETE, do_user_delete)
     || !event_attach(module, OPERSERV_EVENT_STATS_ALL, do_stats_all)
     || !event_attach(module, NICKSERV_EVENT_REGISTER_CHECK, do_reglink_check)
     || !event_attach(module, EVENT_SERVER_EOB_ACK, do_eob_ack)
    ) {
        module_log("Unable to attach event handlers");
        return 0;
    }

    /* The tables exist: this module's migrations were applied when it was
     * loaded (MODULE_APPLY_MIGRATIONS). */
    if (!store_register(&ngi_type) || !store_register(&nick_type)) {
        module_log("Unable to register the record types");
        return 0;
    }
    expire_timeout = add_timeout(EXPIRE_INTERVAL, expire_check, 1);

    if (!init_collide() || !init_set() || !init_util()) {
        return 0;
    }

    old_REGISTER_SYNTAX =
        mapstring(NICK_REGISTER_SYNTAX, NICK_REGISTER_SYNTAX);
    old_HELP_REGISTER_EMAIL =
        mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL);
    old_HELP_UNSET =
        mapstring(NICK_HELP_UNSET, NICK_HELP_UNSET);
    old_DISCONNECT_IN_1_MINUTE =
        mapstring(DISCONNECT_IN_1_MINUTE, DISCONNECT_IN_1_MINUTE);
    old_DISCONNECT_IN_20_SECONDS =
        mapstring(DISCONNECT_IN_20_SECONDS, DISCONNECT_IN_20_SECONDS);
    if (NSRequireEmail) {
        mapstring(NICK_REGISTER_SYNTAX, NICK_REGISTER_REQ_EMAIL_SYNTAX);
        mapstring(NICK_HELP_REGISTER_EMAIL, NICK_HELP_REGISTER_EMAIL_REQ);
        mapstring(NICK_HELP_UNSET, NICK_HELP_UNSET_REQ_EMAIL);
    }
    if (NSForceNickChange) {
        mapstring(DISCONNECT_IN_1_MINUTE, FORCENICKCHANGE_IN_1_MINUTE);
        mapstring(DISCONNECT_IN_20_SECONDS, FORCENICKCHANGE_IN_20_SECONDS);
    }

    return 1;
}

/*************************************************************************/

static int nickserv_fini(Module *module, int shutdown)
{
    if (old_REGISTER_SYNTAX >= 0) {
        mapstring(NICK_REGISTER_SYNTAX, old_REGISTER_SYNTAX);
        old_REGISTER_SYNTAX = -1;
    }
    if (old_HELP_REGISTER_EMAIL >= 0) {
        mapstring(NICK_HELP_REGISTER_EMAIL, old_HELP_REGISTER_EMAIL);
        old_HELP_REGISTER_EMAIL = -1;
    }
    if (old_HELP_UNSET >= 0) {
        mapstring(NICK_HELP_UNSET, old_HELP_UNSET);
        old_HELP_UNSET = -1;
    }
    if (old_DISCONNECT_IN_1_MINUTE >= 0) {
        mapstring(DISCONNECT_IN_1_MINUTE, old_DISCONNECT_IN_1_MINUTE);
        old_DISCONNECT_IN_1_MINUTE = -1;
    }
    if (old_DISCONNECT_IN_20_SECONDS >= 0) {
        mapstring(DISCONNECT_IN_20_SECONDS, old_DISCONNECT_IN_20_SECONDS);
        old_DISCONNECT_IN_20_SECONDS = -1;
    }

    exit_collide();

    if (expire_timeout) {
        del_timeout(expire_timeout);
        expire_timeout = NULL;
    }
    /* Let go of everything the users on the network hold, so that the
     * records are written and released before their type goes. */
    if (nick_type.state) {
        User *u;
        for (u = first_user(); u; u = next_user()) {
            int i;
            validate_cancel(u);
            ARRAY_FOREACH (i, u->id_nicks)
                release_identified(u, u->id_nicks[i]);
            free(u->id_nicks);
            u->id_nicks = NULL;
            u->id_nicks_count = 0;
            cancel_user(u);
        }
        store_collect();
    }
    store_unregister(&nick_type);
    store_unregister(&ngi_type);

    /* These are static, so the pointers don't need to be cleared */
    if (cmd_GETPASS)
        cmd_GETPASS->name = "GETPASS";
    if (cmd_DROPEMAIL_CONFIRM)
        cmd_DROPEMAIL_CONFIRM->name = "DROPEMAIL-CONFIRM";
    if (cmd_DROPEMAIL)
        cmd_DROPEMAIL->name = "DROPEMAIL";
    if (cmd_REGISTER)
        cmd_REGISTER->name = "REGISTER";
    unregister_commands(module, cmds);
    del_commandlist(module);

    return 1;
}

/*************************************************************************/

/* NickServName = <nick>, <description>; in the module block. */
struct Service nickserv_service = {
    .directive = "NickServName",
    .flags = SERVICE_OPER | SERVICE_NICKSERV,
    .on_message = nickserv_message,
};

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "NickServ: nickname registration",
    .requires = MODULE_REQUIRES("operserv/main"),
    .config = nickserv_config,
    .services = MODULE_SERVICES(&nickserv_service),
    .flags = MODULE_APPLY_MIGRATIONS,
    .init = nickserv_init,
    .fini = nickserv_fini,
    .rehash = nickserv_rehash,
};

/*************************************************************************/


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
