/* Routines to load/save Services databases in the obsolete format used
 * by version 4.x (and 5.0).
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
#include "databases.h"
#include "hash.h"
#include "language.h"
#include "encrypt.h"

#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/memoserv/memoserv.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/operserv/akill.h"
#include "modules/operserv/news.h"
#include "modules/operserv/sline.h"
#include "modules/statserv/statserv.h"

#include "extsyms.h"

/* Avoid symbol clashes with database/standard when using static modules */
#define check_file_version version4_check_file_version
#define get_file_version version4_get_file_version
#define write_file_version version4_write_file_version
#define open_db version4_open_db
#define restore_db version4_restore_db
#define close_db version4_close_db
#define read_int8 version4_read_int8
#define read_uint8 version4_read_uint8
#define write_int8 version4_write_int8
#define read_int16 version4_read_int16
#define read_uint16 version4_read_uint16
#define write_int16 version4_write_int16
#define read_int32 version4_read_int32
#define read_uint32 version4_read_uint32
#define write_int32 version4_write_int32
#define read_time version4_read_time
#define write_time version4_write_time
#define read_ptr version4_read_ptr
#define write_ptr version4_write_ptr
#define read_string version4_read_string
#define write_string version4_write_string
#include "fileutil.c"

#define SAFE(x) do { if ((x) < 0) goto fail; } while (0)

/*************************************************************************/

#define FILE_VERSION      11    /* Must remain constant */
#define LOCAL_VERSION    100    /* For extensions to database files */
#define FIRST_VERSION_51 100    /* First extension version in 5.1 */
#define LOCAL_VERSION_50  27    /* Version we show to 5.0 */

/* LOCAL_VERSION change history:
 * [5.1]
 *   100: First version
 * [5.0]
 *    27: Added Bahamut +j handling (ci->mlock.joinrate{1,2} fields)
 *    26: Forced AUTODEOP and NOJOIN to default values
 *    25: Added trircd +J handling (ci->mlock.joindelay field)
 *    24: Moved nickname authentication reason into its own field (no
 *           longer stored as part of authentication code)
 *    23: Added count to autokick entries in channel extension data
 *    22: Fixed bug causing nickgroups with ID 0 to get written out
 *    21: AUTODEOP and NOJOIN levels changed from -10/-20 to -1/-100
 *    20: Access levels changed; v5 level data and access entry levels
 *           added to channel extension data
 *    19: Added last IDENTIFY stamp to nickname extension data
 *    18: Added autojoin functionality; added autojoin list to nickgroup
 *           extension data
 *    17: Added memo ignore functionality; added ignore list to nickgroup
 *           extension data
 *    16: Added Unreal +L/+f handling; added respective fields to channel
 *           extension data
 *    15: Added nick timezones; added timezone field to nickgroup extension
 *           data
 *    14: Added autokick time and lastused fields (saved to channel
 *           extension data)
 *    13: Added nickname privilege level to nickgroup extension data
 */


/* Default channel entries in version 4.5: */
#define CA_SIZE_4_5             18
#define ACCLEV_INVALID_4_5      -10000
static int def_levels_4_5[CA_SIZE_4_5] = {
    /* CA_INVITE */         5,
    /* CA_AKICK */         10,
    /* CA_SET */ ACCLEV_INVALID_4_5,
    /* CA_UNBAN */          5,
    /* CA_AUTOOP */         5,
    /* CA_AUTODEOP */      -1,
    /* CA_AUTOVOICE */      3,
    /* CA_OPDEOP */         5,
    /* CA_ACCESS_LIST */    0,
    /* CA_CLEAR */ ACCLEV_INVALID_4_5,
    /* CA_NOJOIN */        -2,
    /* CA_ACCESS_CHANGE */ 10,
    /* CA_MEMO */          10,
    /* CA_VOICE */          3,
    /* CA_AUTOHALFOP */     4,
    /* CA_HALFOP */         4,
    /* CA_AUTOPROTECT */   10,
    /* CA_PROTECT */       10,
};

/* For handling servadmins/servopers in 4.5.x databases: */
#define MAX_SERVOPERS   256
#define MAX_SERVADMINS  256
static nickname_t services_admins[MAX_SERVADMINS];
static nickname_t services_opers[MAX_SERVOPERS];
static int services_admins_count = 0, services_opers_count = 0;

/* Database file names (loaded from modules.conf): */
static char *NickDBName;
static char *ChanDBName;
static char *OperDBName;
static char *NewsDBName;
static char *AutokillDBName;
static char *ExceptionDBName;
static char *SlineDBName;
static char *StatDBName;

/*************************************************************************/
/**************************** Table load/save ****************************/
/*************************************************************************/

/* Forward declarations of individual load/save routines */

static int load_nick_table(DBTable *nick_table, DBTable *nickgroup_table);
static int save_nick_table(DBTable *nick_table, DBTable *nickgroup_table);
static int load_chan_table(DBTable *table);
static int save_chan_table(DBTable *table);
static int load_oper_table(DBTable *table);
static int save_oper_table(DBTable *table);
static int load_news_table(DBTable *table);
static int save_news_table(DBTable *table);
static int load_akill_table(DBTable *akill_table, DBTable *exclude_table);
static int save_akill_table(DBTable *akill_table, DBTable *exclude_table);
static int load_exception_table(DBTable *table);
static int save_exception_table(DBTable *table);
static int load_sline_table(DBTable *sgline_table, DBTable *sqline_table,
                            DBTable *szline_table);
static int save_sline_table(DBTable *sgline_table, DBTable *sqline_table,
                            DBTable *szline_table);
static int load_stat_servers_table(DBTable *table);
static int save_stat_servers_table(DBTable *table);
static int load_generic_table(DBTable *table);
static int save_generic_table(DBTable *table);

/*************************************************************************/

static int version4_load_table(DBTable *table)
{
    int retval = 1;

    /* In order to load nicknames properly, we need access to both nick
     * and nickgroup tables, so we save the value of each table when it's
     * passed in, and only call load_nick_table() when we have both values
     * (after which we clear them for the next time around). */
    static DBTable *nick_table = NULL, *nickgroup_table = NULL;
    /* Likewise for the autokill/exclude and S-line tables. */
    static DBTable *akill_table = NULL, *exclude_table = NULL,
                   *sgline_table = NULL, *sqline_table = NULL,
                   *szline_table = NULL;

    /* Most load routines have to go through the data multiple times to
     * read it in, due to the workarounds used to maintain backwards
     * compatibility.  The expiration timestamps may not be correct during
     * this time, so disable expiration while the load is in progress. */
    int saved_noexpire = noexpire;
    noexpire = 1;

    if (strcmp(table->name, "nick") == 0) {
        nick_table = table;
    } else if (strcmp(table->name, "nickgroup") == 0) {
        nickgroup_table = table;
    } else if (strcmp(table->name, "nick-access") == 0
            || strcmp(table->name, "nick-autojoin") == 0
            || strcmp(table->name, "memo") == 0
            || strcmp(table->name, "memo-ignore") == 0) {
        /* ignore */
    } else if (strcmp(table->name, "chan") == 0) {
        retval = load_chan_table(table);
    } else if (strcmp(table->name, "chan-access") == 0
            || strcmp(table->name, "chan-akick") == 0) {
        /* ignore */
    } else if (strcmp(table->name, "oper") == 0) {
        retval = load_oper_table(table);
    } else if (strcmp(table->name, "news") == 0) {
        retval = load_news_table(table);
    } else if (strcmp(table->name, "akill") == 0) {
        akill_table = table;
    } else if (strcmp(table->name, "exclude") == 0) {
        exclude_table = table;
    } else if (strcmp(table->name, "exception") == 0) {
        retval = load_exception_table(table);
    } else if (strcmp(table->name, "sgline") == 0) {
        sgline_table = table;
    } else if (strcmp(table->name, "sqline") == 0) {
        sqline_table = table;
    } else if (strcmp(table->name, "szline") == 0) {
        szline_table = table;
    } else if (strcmp(table->name, "stat-servers") == 0) {
        retval = load_stat_servers_table(table);
    } else {
        retval = load_generic_table(table);
    }
    if (nick_table && nickgroup_table) {
        /* Got both tables, run the load routine and clear the variables */
        retval = load_nick_table(nick_table, nickgroup_table);
        nick_table = nickgroup_table = NULL;
    }
    if (akill_table && exclude_table) {
        retval = load_akill_table(akill_table, exclude_table);
        akill_table = exclude_table = NULL;
    }
    if (sgline_table && sqline_table && szline_table) {
        retval = load_sline_table(sgline_table, sqline_table, szline_table);
        sgline_table = sqline_table = szline_table = NULL;
    }

    noexpire = saved_noexpire;

    if (retval && table->postload && !(*table->postload)()) {
        module_log_perror("Table %s postload routine failed", table->name);
        retval = 0;
    }

    return retval;
}

/*************************************************************************/

static int version4_save_table(DBTable *table)
{
    /* See version4_load_table() for why we need these */
    static DBTable *nick_table = NULL, *nickgroup_table = NULL,
                   *akill_table = NULL, *exclude_table = NULL,
                   *sgline_table = NULL, *sqline_table = NULL,
                   *szline_table = NULL;

    if (strcmp(table->name, "nick") == 0) {
        nick_table = table;
    } else if (strcmp(table->name, "nickgroup") == 0) {
        nickgroup_table = table;
    } else if (strcmp(table->name, "nick-access") == 0
            || strcmp(table->name, "nick-autojoin") == 0
            || strcmp(table->name, "memo") == 0
            || strcmp(table->name, "memo-ignore") == 0) {
        /* ignore */
    } else if (strcmp(table->name, "chan") == 0) {
        return save_chan_table(table);
    } else if (strcmp(table->name, "chan-access") == 0
            || strcmp(table->name, "chan-akick") == 0) {
        /* ignore */
    } else if (strcmp(table->name, "oper") == 0) {
        return save_oper_table(table);
    } else if (strcmp(table->name, "news") == 0) {
        return save_news_table(table);
    } else if (strcmp(table->name, "akill") == 0) {
        akill_table = table;
    } else if (strcmp(table->name, "exclude") == 0) {
        exclude_table = table;
    } else if (strcmp(table->name, "exception") == 0) {
        return save_exception_table(table);
    } else if (strcmp(table->name, "sgline") == 0) {
        sgline_table = table;
    } else if (strcmp(table->name, "sqline") == 0) {
        sqline_table = table;
    } else if (strcmp(table->name, "szline") == 0) {
        szline_table = table;
    } else if (strcmp(table->name, "stat-servers") == 0) {
        return save_stat_servers_table(table);
    } else {
        return save_generic_table(table);
    }
    if (nick_table && nickgroup_table) {
        int retval = save_nick_table(nick_table, nickgroup_table);
        nick_table = nickgroup_table = NULL;
        return retval;
    }
    if (akill_table && exclude_table) {
        int retval = save_akill_table(akill_table, exclude_table);
        akill_table = exclude_table = NULL;
        return retval;
    }
    if (sgline_table && sqline_table && szline_table) {
        int retval =
            save_sline_table(sgline_table, sqline_table, szline_table);
        sgline_table = sqline_table = szline_table = NULL;
        return retval;
    }

    return 1;
}

/*************************************************************************/
/*************************************************************************/

/* Common routine to open a file for reading and check version number. */

#define OPENDB_ERROR    ((dbFILE *)PTR_INVALID)

static dbFILE *my_open_db_r(const char *dbname, int32 *ver_ret)
{
    dbFILE *f;
    int32 ver;

    f = open_db(dbname, "r", 0);
    if (!f)
        return NULL;
    ver = get_file_version(f);
    if (ver < 5 || ver > 11) {
        if (ver == -1) {
            module_log("Unable to read version number from %s",
                dbname);
        } else {
            module_log("Invalid version number %d on %s", ver,
                dbname);
        }
        close_db(f);
        return OPENDB_ERROR;
    }
    *ver_ret = ver;
    return f;
}

/*************************************************************************/

/* Read a MaskData category from a file. */

static int read_maskdata(DBTable *table, uint8 type, const char *dbname,
                         dbFILE *f)
{
    int32 ver;
    MaskData *md;
    int16 count;
    int i;

#if CLEAN_COMPILE
    count = 0;
#endif
    SAFE(read_int16(&count, f));
    for (i = 0; i < count; i++) {
        int32 tmp32;
        md = table->newrec();
        SAFE(read_string(&md->mask, f));
        if (type == MD_EXCEPTION) {
            SAFE(read_int16(&md->limit, f));
            SAFE(read_buffer(md->who, f));
            SAFE(read_string(&md->reason, f));
        } else {
            SAFE(read_string(&md->reason, f));
            SAFE(read_buffer(md->who, f));
        }
        SAFE(read_int32(&tmp32, f));
        md->time = tmp32;
        SAFE(read_int32(&tmp32, f));
        md->expires = tmp32;
        md->num = i+1;
        table->insert(md);
    }
    if (read_int32(&ver, f) == 0) {
        if (ver <= FILE_VERSION || ver > LOCAL_VERSION) {
            module_log("Invalid extension data version in %s", dbname);
            return 0;
        }
        for (i = 0; i < count; i++) {
            /* O(n^2), but we can't help it if we want to be robust */
            for (md = table->first(); md; md = table->next()) {
                if (md->num == i+1)
                    break;
            }
            if (md) {
                SAFE(read_time(&md->time, f));
                SAFE(read_time(&md->expires, f));
                SAFE(read_time(&md->lastused, f));
            }
        }
    }
    return 1;

  fail:
    close_db(f);
    module_log("Read error on %s", dbname);
    return 0;
}

/*************************************************************************/

/* Write a MaskData category to a file. */

static int write_maskdata(DBTable *table, uint8 type, const char *dbname,
                          dbFILE *f)
{
    static time_t lastwarn[256];
    MaskData *md;
    int count;
    int save_noexpire = noexpire;

    count = 0;
    for (md = table->first(); md; md = table->next())
        count++;
    write_int16(count, f);
    /* Disable expiration for the remainder of this function.  This is a
     * kludge to ensure that nothing expires while we're writing it out. */
    noexpire = 1;
    for (md = table->first(); md; md = table->next()) {
        SAFE(write_string(md->mask, f));
        if (type == MD_EXCEPTION) {
            SAFE(write_int16(md->limit, f));
            SAFE(write_buffer(md->who, f));
            SAFE(write_string(md->reason, f));
        } else {
            SAFE(write_string(md->reason, f));
            SAFE(write_buffer(md->who, f));
        }
        SAFE(write_int32(md->time, f));
        SAFE(write_int32(md->expires, f));
    }
    SAFE(write_int32(LOCAL_VERSION_50, f));
    for (md = table->first(); md; md = table->next()) {
        SAFE(write_time(md->time, f));
        SAFE(write_time(md->expires, f));
        SAFE(write_time(md->lastused, f));
    }
    noexpire = save_noexpire;
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", dbname);
    if (time(NULL) - lastwarn[type] > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", dbname, strerror(errno));
        lastwarn[type] = time(NULL);
    }
    noexpire = save_noexpire;
    return 0;
}

/*************************************************************************/
/********************** NickServ database handling ***********************/
/*************************************************************************/

#define NGI_TEMP_ID     0xFFFFFFFFU  /* until we know the real group */

static NickInfo *load_one_nick(DBTable *nick_table, DBTable *nickgroup_table,
                               dbFILE *f, int32 ver)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    int16 tmp16;
    int32 tmp32;
    int i;
    char passbuf[PASSMAX];
    char *url, *email;

    ni = nick_table->newrec();
    SAFE(read_buffer(ni->nick, f));
    module_log_debug(2, "loading nick %s", ni->nick);
    SAFE(read_buffer(passbuf, f));
    SAFE(read_string(&url, f));
    SAFE(read_string(&email, f));
    SAFE(read_string(&ni->last_usermask, f));
    if (!ni->last_usermask)
        ni->last_usermask = sstrdup("@");
    SAFE(read_string(&ni->last_realname, f));
    if (!ni->last_realname)
        ni->last_realname = sstrdup("");
    SAFE(read_string(&ni->last_quit, f));
    SAFE(read_int32(&tmp32, f));
    ni->time_registered = tmp32;
    SAFE(read_int32(&tmp32, f));
    ni->last_seen = tmp32;
    SAFE(read_int16(&ni->status, f));
    ni->status &= NS_PERMANENT;
    /* Old-style links were hierarchical; if this nick was linked to
     * another, the name of the parent link, else NULL, was stored here.
     * Store that value in ni->last_realmask, which coincidentally was
     * not saved before version 5.0, and resolve links later. */
    SAFE(read_string(&ni->last_realmask, f));
    SAFE(read_int16(&tmp16, f));  /* linkcount */
    if (ni->last_realmask) {
        SAFE(read_int16(&tmp16, f));  /* channelcount */
        free(url);
        free(email);
    } else {
        ngi = nickgroup_table->newrec();
        ngi->id = NGI_TEMP_ID;
        ARRAY_EXTEND(ngi->nicks);
        strbcpy(ngi->nicks[0], ni->nick);
        init_password(&ngi->pass);
        encrypt_password("a", 1, &ngi->pass);  /* get encryption type */
        memcpy(ngi->pass.password, passbuf, PASSMAX);
        ngi->url = url;
        ngi->email = email;
        SAFE(read_int32(&ngi->flags, f));
        if (ngi->flags & NF_KILL_IMMED)
            ngi->flags |= NF_KILL_QUICK;
        ngi->flags |= NF_NOGROUP;
        if (ver >= 9) {
            void *tmpptr;
            SAFE(read_ptr(&tmpptr, f));
            if (tmpptr)
                ngi->flags |= NF_SUSPENDED;
            else
                ngi->flags &= ~NF_SUSPENDED;
        } else if (ver == 8 && (ngi->flags & 0x10000000)) {
            /* In version 8, 0x10000000 was NI_SUSPENDED */
            ngi->flags |= NF_SUSPENDED;
        } else {
            ngi->flags &= ~NF_SUSPENDED;  /* just in case */
        }
        if (ngi->flags & NF_SUSPENDED) {
            SAFE(read_buffer(ngi->suspend_who, f));
            SAFE(read_string(&ngi->suspend_reason, f));
            SAFE(read_int32(&tmp32, f));
            ngi->suspend_time = tmp32;
            SAFE(read_int32(&tmp32, f));
            ngi->suspend_expires = tmp32;
        }
        SAFE(read_int16(&ngi->access_count, f));
        if (ngi->access_count) {
            char **access;
            access = smalloc(sizeof(char *) * ngi->access_count);
            ngi->access = access;
            ARRAY_FOREACH (i, ngi->access)
                SAFE(read_string(&ngi->access[i], f));
        }
        SAFE(read_int16(&ngi->memos.memos_count, f));
        SAFE(read_int16(&ngi->memos.memomax, f));
        /*
         * Note that at this stage we have no way of comparing this to the
         * default memo max (MSMaxMemos) because the MemoServ module is not
         * loaded.  If this is a 5.x database, this is not a problem,
         * because the correct memo max value will be stored in the
         * extension data, but if not, we need to check and change the
         * value to MEMOMAX_DEFAULT as needed.  This is handled by a
         * callback (nick_memomax_callback() below) which triggers when the
         * MemoServ module is loaded.  The callback is added by
         * open_nick_db() if needed.
         */
        if (ngi->memos.memos_count) {
            Memo *memos;
            memos = smalloc(sizeof(Memo) * ngi->memos.memos_count);
            ngi->memos.memos = memos;
            ARRAY_FOREACH (i, ngi->memos.memos) {
                SAFE(read_uint32(&ngi->memos.memos[i].number, f));
                SAFE(read_int16(&ngi->memos.memos[i].flags, f));
                SAFE(read_int32(&tmp32, f));
                ngi->memos.memos[i].time = tmp32;
                if (ngi->memos.memos[i].flags & MF_UNREAD)
                    ngi->memos.memos[i].firstread = 0;
                else
                    ngi->memos.memos[i].firstread = tmp32;
                SAFE(read_buffer(ngi->memos.memos[i].sender, f));
                ngi->memos.memos[i].channel = NULL;
                SAFE(read_string(&ngi->memos.memos[i].text, f));
            }
        }
        /* Channel counts are recalculated by open_channel_db() */
        SAFE(read_int16(&tmp16, f));  /* channelcount */
        /* If this is a 5.x database, we now get the real nickgroup value
         * from bits 30-15 of the flags and the 16 bits we just read; the
         * real flags value is stored in the extension data. */
        if (ngi->flags & 0x80000000) {
            ngi->id = (ngi->flags & 0x7FFF8000) << 1 | ((int)tmp16 & 0xFFFF);
            ngi->flags &= ~NF_NOGROUP;
        }
        /* There was no way to set channel limits, so must be the default.
         * Note that if this is a 5.x database, the correct value for this
         * field (as well as memomax and language) will be read from the
         * extension data. */
        SAFE(read_int16(&tmp16, f));  /* channelmax */
        ngi->channelmax = CHANMAX_DEFAULT;
        SAFE(read_int16(&ngi->language, f));
        if (!have_language(ngi->language))
            ngi->language = LANG_DEFAULT;
        /* Ver 4.x had no way to say "use the default language", so set that
         * for all nicks that are using DEF_LANGUAGE */
        if (ngi->language == DEF_LANGUAGE)
            ngi->language = LANG_DEFAULT;
        ngi->timezone = TIMEZONE_DEFAULT;
        ni->nickgroup = ngi->id;
        if (ngi->id != 0) {
            nickgroup_table->insert(ngi);
        } else {
            nickgroup_table->freerec(ngi);
            if (!(ni->status & NS_VERBOTEN)) {
                module_log("warning: nick %s has no nick group but is not"
                           " forbidden (corrupt database or BUG?)", ni->nick);
            }
        }
    }
    return ni;

  fail:
    module_log("Read error on %s", f->filename);
    return NULL;
}

/*************************************************************************/

/* Load extension data for a nick.  Returns zero on success, nonzero on
 * failure.
 */

static int load_one_nick_ext(DBTable *nick_table, DBTable *nickgroup_table,
                             dbFILE *f, int32 ver)
{
    char *nick;
    NickInfo *ni = NULL;
    NickInfo dummy_ni;  /* for nonexistent nicks */

    SAFE(read_string(&nick, f));
    if (!nick)
        goto fail;
    module_log_debug(2, "loading nick extension %s", nick);
    if (!(ni = get_nickinfo(nick))) {
        module_log("Extension data found for nonexistent nick `%s'", nick);
        ni = &dummy_ni;
        memset(ni, 0, sizeof(*ni));
    }
    free(nick);
    nick = NULL;
    free(ni->last_realmask);  /* copied from last_usermask */
    SAFE(read_string(&ni->last_realmask, f));
    if (ver >= 19)
        SAFE(read_uint32(&ni->id_stamp, f));
    if (ni == &dummy_ni)
        free(ni->last_realmask);
    else
        put_nickinfo(ni);
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    if (ni != &dummy_ni)
        put_nickinfo(ni);
    return 0;
}

/*************************************************************************/

/* Load extension data for a nick group. */

static int load_one_nickgroup_ext(DBTable *table, dbFILE *f, int32 ver)
{
    uint32 group;
    NickGroupInfo *ngi = NULL;
    NickGroupInfo dummy_ngi;  /* for nonexistent nick groups */
    int i;

    SAFE(read_uint32(&group, f));
    module_log_debug(2, "loading nickgroup extension %u", group);
    if (!group) {
        if (ver < 22) {
            module_log("Ignoring nickgroup 0 (bug in previous versions)");
            ngi = &dummy_ngi;
            memset(ngi, 0, sizeof(*ngi));
        } else {
            goto fail;
        }
    } else if (!(ngi = get_nickgroupinfo(group))) {
        module_log("Extension data found for nonexistent nick group %u",
                   group);
        ngi = &dummy_ngi;
        memset(ngi, 0, sizeof(*ngi));
    }
    SAFE(read_int32(&ngi->flags, f));
    SAFE(read_int32(&ngi->authcode, f));
    SAFE(read_time(&ngi->authset, f));
    if (ver >= 24) {
        SAFE(read_int16(&ngi->authreason, f));
    } else {
        switch ((ngi->authcode & 0x300) >> 8) {
            case 0 : ngi->authreason = NICKAUTH_REGISTER;  break;
            case 1 : ngi->authreason = NICKAUTH_SET_EMAIL; break;
            case 2 : ngi->authreason = NICKAUTH_SETAUTH;   break;
            default: ngi->authreason = 0;                  break;
        }
    }
    SAFE(read_int16(&ngi->channelmax, f));
    if (ver >= 18) {
        SAFE(read_int16(&ngi->ajoin_count, f));
        if (ngi->ajoin_count) {
            ngi->ajoin = smalloc(sizeof(char *) * ngi->ajoin_count);
            ARRAY_FOREACH (i, ngi->ajoin)
                SAFE(read_string(&ngi->ajoin[i], f));
        }
    }
    SAFE(read_int16(&ngi->memos.memomax, f));
    if (ver >= 17) {
        SAFE(read_int16(&ngi->ignore_count, f));
        if (ngi->ignore_count) {
            ngi->ignore = smalloc(sizeof(char *) * ngi->ignore_count);
            ARRAY_FOREACH (i, ngi->ignore)
                SAFE(read_string(&ngi->ignore[i], f));
        }
    }
    SAFE(read_int16(&ngi->language, f));
    if (!have_language(ngi->language))
        ngi->language = LANG_DEFAULT;
    if (ver >= 15)
        SAFE(read_int16(&ngi->timezone, f));
    SAFE(read_string(&ngi->info, f));
    if (ver >= 13)
        SAFE(read_int16(&ngi->os_priv, f));
    if (ngi == &dummy_ngi) {
        ARRAY_FOREACH (i, ngi->ajoin)
            free(ngi->ajoin[i]);
        free(ngi->ajoin);
        ARRAY_FOREACH (i, ngi->ignore)
            free(ngi->ignore[i]);
        free(ngi->ignore);
    } else {
        put_nickgroupinfo(ngi);
    }
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    if (ngi != &dummy_ngi)
        put_nickgroupinfo(ngi);
    return 0;
}

/*************************************************************************/

/* Load version 5.1 extension data for a nick group. */

static int load_one_nickgroup_ext51(DBTable *table, dbFILE *f, int32 ver)
{
    uint16 tmp16;
    uint32 group;
    NickGroupInfo *ngi = NULL;
    NickGroupInfo dummy_ngi;  /* for nonexistent nick groups */
    int i;

    SAFE(read_uint32(&group, f));
    module_log_debug(2, "loading nickgroup extension %u", group);
    if (!group) {
        goto fail;
    } else if (!(ngi = get_nickgroupinfo(group))) {
        module_log("5.1 extension data found for nonexistent nick group %u",
                   group);
        ngi = &dummy_ngi;
        memset(ngi, 0, sizeof(*ngi));
        init_password(&ngi->pass);
    }
    SAFE(read_string(&ngi->last_email, f));
    SAFE(read_uint16(&tmp16, f));
    if (ngi != &dummy_ngi && tmp16 != ngi->memos.memos_count) {
        module_log("Warning: memo count mismatch on nickgroup %u"
                   " (main data: %d, ext51 data: %d)", ngi->id,
                   ngi->memos.memos_count, tmp16);
    }
    for (i = 0; i < tmp16; i++) {
        time_t t;
        char *s;
        SAFE(read_time(&t, f));
        SAFE(read_string(&s, f));
        if (ngi != &dummy_ngi && i < ngi->memos.memos_count) {
            ngi->memos.memos[i].firstread = t;
            ngi->memos.memos[i].channel = s;
        } else {
            free(s);
        }
    }
    free((char *)ngi->pass.cipher);
    SAFE(read_string((char **)&ngi->pass.cipher, f));
    if (ngi == &dummy_ngi) {
        ARRAY_FOREACH (i, ngi->ajoin)
            free(ngi->ajoin[i]);
        free(ngi->ajoin);
        ARRAY_FOREACH (i, ngi->ignore)
            free(ngi->ignore[i]);
        free(ngi->ignore);
        clear_password(&ngi->pass);
    } else {
        put_nickgroupinfo(ngi);
    }
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    if (ngi != &dummy_ngi)
        put_nickgroupinfo(ngi);
    return 0;
}

/*************************************************************************/

static int nick_memomax_callback(Module *mod, const char *name)
{
    NickGroupInfo *ngi;

    if (strcmp(name, "memoserv/main") != 0)
        return 0;
    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {
        if (ngi->memos.memomax == MSMaxMemos)
            ngi->memos.memomax = MEMOMAX_DEFAULT;
    }
    /* We only need to do this once */
    remove_callback(NULL, "load module", nick_memomax_callback);
    return 0;
}

/*************************************************************************/

static int load_nick_table(DBTable *nick_table, DBTable *nickgroup_table)
{
    dbFILE *f;
    int32 ver;
    int i, c;
    NickInfo *ni;
    NickGroupInfo *ngi;
    int failed = 0, need_memomax_check = 1;

    /* Open database. */
    if (!(f = my_open_db_r(NickDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;

    /* Load original data. */
    for (i = 0; i < 256 && !failed; i++) {
        while ((c = getc_db(f)) != 0) {
            if (c != 1) {
                module_log("Invalid format in nick.db");
                failed = 1;
            }
            ni = load_one_nick(nick_table, nickgroup_table, f, ver);
            if (ni) {
                nick_table->insert(ni);
            } else {
                failed = 1;
            }
        }
    }

    /* Assign nickgroup IDs to groups that don't have them (e.g. from a
     * 4.5 database) */
    for (ngi = nickgroup_table->first(); ngi; ngi = nickgroup_table->next()) {
        if (ngi->flags & NF_NOGROUP) {
#define NEWNICKGROUP_TRIES 1000
            int tries;
            for (tries = 0; tries < NEWNICKGROUP_TRIES; tries++) {
                uint32 id = rand() + rand();
                if (id == 0 || id == NGI_TEMP_ID)
                    id = 1;
                if (!get_nickgroupinfo(id)) {
                    ngi->id = id;
                    break;
                }
            }
            if (tries >= NEWNICKGROUP_TRIES) {
                module_log("load_nick_table(): unable to assign new ID to"
                           " nickgroup for nick %s--dropping", ngi->nicks[0]);
                ARRAY_FOREACH (i, ngi->nicks) {
                    if ((ni = get_nickinfo(ngi->nicks[i])) != NULL)
                        del_nickinfo(ni);
                }
                del_nickgroupinfo(ngi);
            }
        }
    }

    /* Resolve links.  First point each last_realmask field at the
     * NickInfo * of the appropriate nick; then copy the nickgroup ID from
     * each root nick to all of its children, effectively collapsing the
     * link hierarchies to a single level, and add the child nicks to the
     * root nickgroup's nick array.
     */
    for (ni = nick_table->first(); ni; ni = nick_table->next()) {
        if (ni->last_realmask) {
            char *s = ni->last_realmask;
            ni->last_realmask = (char *)get_nickinfo(s);
            free(s);
        }
    }
    for (ni = nick_table->first(); ni; ni = nick_table->next()) {
        if (ni->last_realmask) {
            NickInfo *root = (NickInfo *)ni->last_realmask;
            NickGroupInfo *ngi;
            while (root->last_realmask)
                root = (NickInfo *)root->last_realmask;
            ni->nickgroup = root->nickgroup;
            ngi = get_nickgroupinfo(ni->nickgroup);
            if (!ngi) {
                module_log("BUG: Unable to find nickgroup %u for linked"
                           " nick %s (parent = %s, root = %s)",
                           ni->nickgroup, ni->nick,
                           ((NickInfo *)ni->last_realmask)->nick,
                           root->nick);
            } else {
                ARRAY_EXTEND(ngi->nicks);
                strbcpy(ngi->nicks[ngi->nicks_count-1], ni->nick);
                put_nickgroupinfo(ngi);
            }
        }
        if (!ni->nickgroup && !(ni->status & NS_VERBOTEN)) {
            module_log("Nick %s has no settings (linked to missing nick?),"
                       " deleting", ni->nick);
            if (ni->last_realmask)
                put_nickinfo((NickInfo *)ni->last_realmask);
            ni->last_realmask = NULL;  /* Don't free someone else's NickInfo */
            del_nickinfo(ni);
        }
    }

    /* Copy all last_usermask fields to last_realmask. */
    for (ni = nick_table->first(); ni; ni = nick_table->next()) {
        if (ni->last_realmask)
            put_nickinfo((NickInfo *)ni->last_realmask);
        ni->last_realmask = sstrdup(ni->last_usermask);
    }

    /* Load extension data if present. */
    ver = 0;
    if (read_int32(&ver, f) == 0) {
        if (ver <= FILE_VERSION || ver > LOCAL_VERSION_50) {
            module_log("Invalid extension data version in nick.db");
            failed = 1;
        } else {
            while (!failed && (c = getc_db(f)) != 0) {
                if (c != 1) {
                    module_log("Invalid format in nick.db extension data");
                    failed = 1;
                } else if (!load_one_nick_ext(nick_table, nickgroup_table,
                                              f, ver)) {
                    failed = 1;
                }
            }
            while (!failed && (c = getc_db(f)) != 0) {
                if (c != 1) {
                    module_log("Invalid format in nick.db extension data");
                    failed = 1;
                } else if (!load_one_nickgroup_ext(nickgroup_table, f, ver)) {
                    failed = 1;
                }
            }
        }
        need_memomax_check = 0;
    }
    if (read_int32(&ver, f) == 0) {
        if (ver < FIRST_VERSION_51 || ver > LOCAL_VERSION) {
            module_log("Invalid extension (5.1) data version in nick.db");
            failed = 1;
        } else {
            while (!failed && (c = getc_db(f)) != 0) {
                if (c != 1) {
                    module_log("Invalid format in nick.db extension (5.1)"
                               " data");
                    failed = 1;
                } else if (!load_one_nickgroup_ext51(nickgroup_table, f, ver)){
                    failed = 1;
                }
            }
        }
    }

    if (!failed && ver < 13) {
        /* Need to restore Services admin/oper privs from oper.db lists */
        NickGroupInfo *ngi;
        ARRAY_FOREACH (i, services_admins) {
            ni = get_nickinfo(services_admins[i]);
            if (ni != NULL && (ngi = get_ngi(ni)) != NULL) {
                ngi->os_priv = NP_SERVADMIN;
                put_nickgroupinfo(ngi);
            }
            put_nickinfo(ni);
        }
        ARRAY_FOREACH (i, services_opers) {
            ni = get_nickinfo(services_opers[i]);
            if (ni != NULL && (ngi = get_ngi(ni)) != NULL) {
                ngi->os_priv = NP_SERVOPER;
                put_nickgroupinfo(ngi);
            }
            put_nickinfo(ni);
        }
    }

    /* Add the memomax check callback if needed. */
    if (!failed && need_memomax_check)
        add_callback(NULL, "load module", nick_memomax_callback);

    /* Close database. */
    close_db(f);

    /* All done! */
    return !failed || forceload;
}

/*************************************************************************/

static int save_nick_table(DBTable *nick_table, DBTable *nickgroup_table)
{
    dbFILE *f;
    int i;
    NickInfo *ni;
    NickGroupInfo *ngi;
    static NickGroupInfo forbidden_ngi;  /* dummy for forbidden nicks */
    static time_t lastwarn = 0;

    if (!(f = open_db(NickDBName, "w", 11)))
        return 0;
    for (ni = nick_table->first(); ni; ni = nick_table->next()) {
        if (ni->nickgroup)
            ngi = get_nickgroupinfo(ni->nickgroup);
        else
            ngi = NULL;
        SAFE(write_int8(1, f));
        SAFE(write_buffer(ni->nick, f));
        if (ngi) {
            SAFE(write_buffer(ngi->pass.password, f));
            SAFE(write_string(ngi->url, f));
            SAFE(write_string(ngi->email, f));
        } else {
            Password dummypass;
            init_password(&dummypass);
            if (!(ni->status & NS_VERBOTEN)) {
                module_log("nick %s has no NickGroupInfo, setting password"
                           " to nick", ni->nick);
                encrypt_password(ni->nick, strlen(ni->nick), &dummypass);
            }
            SAFE(write_buffer(dummypass.password, f));
            clear_password(&dummypass);
            SAFE(write_string(NULL, f));
            SAFE(write_string(NULL, f));
        }
        SAFE(write_string(ni->last_usermask, f));
        SAFE(write_string(ni->last_realname, f));
        SAFE(write_string(ni->last_quit, f));
        SAFE(write_int32(ni->time_registered, f));
        SAFE(write_int32(ni->last_seen, f));
        SAFE(write_int16(ni->status, f));
        if (ngi && irc_stricmp(ni->nick, ngi_mainnick(ngi)) != 0) {
            /* Not the main nick in the group; save it as a link */
            SAFE(write_string(ngi_mainnick(ngi), f));
            SAFE(write_int16(0, f));
            SAFE(write_int16(0, f));
        } else {
            int32 tmp32;
            /* Main nick in the group, or forbidden; save as a root nick */
            SAFE(write_string(NULL, f));
            SAFE(write_int16(0, f));
            /* If it's forbidden, use a dummy NickGroupInfo from here on */
            if (!ngi)
                ngi = &forbidden_ngi;
            /* Store top 16 bits of group ID in flags */
            tmp32 = ngi->flags | 0x80000000 | (ngi->id & 0xFFFF0000)>>1;
            if (tmp32 & NF_KILL_IMMED)
                tmp32 &= ~NF_KILL_QUICK;
            SAFE(write_int32(tmp32, f));
            SAFE(write_ptr((ngi->flags & NF_SUSPENDED) ? (void *)1 : NULL, f));
            if (ngi->flags & NF_SUSPENDED) {
                SAFE(write_buffer(ngi->suspend_who, f));
                SAFE(write_string(ngi->suspend_reason, f));
                SAFE(write_int32(ngi->suspend_time, f));
                SAFE(write_int32(ngi->suspend_expires, f));
            }
            SAFE(write_int16(ngi->access_count, f));
            ARRAY_FOREACH (i, ngi->access)
                SAFE(write_string(ngi->access[i], f));
            SAFE(write_int16(ngi->memos.memos_count, f));
            /* Note that we have to save the memo maximum here as a static
             * value; we save the real value (which may be MEMOMAX_DEFAULT)
             * in the extension area below.  The same applies for channelmax
             * and language. */
            if (ngi->memos.memomax == MEMOMAX_DEFAULT)
                SAFE(write_int16(MSMaxMemos, f));
            else
                SAFE(write_int16(ngi->memos.memomax, f));
            ARRAY_FOREACH (i, ngi->memos.memos) {
                SAFE(write_int32(ngi->memos.memos[i].number, f));
                SAFE(write_int16(ngi->memos.memos[i].flags, f));
                SAFE(write_int32(ngi->memos.memos[i].time, f));
                SAFE(write_buffer(ngi->memos.memos[i].sender, f));
                SAFE(write_string(ngi->memos.memos[i].text, f));
            }
            /* Store bottom 16 bits of group ID in channelcount */
            SAFE(write_int16(ngi->id & 0xFFFF, f));
            if (ngi->channelmax == CHANMAX_DEFAULT)
                SAFE(write_int16(CSMaxReg, f));
            else
                SAFE(write_int16(ngi->channelmax, f));
            if (ngi->language == LANG_DEFAULT)
                SAFE(write_int16(DEF_LANGUAGE, f));
            else
                SAFE(write_int16(ngi->language, f));
        }
        put_nickgroupinfo(ngi);
    } /* for (ni) */
    {
        /* This is an UGLY HACK but it simplifies loading. */
        static char buf[256];  /* initialized to zero */
        SAFE(write_buffer(buf, f));
    }

    services_admins_count = services_opers_count = 0;
    SAFE(write_int32(LOCAL_VERSION_50, f));
    for (ni = nick_table->first(); ni; ni = nick_table->next()) {
        SAFE(write_int8(1, f));
        SAFE(write_string(ni->nick, f));
        SAFE(write_string(ni->last_realmask, f));
        SAFE(write_int32(ni->id_stamp, f));
    }
    SAFE(write_int8(0, f));
    for (ngi = nickgroup_table->first(); ngi; ngi = nickgroup_table->next()) {
        if (ngi->id == 0) {
            module_log("BUG: nickgroup with ID 0 found during write"
                       " (skipping)");
            continue;
        }
        SAFE(write_int8(1, f));
        SAFE(write_int32(ngi->id, f));
        SAFE(write_int32(ngi->flags, f));
        SAFE(write_int32(ngi->authcode, f));
        SAFE(write_time(ngi->authset, f));
        SAFE(write_int16(ngi->authreason, f));
        SAFE(write_int16(ngi->channelmax, f));
        SAFE(write_int16(ngi->ajoin_count, f));
        ARRAY_FOREACH (i, ngi->ajoin)
            SAFE(write_string(ngi->ajoin[i], f));
        SAFE(write_int16(ngi->memos.memomax, f));
        SAFE(write_int16(ngi->ignore_count, f));
        ARRAY_FOREACH (i, ngi->ignore)
            SAFE(write_string(ngi->ignore[i], f));
        SAFE(write_int16(ngi->language, f));
        SAFE(write_int16(ngi->timezone, f));
        SAFE(write_string(ngi->info, f));
        SAFE(write_int16(ngi->os_priv, f));
        if (ngi->os_priv >= NP_SERVADMIN) {
            strbcpy(services_admins[services_admins_count++],
                    ngi_mainnick(ngi));
        } else if (ngi->os_priv >= NP_SERVOPER) {
            strbcpy(services_opers[services_opers_count++],
                    ngi_mainnick(ngi));
        }
    }
    SAFE(write_int8(0, f));

    SAFE(write_int32(LOCAL_VERSION, f));
    for (ngi = nickgroup_table->first(); ngi; ngi = nickgroup_table->next()) {
        if (ngi->id == 0) {
            module_log("BUG: nickgroup with ID 0 found during write"
                       " (skipping)");
            continue;
        }
        SAFE(write_int8(1, f));
        SAFE(write_int32(ngi->id, f));
        SAFE(write_string(ngi->last_email, f));
        SAFE(write_int16(ngi->memos.memos_count, f));
        ARRAY_FOREACH (i, ngi->memos.memos) {
            SAFE(write_time(ngi->memos.memos[i].firstread, f));
            SAFE(write_string(ngi->memos.memos[i].channel, f));
        }
        SAFE(write_string(ngi->pass.cipher, f));
    }
    SAFE(write_int8(0, f));

    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", NickDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", NickDBName, strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/********************** ChanServ database handling ***********************/
/*************************************************************************/

/* Helper functions to convert between old and new channel levels. */

static inline int16 convert_old_level(int16 old)
{
    if (old < 0)
        return -convert_old_level(-old);/* avoid negative division */
    else if (old <= 25)
        return old*10;                  /*    0..  25 ->   0..250 (10x) */
    else if (old <= 50)
        return 200 + old*2;             /*   25..  50 -> 250..300 ( 2x) */
    else if (old <= 100)
        return 280 + old*2/5;           /*   50.. 100 -> 300..320 ( 0.4x) */
    else if (old <= 1000)
        return 300 + old/5;             /*  100..1000 -> 320..500 ( 0.2x) */
    else if (old <= 2000)
        return 400 + old/10;            /* 1000..2000 -> 500..600 ( 0.1x) */
    else
        return 500 + old/20;            /* 2000..9999 -> 600..999 ( 0.05x) */
}

static inline int16 convert_new_level(int16 new)
{
    if (new < 0)
        return -convert_new_level(-new);/* avoid negative division */
    else if (new <= 250)
        return new/10;                  /*   0..250 ->    0..  25 */
    else if (new <= 300)
        return new/2 - 100;             /* 250..300 ->   25..  50 */
    else if (new <= 320)
        return new*5/2 - 700;           /* 300..320 ->   50.. 100 */
    else if (new <= 500)
        return new*5 - 1500;            /* 320..500 ->  100..1000 */
    else if (new <= 600)
        return new*10 - 4000;           /* 500..600 -> 1000..2000 */
    else
        return new*20 - 10000;          /* 600..999 -> 2000..9980 */
}

/*************************************************************************/

static ChannelInfo *load_one_channel(DBTable *table, dbFILE *f, int32 ver)
{
    ChannelInfo *ci;
    NickInfo *ni;
    MemoInfo tmpmi;
    int16 tmp16, lev;
    int32 tmp32;
    int n_levels;
    char *s;
    int i;

    ci = table->newrec();
    SAFE(read_buffer(ci->name, f));
    module_log_debug(2, "loading channel %s", ci->name);
    SAFE(read_string(&s, f));
    if (s) {
        ni = get_nickinfo(s);
        if (ni) {
            ci->founder = ni->nickgroup;
            put_nickinfo(ni);
        }
        free(s);
    }
    if (ver >= 7) {
        SAFE(read_string(&s, f));
        if (s) {
            ni = get_nickinfo(s);
            if (ni) {
                ci->successor = ni->nickgroup;
                put_nickinfo(ni);
            }
            free(s);
        }
        /* Founder could be successor, which is bad, in vers 7,8 */
        /* We can also have the case where two linked nicks were founder
         * and successor, which would give them the same group ID in this
         * version--remove the successor in this case as well */
        if (ci->founder == ci->successor)
            ci->successor = 0;
    }
    init_password(&ci->founderpass);
    encrypt_password("a", 1, &ci->founderpass);  /* get encryption type */
    SAFE(read_buffer(ci->founderpass.password, f));
    SAFE(read_string(&ci->desc, f));
    if (!ci->desc)
        ci->desc = sstrdup("");
    SAFE(read_string(&ci->url, f));
    SAFE(read_string(&ci->email, f));
    SAFE(read_int32(&tmp32, f));
    ci->time_registered = tmp32;
    SAFE(read_int32(&tmp32, f));
    ci->last_used = tmp32;
    SAFE(read_string(&ci->last_topic, f));
    SAFE(read_buffer(ci->last_topic_setter, f));
    SAFE(read_int32(&tmp32, f));
    ci->last_topic_time = tmp32;
    SAFE(read_int32(&ci->flags, f));
    ci->flags &= CF_ALLFLAGS;  /* clear any invalid flags */
    if (ver >= 9) {
        void *tmpptr;
        SAFE(read_ptr(&tmpptr, f));
        if (tmpptr) {
            SAFE(read_buffer(ci->suspend_who, f));
            SAFE(read_string(&ci->suspend_reason, f));
            SAFE(read_int32(&tmp32, f));
            ci->suspend_time = tmp32;
            SAFE(read_int32(&tmp32, f));
            ci->suspend_expires = tmp32;
            ci->flags |= CF_SUSPENDED;
        }
    }
    SAFE(read_int16(&tmp16, f));
    n_levels = tmp16;
    reset_levels(ci);
    for (i = 0; i < n_levels; i++) {
        SAFE(read_int16(&lev, f));
        if (i < CA_SIZE)
            ci->levels[i] = convert_old_level(lev);
    }

    SAFE(read_int16(&ci->access_count, f));
    if (ci->access_count) {
        ci->access = scalloc(ci->access_count, sizeof(ChanAccess));
        ARRAY_FOREACH (i, ci->access) {
            ci->access[i].channel = ci;
            SAFE(read_int16(&tmp16, f));  /* in_use */
            if (tmp16) {
                SAFE(read_int16(&lev, f));
                ci->access[i].level = convert_old_level(lev);
                SAFE(read_string(&s, f));
                if (s) {
                    ni = get_nickinfo(s);
                    if (ni) {
                        ci->access[i].nickgroup = ni->nickgroup;
                        put_nickinfo(ni);
                    }
                    free(s);
                }
            }
        }
    } else {
        ci->access = NULL;
    }

    SAFE(read_int16(&ci->akick_count, f));
    if (ci->akick_count) {
        ci->akick = scalloc(ci->akick_count, sizeof(AutoKick));
        ARRAY_FOREACH (i, ci->akick) {
            ci->akick[i].channel = ci;
            SAFE(read_int16(&tmp16, f));  /* in_use */
            if (tmp16) {
                SAFE(read_int16(&tmp16, f));  /* is_nick */
                SAFE(read_string(&s, f));
                if (tmp16) {
                    ci->akick[i].mask = smalloc(strlen(s)+5);
                    sprintf(ci->akick[i].mask, "%s!*@*", s);
                    free(s);
                } else {
                    ci->akick[i].mask = s;
                }
                SAFE(read_string(&ci->akick[i].reason, f));
                if (ver >= 8)
                    SAFE(read_buffer(ci->akick[i].who, f));
                else
                    *ci->akick[i].who = 0;
                time(&ci->akick[i].set);
                ci->akick[i].lastused = 0;
            } /* if (in_use) */
        } /* for (i = 0..ci->akick_count-1) */
    } else {
        ci->akick = NULL;
    }

    if (ver < 10) {
        SAFE(read_int16(&tmp16, f));
        ci->mlock.on = tmp16;
        SAFE(read_int16(&tmp16, f));
        ci->mlock.off = tmp16;
    } else {
        SAFE(read_int32(&ci->mlock.on, f));
        SAFE(read_int32(&ci->mlock.off, f));
    }
    SAFE(read_int32(&ci->mlock.limit, f));
    SAFE(read_string(&ci->mlock.key, f));
    ci->mlock.on &= ~chanmode_reg;  /* check_modes() takes care of this */

    SAFE(read_int16(&tmpmi.memos_count, f));
    SAFE(read_int16(&tmpmi.memomax, f));
    if (tmpmi.memos_count) {
        for (i = 0; i < tmpmi.memos_count; i++) {
            Memo tmpmemo;
            SAFE(read_uint32(&tmpmemo.number, f));
            SAFE(read_int16(&tmpmemo.flags, f));
            SAFE(read_int32(&tmp32, f));
            tmpmemo.time = tmp32;
            SAFE(read_buffer(tmpmemo.sender, f));
            SAFE(read_string(&tmpmemo.text, f));
            free(tmpmemo.text);
        }
    }

    SAFE(read_string(&ci->entry_message, f));

    ci->c = NULL;

    return ci;

  fail:
    module_log("Read error on %s", f->filename);
    return NULL;
}

/*************************************************************************/

/* Load extension data for a channel. */

static int load_one_channel_ext(DBTable *table, dbFILE *f, int32 ver)
{
    char *name;
    ChannelInfo *ci = NULL;
    ChannelInfo dummy_ci;  /* for nonexistent channels */
    int i;
    int16 count, tmp16;

    SAFE(read_string(&name, f));
    if (!name)
        goto fail;
    module_log_debug(2, "loading channel extension %s", name);
    if (!(ci = get_channelinfo(name))) {
        module_log("Extension data found for nonexistent channel `%s'", name);
        ci = &dummy_ci;
        memset(ci, 0, sizeof(*ci));
    }
    free(name);
    name = NULL;
    SAFE(read_int16(&tmp16, f));  /* was memomax */
    if (ver >= 14) {
        if (ver >= 23) {
            SAFE(read_int16(&count, f));
            if (count != ci->akick_count && ci != &dummy_ci) {
                module_log("warning: autokick mismatch in extension data"
                           " for channel %s (corrupt database?): expected"
                           " %d, got %d", ci->name, ci->akick_count, count);
            }
        } else {
            count = ci->akick_count;
        }
        for (i = 0; i < count; i++) {
            if (i < ci->akick_count) {
                SAFE(read_time(&ci->akick[i].set, f));
                SAFE(read_time(&ci->akick[i].lastused, f));
            } else {
                time_t t;
                SAFE(read_time(&t, f));
                SAFE(read_time(&t, f));
            }
        }
    }
    if (ver >= 16) {
        SAFE(read_string(&ci->mlock.link, f));
        SAFE(read_string(&ci->mlock.flood, f));
    }
    if (ver >= 25)
        SAFE(read_int32(&ci->mlock.joindelay, f));
    if (ver >= 27) {
        SAFE(read_int32(&ci->mlock.joinrate1, f));
        SAFE(read_int32(&ci->mlock.joinrate2, f));
    }
    if (ver >= 20) {
        int16 lev;
        SAFE(read_int16(&count, f));
        if (count) {
            reset_levels(ci);
            for (i = 0; i < count; i++) {
                SAFE(read_int16(&lev, f));
                if (i < CA_SIZE)
                    ci->levels[i] = lev;
            }
            if (ver == 20) {
                if (ci->levels[CA_AUTODEOP] == -10)
                    ci->levels[CA_AUTODEOP] = -1;
                if (ci->levels[CA_NOJOIN] == -20)
                    ci->levels[CA_NOJOIN] = -100;
            }
        }
        SAFE(read_int16(&count, f));
        if (count != ci->access_count && ci != &dummy_ci) {
            module_log("warning: access count mismatch in extension data"
                       " for channel %s (corrupt database?): expected %d,"
                       " got %d", ci->name, ci->access_count, count);
        }
        for (i = 0; i < count; i++) {
            SAFE(read_int16(&lev, f));
            if (i < ci->access_count)
                ci->access[i].level = lev;
        }
    }
    if (ci == &dummy_ci) {
        free(ci->mlock.link);
        free(ci->mlock.flood);
    } else {
        put_channelinfo(ci);
    }
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    if (ci != &dummy_ci)
        put_channelinfo(ci);
    return 0;
}

/*************************************************************************/

/* Load version 5.1 extension data for a channel. */

static int load_one_channel_ext51(DBTable *table, dbFILE *f, int32 ver)
{
    char *name;
    ChannelInfo *ci = NULL;
    ChannelInfo dummy_ci;  /* for nonexistent channels */
    char *s;

    SAFE(read_string(&name, f));
    if (!name)
        goto fail;
    module_log_debug(2, "loading channel extension (5.1) %s", name);
    if (!(ci = get_channelinfo(name))) {
        module_log("5.1 extension data found for nonexistent channel `%s'",
                   name);
        ci = &dummy_ci;
        memset(ci, 0, sizeof(*ci));
        init_password(&ci->founderpass);
    }
    free(name);
    name = NULL;
    SAFE(read_string(&s, f));
    if (s)
        ci->mlock.on = mode_string_to_flags(s, MODE_CHANNEL|MODE_NOERROR);
    free(s);
    SAFE(read_string(&s, f));
    if (s)
        ci->mlock.off = mode_string_to_flags(s, MODE_CHANNEL|MODE_NOERROR);
    free(s);
    free((char *)ci->founderpass.cipher);
    SAFE(read_string((char **)&ci->founderpass.cipher, f));
    if (ci == &dummy_ci) {
        clear_password(&ci->founderpass);
    } else {
        put_channelinfo(ci);
    }
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    if (ci != &dummy_ci)
        put_channelinfo(ci);
    return 0;
}

/*************************************************************************/

static int load_chan_table(DBTable *table)
{
    dbFILE *f;
    int32 ver;
    int i, c;
    ChannelInfo *ci;
    int failed = 0;

    /* Open database. */
    if (!(f = my_open_db_r(ChanDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;

    /* Load original data. */
    for (i = 0; i < 256 && !failed; i++) {
        while ((c = getc_db(f)) != 0) {
            if (c != 1) {
                module_log("Invalid format in %s", ChanDBName);
                failed = 1;
                break;
            }
            ci = load_one_channel(table, f, ver);
            if (ci) {
                if (strcmp(ci->name, "#") == 0) {
                    module_log("Deleting unsupported channel \"#\"");
                    table->freerec(ci);
                } else if (!(ci->flags & CF_VERBOTEN) && !ci->founder) {
                    /* Delete non-forbidden channels with no founder.  These
                     * can crop up if the nick and channel databases get out
                     * of sync and the founder's nick has disappeared.  Note
                     * that we ignore the successor here, but since this
                     * shouldn't happen normally anyway, no big deal.
                     */
                    module_log("load channel database: Deleting founderless"
                               " channel %s", ci->name);
                    table->freerec(ci);
                } else {
                    NickGroupInfo *ngi = get_nickgroupinfo(ci->founder);
                    if (ngi) {
                        ARRAY_EXTEND(ngi->channels);
                        strbcpy(ngi->channels[ngi->channels_count-1],ci->name);
                        put_nickgroupinfo(ngi);
                    }
                    table->insert(ci);
                }
            } else {
                failed = 1;
                break;
            }
        }
    }

    /* Load extension data if present. */
    if (!failed && read_int32(&ver, f) == 0) {
        if (ver <= FILE_VERSION || ver > LOCAL_VERSION_50) {
            module_log("Invalid extension data version in %s", ChanDBName);
            failed = 1;
        }
        while (!failed && (c = getc_db(f)) != 0) {
            if (c != 1) {
                module_log("Invalid format in %s extension data", ChanDBName);
                failed = 1;
            } else {
                failed = !load_one_channel_ext(table, f, ver);
            }
        }
    }
    if (ver < 26) {
        /* Reset all AUTODEOP/NOJOIN levels to the defaults (version 4.x
         * databases may have them set to non-default levels, and version 5
         * doesn't allow them to be changed) */
        for (ci = table->first(); ci; ci = table->next()) {
            if (ci->levels) {
                ci->levels[CA_AUTODEOP] = -1;
                ci->levels[CA_NOJOIN] = -100;
            }
        }
    }
    /* Clean out all empty (mask==NULL) autokick entries */
    for (ci = table->first(); ci; ci = table->next()) {
        int i;
        ARRAY_FOREACH (i, ci->akick) {
            if (!ci->akick[i].mask) {
                ARRAY_REMOVE(ci->akick, i);
                i--;
            }
        }
    }

    if (!failed && read_int32(&ver, f) == 0) {
        if (ver < FIRST_VERSION_51 || ver > LOCAL_VERSION) {
            module_log("Invalid 5.1 extension data version in %s", ChanDBName);
            failed = 1;
        }
        while (!failed && (c = getc_db(f)) != 0) {
            if (c != 1) {
                module_log("Invalid format in %s 5.1 extension data",
                           ChanDBName);
                failed = 1;
            } else {
                failed = !load_one_channel_ext51(table, f, ver);
            }
        }
    }

    /* Close database and return. */
    close_db(f);
    return !failed || forceload;
}

/*************************************************************************/

static int save_chan_table(DBTable *table)
{
    dbFILE *f;
    int i;
    ChannelInfo *ci;
    NickGroupInfo *ngi;
    static time_t lastwarn = 0;

    if (!(f = open_db(ChanDBName, "w", 11)))
        return 0;

    for (ci = table->first(); ci; ci = table->next()) {
        SAFE(write_int8(1, f));
        SAFE(write_buffer(ci->name, f));
        if (ci->founder && (ngi = get_ngi_id(ci->founder))) {
            SAFE(write_string(ngi_mainnick(ngi), f));
            put_nickgroupinfo(ngi);
        } else {
            SAFE(write_string(NULL, f));
        }
        if (ci->successor && (ngi = get_ngi_id(ci->successor))) {
            SAFE(write_string(ngi_mainnick(ngi), f));
            put_nickgroupinfo(ngi);
        } else {
            SAFE(write_string(NULL, f));
        }
        SAFE(write_buffer(ci->founderpass.password, f));
        SAFE(write_string(ci->desc, f));
        SAFE(write_string(ci->url, f));
        SAFE(write_string(ci->email, f));
        SAFE(write_int32(ci->time_registered, f));
        SAFE(write_int32(ci->last_used, f));
        SAFE(write_string(ci->last_topic, f));
        SAFE(write_buffer(ci->last_topic_setter, f));
        SAFE(write_int32(ci->last_topic_time, f));
        SAFE(write_int32(ci->flags, f));
        SAFE(write_ptr(ci->flags & CF_SUSPENDED ? (void *)1 : NULL, f));
        if (ci->flags & CF_SUSPENDED) {
            SAFE(write_buffer(ci->suspend_who, f));
            SAFE(write_string(ci->suspend_reason, f));
            SAFE(write_int32(ci->suspend_time, f));
            SAFE(write_int32(ci->suspend_expires, f));
        }

        if (ci->levels) {
            SAFE(write_int16(CA_SIZE, f));
            for (i = 0; i < CA_SIZE; i++)
                SAFE(write_int16(convert_new_level(ci->levels[i]), f));
        } else {
            SAFE(write_int16(CA_SIZE_4_5, f));
            for (i = 0; i < CA_SIZE_4_5; i++) {
                if (i == CA_NOJOIN && (ci->flags & CF_RESTRICTED))
                    SAFE(write_int16(0, f));
                else
                    SAFE(write_int16(def_levels_4_5[i], f));
            }
        }

        SAFE(write_int16(ci->access_count, f));
        ARRAY_FOREACH (i, ci->access) {
            if (ci->access[i].nickgroup)
                ngi = get_ngi_id(ci->access[i].nickgroup);
            else
                ngi = NULL;
            SAFE(write_int16(ngi != NULL, f));
            if (ngi) {
                SAFE(write_int16(convert_new_level(ci->access[i].level), f));
                SAFE(write_string(ngi_mainnick(ngi), f));
                put_nickgroupinfo(ngi);
            }
        }

        SAFE(write_int16(ci->akick_count, f));
        ARRAY_FOREACH (i, ci->akick) {
            SAFE(write_int16((ci->akick[i].mask != NULL), f));  /* in_use */
            if (ci->akick[i].mask) {
                SAFE(write_int16(0, f));  /* is_nick */
                SAFE(write_string(ci->akick[i].mask, f));
                SAFE(write_string(ci->akick[i].reason, f));
                SAFE(write_buffer(ci->akick[i].who, f));
            }
        }

        SAFE(write_int32(ci->mlock.on, f));
        SAFE(write_int32(ci->mlock.off, f));
        SAFE(write_int32(ci->mlock.limit, f));
        SAFE(write_string(ci->mlock.key, f));

        SAFE(write_int16(0, f));  /* memos_count */
        SAFE(write_int16(MSMaxMemos, f));  /* memomax */

        SAFE(write_string(ci->entry_message, f));

    } /* for (ci) */

    {
        /* This is an UGLY HACK but it simplifies loading. */
        static char buf[256];  /* initialized to zero */
        SAFE(write_buffer(buf, f));
    }

    SAFE(write_int32(LOCAL_VERSION_50, f));
    for (ci = table->first(); ci; ci = table->next()) {
        SAFE(write_int8(1, f));
        SAFE(write_string(ci->name, f));
        SAFE(write_int16(MEMOMAX_DEFAULT, f));  /* for 5.0's sake */
        SAFE(write_int16(ci->akick_count, f));
        ARRAY_FOREACH (i, ci->akick) {
            SAFE(write_time(ci->akick[i].set, f));
            SAFE(write_time(ci->akick[i].lastused, f));
        }
        SAFE(write_string(ci->mlock.link, f));
        SAFE(write_string(ci->mlock.flood, f));
        SAFE(write_int32(ci->mlock.joindelay, f));
        SAFE(write_int32(ci->mlock.joinrate1, f));
        SAFE(write_int32(ci->mlock.joinrate2, f));
        if (ci->levels) {
            SAFE(write_int16(CA_SIZE, f));
            for (i = 0; i < CA_SIZE; i++)
                SAFE(write_int16(ci->levels[i], f));
        } else {
            SAFE(write_int16(0, f));
        }
        SAFE(write_int16(ci->access_count, f));
        ARRAY_FOREACH (i, ci->access)
            SAFE(write_int16(ci->access[i].level, f));
    }
    SAFE(write_int8(0, f));

    SAFE(write_int32(LOCAL_VERSION, f));
    for (ci = table->first(); ci; ci = table->next()) {
        SAFE(write_string(mode_flags_to_string(ci->mlock.on,MODE_CHANNEL),f));
        SAFE(write_string(mode_flags_to_string(ci->mlock.off,MODE_CHANNEL),f));
        SAFE(write_string(ci->founderpass.cipher, f));
    }
    SAFE(write_int8(0, f));

    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", ChanDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", ChanDBName, strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/********************** OperServ database handling ***********************/
/*************************************************************************/

static int load_oper_table(DBTable *table)
{
    dbFILE *f;
    int32 ver;
    int16 i, n;
    DBField *field;
    void *record;  /* pointer to OperServ's data structure */
    int8 no_supass = 1;
    Password *supass = NULL;

    if (!(record = table->first())) {
        module_log("BUG: record missing from OperServ table!");
        return 0;
    }
    if (table->next()) {
        module_log("BUG: too many records in OperServ table!");
        return 0;
    }

    if (!(f = my_open_db_r(OperDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;

    services_admins_count = services_opers_count = 0;
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        char *s;
        SAFE(read_string(&s, f));
        if (s && i < MAX_SERVADMINS)
            strbcpy(services_admins[services_admins_count++], s);
        free(s);
    }
    SAFE(read_int16(&n, f));
    for (i = 0; i < n; i++) {
        char *s;
        SAFE(read_string(&s, f));
        if (s && i < MAX_SERVOPERS)
            strbcpy(services_opers[services_opers_count++], s);
        free(s);
    }
    if (ver >= 7) {
        int32 tmp32;
        SAFE(read_int32(&tmp32, f));
        for (field = table->fields; field->name; field++) {
            if (strcmp(field->name, "maxusercnt") == 0) {
                if (field->type != DBTYPE_INT32) {
                    module_log("BUG: oper.maxusercnt type is not INT32!");
                    goto fail;
                }
                *((int32 *)DB_FIELDPTR(record,field)) = tmp32;
                break;
            }
        }
        SAFE(read_int32(&tmp32, f));
        for (field = table->fields; field->name; field++) {
            if (strcmp(field->name, "maxusertime") == 0) {
                if (field->type != DBTYPE_TIME) {
                    module_log("BUG: oper.maxusertime type is not TIME!");
                    goto fail;
                }
                *((time_t *)DB_FIELDPTR(record,field)) = tmp32;
                break;
            }
        }
    }
    if (ver >= 9) {
        SAFE(read_int8(&no_supass, f));
        for (field = table->fields; field->name; field++) {
            if (strcmp(field->name, "no_supass") == 0) {
                if (field->type != DBTYPE_INT8) {
                    module_log("BUG: oper.no_supass type is not INT8!");
                    goto fail;
                }
                *((int8 *)DB_FIELDPTR(record,field)) = no_supass;
                break;
            }
        }
        if (!no_supass) {
            Password tmppass;
            init_password(&tmppass);
            encrypt_password("a", 1, &tmppass);
            SAFE(read_buffer(tmppass.password, f));
            for (field = table->fields; field->name; field++) {
                if (strcmp(field->name, "supass") == 0) {
                    if (field->type != DBTYPE_PASSWORD) {
                        module_log("BUG: oper.supass type is not PASSWORD!");
                        goto fail;
                    }
                    supass = (Password *)DB_FIELDPTR(record,field);
                    init_password(supass);
                    copy_password(supass, &tmppass);
                    break;
                }
            }
        }
    }

    if (read_int32(&ver, f) == 0) {
        if (ver < FIRST_VERSION_51 || ver > LOCAL_VERSION) {
            module_log("Invalid 5.1 extension data version in %s", OperDBName);
            close_db(f);
            return 0;
        }
        SAFE(read_string((char **)&supass->cipher, f));
    }

    close_db(f);
    return 1;

  fail:
    close_db(f);
    module_log("Read error on %s", OperDBName);
    return 0;
}

/*************************************************************************/

static int save_oper_table(DBTable *table)
{
    dbFILE *f;
    int16 i;
    void *record;
    DBField *field;
    static time_t lastwarn = 0;
    int8 no_supass = 1;
    Password *supass = NULL;

    if (!(record = table->first())) {
        module_log("BUG: record missing from OperServ table!");
        return 0;
    }
    if (table->next()) {
        module_log("BUG: too many records in OperServ table!");
        return 0;
    }

    if (!(f = open_db(OperDBName, "w", 11)))
        return 0;

    SAFE(write_int16(services_admins_count, f));
    ARRAY_FOREACH (i, services_admins)
        SAFE(write_string(services_admins[i], f));

    SAFE(write_int16(services_opers_count, f));
    ARRAY_FOREACH (i, services_opers)
        SAFE(write_string(services_opers[i], f));

    for (field = table->fields; field->name; field++) {
        if (strcmp(field->name, "maxusercnt") == 0) {
            if (field->type != DBTYPE_INT32) {
                module_log("BUG: oper.maxusercnt type is not INT32!");
                goto fail;
            }
            SAFE(write_int32(*((int32 *)DB_FIELDPTR(record,field)), f));
            break;
        }
    }
    for (field = table->fields; field->name; field++) {
        if (strcmp(field->name, "maxusertime") == 0) {
            if (field->type != DBTYPE_TIME) {
                module_log("BUG: oper.maxusertime type is not TIME!");
                goto fail;
            }
            SAFE(write_int32((int32) *((time_t *)DB_FIELDPTR(record,field)),
                             f));
            break;
        }
    }
    for (field = table->fields; field->name; field++) {
        if (strcmp(field->name, "no_supass") == 0) {
            if (field->type != DBTYPE_INT8) {
                module_log("BUG: oper.no_supass type is not INT8!");
                goto fail;
            }
            no_supass = *((int8 *)DB_FIELDPTR(record,field));
            SAFE(write_int8(no_supass, f));
            break;
        }
    }
    if (!no_supass) {
        for (field = table->fields; field->name; field++) {
            if (strcmp(field->name, "supass") == 0) {
                if (field->type != DBTYPE_PASSWORD) {
                    module_log("BUG: oper.supass type is not PASSWORD!");
                    goto fail;
                }
                supass = (Password *)DB_FIELDPTR(record,field);
                SAFE(write_buffer(supass->password, f));
                break;
            }
        }
    }

    SAFE(write_int32(LOCAL_VERSION, f));
    if (!no_supass)
        SAFE(write_string(supass->cipher, f));

    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", OperDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", OperDBName, strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/************************ News database handling *************************/
/*************************************************************************/

static int load_news_table(DBTable *table)
{
    dbFILE *f;
    int32 ver;
    int i;
    int16 count;

    if (!(f = my_open_db_r(NewsDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;

#if CLEAN_COMPILE
    count = 0;
#endif
    SAFE(read_int16(&count, f));
    for (i = 0; i < count; i++) {
        int32 tmp32;
        NewsItem *news = table->newrec();
        SAFE(read_int16(&news->type, f));
        SAFE(read_int32(&news->num, f));
        SAFE(read_string(&news->text, f));
        SAFE(read_buffer(news->who, f));
        SAFE(read_int32(&tmp32, f));
        news->time = tmp32;
        table->insert(news);
    }

    close_db(f);
    return 1;

  fail:
    close_db(f);
    module_log("Read error on %s", NewsDBName);
    return 0;
}

/*************************************************************************/

static int save_news_table(DBTable *table)
{
    dbFILE *f;
    int count, i;
    NewsItem *news;
    static time_t lastwarn = 0;

    if (!(f = open_db(NewsDBName, "w", 11)))
        return 0;

    count = 0;
    for (news = table->first(); news; news = table->next())
        count++;
    if (count > 32767)
        count = 32767;  // avoid overflow
    write_int16((int16)count, f);
    i = 0;
    for (news = table->first(), i = 0; i < count; news = table->next(), i++) {
        if (!news) {
            module_log("BUG: news item count changed while saving!");
            wallops(NULL, "Error saving %s!  Please check log file.",
                    NewsDBName);
            restore_db(f);
            return 0;
        }
        SAFE(write_int16(news->type, f));
        SAFE(write_int32(news->num, f));
        SAFE(write_string(news->text, f));
        SAFE(write_buffer(news->who, f));
        SAFE(write_int32(news->time, f));
    }

    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", NewsDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", NewsDBName, strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/********************** Autokill database handling ***********************/
/*************************************************************************/

static int load_akill_table(DBTable *akill_table, DBTable *exclude_table)
{
    dbFILE *f;
    int32 ver;

    if (!(f = my_open_db_r(AutokillDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;
    if (!read_maskdata(akill_table, MD_AKILL, AutokillDBName, f))
        return 0;
    if (getc_db(f) == 1) {
        if (!read_maskdata(exclude_table, MD_EXCLUDE, AutokillDBName, f))
            return 0;
    }
    close_db(f);
    return 1;
}

/*************************************************************************/

static int save_akill_table(DBTable *akill_table, DBTable *exclude_table)
{
    dbFILE *f;
    static time_t lastwarn = 0;

    if (!(f = open_db(AutokillDBName, "w", 11)))
        return 0;
    if (!write_maskdata(akill_table, MD_AKILL, AutokillDBName, f))
        return 0;
    SAFE(write_int8(1, f));
    if (!write_maskdata(exclude_table, MD_EXCLUDE, AutokillDBName, f))
        return 0;
    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", AutokillDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", AutokillDBName,
                strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/********************** Exception database handling **********************/
/*************************************************************************/

static int load_exception_table(DBTable *table)
{
    dbFILE *f;
    int32 ver;

    if (!(f = my_open_db_r(ExceptionDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;
    if (!read_maskdata(table, MD_EXCEPTION, ExceptionDBName, f))
        return 0;
    close_db(f);
    return 1;
}

/*************************************************************************/

static int save_exception_table(DBTable *table)
{
    dbFILE *f;
    static time_t lastwarn = 0;

    if (!(f = open_db(ExceptionDBName, "w", 11)))
        return 0;
    if (!write_maskdata(table, MD_EXCEPTION, ExceptionDBName, f))
        return 0;
    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", ExceptionDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", ExceptionDBName,
                strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/*********************** S-line database handling ************************/
/*************************************************************************/

static int load_sline_table(DBTable *sgline_table, DBTable *sqline_table,
                            DBTable *szline_table)
{
    dbFILE *f;
    int32 ver;

    if (!(f = my_open_db_r(SlineDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;
    if (!read_maskdata(sgline_table, MD_SGLINE, SlineDBName, f))
        return 0;
    if (!read_maskdata(sqline_table, MD_SQLINE, SlineDBName, f))
        return 0;
    if (!read_maskdata(szline_table, MD_SZLINE, SlineDBName, f))
        return 0;
    close_db(f);
    return 1;
}

/*************************************************************************/

static int save_sline_table(DBTable *sgline_table, DBTable *sqline_table,
                            DBTable *szline_table)
{
    dbFILE *f;
    static time_t lastwarn = 0;

    if (!(f = open_db(SlineDBName, "w", 11)))
        return 0;
    if (!write_maskdata(sgline_table, MD_SGLINE, SlineDBName, f))
        return 0;
    if (!write_maskdata(sqline_table, MD_SQLINE, SlineDBName, f))
        return 0;
    if (!write_maskdata(szline_table, MD_SZLINE, SlineDBName, f))
        return 0;
    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", SlineDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", SlineDBName, strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/********************** StatServ database handling ***********************/
/*************************************************************************/

static ServerStats *load_one_serverstats(DBTable *table, dbFILE *f)
{
    ServerStats *ss;
    int32 tmp32;

    ss = table->newrec();
    SAFE(read_string(&ss->name, f));
    SAFE(read_int32(&tmp32, f));
    ss->t_join = tmp32;
    SAFE(read_int32(&tmp32, f));  /* t_quit */
    /* Avoid join>=quit staying true on load (which would indicate that the
     * server is online even before any server connections are processed) */
    ss->t_quit = time(NULL)-1;
    if (ss->t_join >= ss->t_quit)
        ss->t_join = ss->t_quit-1;
    SAFE(read_string(&ss->quit_message, f));
    return ss;

  fail:
    module_log("Read error on %s", f->filename);
    return NULL;
}

/*************************************************************************/

static int load_one_serverstats_ext(DBTable *table, dbFILE *f, int32 ver)
{
    ServerStats *ss = NULL;
    char *servername;

    SAFE(read_string(&servername, f));
    if (!servername)
        goto fail;
    ss = get_serverstats(servername);
    if (!ss) {
        module_log("Extension data found for nonexistent server `%s'",
                   servername);
        free(servername);
        return 0;
    }
    free(servername);
    SAFE(read_time(&ss->t_join, f));
    put_serverstats(ss);
    return 1;

  fail:
    module_log("Read error on %s", f->filename);
    put_serverstats(ss);
    return 0;
}

/*************************************************************************/

static int load_stat_servers_table(DBTable *table)
{
    dbFILE *f;
    int32 ver, i, nservers;
    int16 tmp16;
    int failed = 0;
    ServerStats *ss;

    /* Open database. */
    if (!(f = my_open_db_r(StatDBName, &ver)))
        return 1;
    else if (f == OPENDB_ERROR)
        return 0;

    /* Load original data. */
    SAFE(read_int16(&tmp16, f));
    nservers = tmp16;
    for (i = 0; i < nservers && !failed; i++) {
        ss = load_one_serverstats(table, f);
        if (ss)
            table->insert(ss);
        else
            failed = 1;
    }

    /* Load extension data if present. */
    if (!failed && read_int32(&ver, f) == 0) {
        int32 moreservers;
        if (ver <= FILE_VERSION || ver > LOCAL_VERSION_50) {
            module_log("Invalid extension data version in %s", StatDBName);
        } else {
            SAFE(read_int32(&moreservers, f));
            for (i = 0; i < moreservers && !failed; i++) {
                ss = load_one_serverstats(table, f);
                if (ss)
                    table->insert(ss);
                else
                    failed = 1;
            }
            nservers += moreservers;
            for (i = 0; i < nservers && !failed; i++)
                failed = !load_one_serverstats_ext(table, f, ver);
        }
    }

    /* Close database and return. */
    close_db(f);
    return !failed || forceload;

  fail:
    close_db(f);
    module_log("Read error on %s", StatDBName);
    return 0;
}

/*************************************************************************/

static int save_stat_servers_table(DBTable *table)
{
    dbFILE *f;
    int32 count, realcount, i;
    ServerStats *ss;
    static time_t lastwarn = 0;

    if (!(f = open_db(StatDBName, "w", 11)))
        return 0;

    realcount = 0;
    for (ss = table->first(); ss; ss = table->next())
        realcount++;
    if (realcount > 32767)  /* Well, you never know... */
        count = 32767;
    else
        count = realcount;
    SAFE(write_int16((int16)count, f));

    for (ss = table->first(), i = 0; i < count; ss = table->next(), i++) {
        if (!ss) {
            module_log("BUG: save_stat_servers_table(): ss NULL but i <"
                       " count!");
            wallops(NULL, "Error saving %s!  Please check log file.",
                    StatDBName);
            restore_db(f);
            return 0;
        }
        SAFE(write_string(ss->name, f));
        SAFE(write_int32(ss->t_join, f));
        SAFE(write_int32(ss->t_quit, f));
        SAFE(write_string(ss->quit_message, f));
    }

    SAFE(write_int32(LOCAL_VERSION_50, f));
    if (realcount > count) {
        SAFE(write_int32(realcount-count, f));
        for (; i < realcount; ss = table->next(), i++) {
            if (!ss) {
                module_log("BUG: save_stat_servers_table(): ss NULL but i <"
                           " realcount!");
                wallops(NULL, "Error saving %s!  Please check log file.",
                        StatDBName);
                restore_db(f);
                return 0;
            }
            SAFE(write_string(ss->name, f));
            SAFE(write_int32(ss->t_join, f));
            SAFE(write_int32(ss->t_quit, f));
            SAFE(write_string(ss->quit_message, f));
        }
    } else {
        SAFE(write_int32(0, f));
    }
    for (ss = table->first(); ss; ss = table->next()) {
        SAFE(write_string(ss->name, f));
        SAFE(write_time(ss->t_join, f));
    }

    SAFE(close_db(f));
    return 1;

  fail:
    restore_db(f);
    module_log_perror("Write error on %s", StatDBName);
    if (time(NULL) - lastwarn > WarningTimeout) {
        wallops(NULL, "Write error on %s: %s", StatDBName, strerror(errno));
        lastwarn = time(NULL);
    }
    return 0;
}

/*************************************************************************/
/*********************** Generic database handling ***********************/
/*************************************************************************/

/* Routines used for databases not part of the v4/v5.0 set */

#define INCLUDE_IN_VERSION4
#define standard_load_table load_generic_table
#define standard_save_table save_generic_table
#include "standard.c"
#undef INCLUDE_IN_VERSION4
#undef standard_load_table
#undef standard_save_table

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "AutokillDB",       { { CD_STRING, CF_DIRREQ, &AutokillDBName } } },
    { "ChanServDB",       { { CD_STRING, CF_DIRREQ, &ChanDBName } } },
    { "ExceptionDB",      { { CD_STRING, CF_DIRREQ, &ExceptionDBName } } },
    { "NewsDB",           { { CD_STRING, CF_DIRREQ, &NewsDBName } } },
    { "NickServDB",       { { CD_STRING, CF_DIRREQ, &NickDBName } } },
    { "OperServDB",       { { CD_STRING, CF_DIRREQ, &OperDBName } } },
    { "SlineDB",          { { CD_STRING, CF_DIRREQ, &SlineDBName } } },
    { "StatServDB",       { { CD_STRING, CF_DIRREQ, &StatDBName } } },
    { NULL }
};


static DBModule dbmodule_version4 = {
    .load_table = version4_load_table,
    .save_table = version4_save_table,
};

/*************************************************************************/

int init_module(void)
{
    if (!init_extsyms(MODULE_NAME)) {
        exit_module(0);
        return 0;
    }

    if (!register_dbmodule(&dbmodule_version4)) {
        module_log("Unable to register module with database core");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown)
{
    unregister_dbmodule(&dbmodule_version4);
    exit_extsyms();
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
