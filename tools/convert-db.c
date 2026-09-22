/* Convert other programs' databases to XML.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#define CONVERT_DB_MAIN
#define STANDALONE_NICKSERV
#define STANDALONE_CHANSERV
#include "convert-db.h"
#include "language.h"
#include "modules/misc/xml.h"

/*************************************************************************/

/* Data read in. */

NickGroupInfo *ngi_list;
NickInfo *ni_list;
ChannelInfo *ci_list;
NewsItem *news_list;
MaskData *md_list[256];
ServerStats *ss_list;
int32 maxusercnt;
time_t maxusertime;
Password supass;
int no_supass = 1;

/* Symbols needed for core's misc.c. */
char **RejectEmail = NULL;
int RejectEmail_count = 0;

/*************************************************************************/

/* NULL-terminated list of supported database types. */

extern DBTypeInfo ALL_DB_TYPES();
static DBTypeInfo *dbtypes[] = {
    ALL_DB_TYPES(&),
    NULL
};

/*************************************************************************/
/*************************** Exported routines ***************************/
/*************************************************************************/

/* Safe memory allocation and string duplication (from memory.c). */

void *smalloc(long size)
{
    void *ptr;
    if (!size)
        size = 1;
    ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return ptr;
}

void *scalloc(long els, long elsize)
{
    void *ptr;
    if (!(els*elsize))
        els = elsize = 1;
    ptr = calloc(els, elsize);
    if (!ptr) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return ptr;
}

void *srealloc(void *ptr, long size)
{
    if (!size)
        size = 1;
    ptr = realloc(ptr, size);
    if (!ptr) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    return ptr;
}

char *sstrdup(const char *s)
{
    int len = strlen(s)+1;
    char *d = smalloc(len);
    memcpy(d, s, len);
    return d;
}

/*************************************************************************/

/* Initialize or clear a Password structure (from encrypt.c). */

void init_password(Password *password)
{
    memset(password, 0, sizeof(*password));
    password->cipher = NULL;
}

void clear_password(Password *password)
{
    memset(password, 0, sizeof(*password));
    free((char *)password->cipher);
    password->cipher = NULL;
}

/*************************************************************************/

/* NickInfo/NickGroupInfo allocation (from modules/nickserv/util.c).  The
 * NickInfo and NickGroupInfo (if appropriate) are added into their
 * respective hash tables.  Always succeeds (if the memory allocations
 * fail, aborts the program).
 */

#include "modules/nickserv/util.c"

NickInfo *makenick(const char *nick, NickGroupInfo **nickgroup_ret)
{
    NickInfo *ni = new_nickinfo();
    strbcpy(ni->nick, nick);
    if (nickgroup_ret) {
        NickGroupInfo *ngi = new_nickgroupinfo(ni->nick);
        while (get_nickgroupinfo(ngi->id)) {
            /* We assume that eventually we'll find one that's not in use */
            ngi->id = rand() + rand();
            if (ngi->id == 0)
                ngi->id = 1;
        }
        ni->nickgroup = ngi->id;
        ARRAY_EXTEND(ngi->nicks);
        strbcpy(ngi->nicks[0], nick);
        add_nickgroupinfo(ngi);
        *nickgroup_ret = ngi;
    }
    add_nickinfo(ni);
    return ni;
}

/*************************************************************************/

/* ChannelInfo allocation.  The channel is added to the hash table. */

#include "modules/chanserv/util.c"

ChannelInfo *makechan(const char *name)
{
    ChannelInfo *ci = new_channelinfo();
    strbcpy(ci->name, name);
    add_channelinfo(ci);
    return ci;
}

/*************************************************************************/

/* Open a (Services pre-5.0 style) data file and check the version number.
 * Prints an error message and exits with 1 if either the file cannot be
 * opened or the version number is wrong.  If `version_ret' is non-NULL,
 * the version number is stored there.
 */

dbFILE *open_db_ver(const char *dir, const char *name, int32 min_version,
                    int32 max_version, int32 *version_ret)
{
    char filename[PATH_MAX+1];
    dbFILE *f;
    int32 ver;

    snprintf(filename, sizeof(filename), "%s/%s", dir, name);
    f = open_db(filename, "r", 0);
    if (!f) {
        fprintf(stderr, "Can't open %s for reading: %s\n", filename,
                strerror(errno));
        exit(1);
    }
    if (read_int32(&ver, f) < 0) {
        fprintf(stderr, "Error reading version number on %s\n", filename);
        exit(1);
    }
    if (ver < min_version || ver > max_version) {
        fprintf(stderr, "Wrong version number on %s\n", filename);
        exit(1);
    }
    if (version_ret)
        *version_ret = ver;
    return f;
}

/*************************************************************************/

/* Retrieve the NickGroupInfo structure for the given nick.  Returns NULL
 * if the nick does not exist or is forbidden (i.e. has no NickGroupInfo).
 */

NickGroupInfo *get_nickgroupinfo_by_nick(const char *nick)
{
    NickInfo *ni = get_nickinfo(nick);
    return ni ? get_nickgroupinfo(ni->nickgroup) : NULL;
}

/*************************************************************************/

/* Set the OperServ privilege level (os_priv) for the nickgroup associated
 * with `nick' to `level', if it is not already greater than `level'.  Does
 * nothing if `nick' is NULL or does not have an associated nickgroup.
 */

void set_os_priv(const char *nick, int16 level)
{
    NickGroupInfo *ngi;

    if (!nick)
        return;
    ngi = get_nickgroupinfo_by_nick(nick);
    if (ngi && ngi->os_priv < level)
        ngi->os_priv = level;
}

/*************************************************************************/

/* Return the Services 5.0 channel access level that corresponds to the
 * given pre-5.0 level.  Code taken from modules/database/version4.c
 * (convert_old_level()).
 */

int16 convert_acclev(int16 old)
{
    if (old < 0)
        return -convert_acclev(-old);   /* avoid negative division */
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

/*************************************************************************/
/*************************************************************************/

/* Replacements for database functions.  add_*() functions are defined as
 * macros in convert-db.h. */

/*************************************************************************/

static NickGroupInfo *ngi_iter;

NickGroupInfo *get_nickgroupinfo(uint32 id)
{
    NickGroupInfo *ngi;
    if (!id)
        return NULL;
    LIST_SEARCH_SCALAR(ngi_list, id, id, ngi);
    return ngi;
}

NickGroupInfo *first_nickgroupinfo(void)
{
    ngi_iter = ngi_list;
    return next_nickgroupinfo();
}

NickGroupInfo *next_nickgroupinfo(void)
{
    NickGroupInfo *retval = ngi_iter;
    if (ngi_iter)
        ngi_iter = ngi_iter->next;
    return retval;
}

/*************************************************************************/

static NickInfo *ni_iter;

NickInfo *get_nickinfo(const char *nick)
{
    NickInfo *ni;
    LIST_SEARCH(ni_list, nick, nick, stricmp, ni);
    return ni;
}

NickInfo *first_nickinfo(void)
{
    ni_iter = ni_list;
    return next_nickinfo();
}

NickInfo *next_nickinfo(void)
{
    NickInfo *retval = ni_iter;
    if (ni_iter)
        ni_iter = ni_iter->next;
    return retval;
}

/*************************************************************************/

static ChannelInfo *ci_iter;

ChannelInfo *get_channelinfo(const char *channel)
{
    ChannelInfo *ci;
    LIST_SEARCH(ci_list, name, channel, stricmp, ci);
    return ci;
}

ChannelInfo *first_channelinfo(void)
{
    ci_iter = ci_list;
    return next_channelinfo();
}

ChannelInfo *next_channelinfo(void)
{
    ChannelInfo *retval = ci_iter;
    if (ci_iter)
        ci_iter = ci_iter->next;
    return retval;
}

/*************************************************************************/

static NewsItem *news_iter;

NewsItem *first_news(void)
{
    news_iter = news_list;
    return next_news();
}

NewsItem *next_news(void)
{
    NewsItem *retval = news_iter;
    if (news_iter)
        news_iter = news_iter->next;
    return retval;
}

/*************************************************************************/

static MaskData *md_iter[256];

MaskData *first_maskdata(uint8 type)
{
    md_iter[type] = md_list[type];
    return next_maskdata(type);
}

MaskData *next_maskdata(uint8 type)
{
    MaskData *retval = md_iter[type];
    if (md_iter[type])
        md_iter[type] = md_iter[type]->next;
    return retval;
}

/*************************************************************************/

static ServerStats *ss_iter;

ServerStats *first_serverstats(void)
{
    ss_iter = ss_list;
    return next_serverstats();
}

ServerStats *next_serverstats(void)
{
    ServerStats *retval = ss_iter;
    if (ss_iter)
        ss_iter = ss_iter->next;
    return retval;
}

/*************************************************************************/

int get_operserv_data(int what, void *ptr)
{
    switch (what) {
      case OSDATA_MAXUSERCNT:
        *(int32 *)ptr = maxusercnt;
        break;
      case OSDATA_MAXUSERTIME:
        *(time_t *)ptr = maxusertime;
        break;
      case OSDATA_SUPASS:
        *(Password **)ptr = no_supass ? NULL : &supass;
        break;
      default:
        return 0;
    }
    return 1;
}

/*************************************************************************/
/*************************************************************************/

/* Perform sanity checks on the data after it's been read in. */

static void sanity_check_nicks(void);
static void sanity_check_nickgroups(void);
static void sanity_check_channels(void);
static void sanity_check_maskdata(void);

static void sanity_checks(void)
{
    sanity_check_nicks();
    sanity_check_nickgroups();
    sanity_check_channels();
    sanity_check_maskdata();
}

/*************************************************************************/

static void sanity_check_nicks(void)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *s;
    int i;

    for (ni = first_nickinfo(); ni; ni = next_nickinfo()) {

        /* Forbidden nicks should have no other data associated with them. */
        if (ni->status & NS_VERBOTEN) {
            ni->status &= ~(NS_VERBOTEN);
            ni->last_usermask = NULL;
            ni->last_realmask = NULL;
            ni->last_realname = NULL;
            ni->last_quit = NULL;
            ni->last_seen = 0;
            if (ni->nickgroup) {
                fprintf(stderr, "BUG: Forbidden nickname %s associated with"
                        " nickgroup %u!  Clearing nickgroup field.\n",
                        ni->nick, ni->nickgroup);
                ni->nickgroup = 0;
            }
            ni->id_stamp = 0;
            continue;
        }

        /* The nick isn't a forbidden nick.  First make sure it has the
         * right nickgroup. */
        if (!ni->nickgroup) {
            fprintf(stderr, "BUG: Nickname %s has no nickgroup!  Deleting.\n",
                    ni->nick);
            del_nickinfo(ni);
            continue;
        }
        ngi = get_nickgroupinfo(ni->nickgroup);
        if (!ngi) {
            fprintf(stderr, "BUG: Nickname %s points to nonexistent nickgroup"
                    " %u!  Deleting.\n", ni->nick, ni->nickgroup);
            del_nickinfo(ni);
        } else {
            /* Nicknames in the nicks[] array should match those in the
             * NickInfo structure, so use strcmp() (this keeps us from
             * having to worry about irc_stricmp() idiosyncrasies). */
            ARRAY_SEARCH_PLAIN(ngi->nicks, ni->nick, strcmp, i);
            if (i >= ngi->nicks_count) {
                fprintf(stderr, "BUG: Nickname %s points to nickgroup %u,"
                        " but the nickgroup doesn't contain the nickname!"
                        "  Deleting.\n", ni->nick, ni->nickgroup);
                del_nickinfo(ni);
            }
        }

        /* Clear unknown flags. */
        ni->status &= NS_PERMANENT;

        /* Make sure usermasks actually have non-empty user and host parts. */
        if (ni->last_usermask) {
            s = strchr(ni->last_usermask, '@');
            if (!s || s==ni->last_usermask || !s[1]) {
                fprintf(stderr, "Last usermask for nickname %s isn't a valid"
                        " user@host mask, clearing.\n", ni->nick);
                ni->last_usermask = NULL;
            }
        }
        if (ni->last_realmask) {
            s = strchr(ni->last_realmask, '@');
            if (!s || s==ni->last_realmask || !s[1]) {
                fprintf(stderr, "Last real usermask for nickname %s isn't"
                        " a valid user@host mask, clearing.\n", ni->nick);
                ni->last_realmask = NULL;
            }
        }

        /* Make sure last-seen time is not earlier than registered time.
         * Allow zero, to accommodate cases where the nickname was
         * registered by an outside entity without the user actually
         * logging on. */
        if (ni->last_seen && ni->last_seen < ni->time_registered) {
            fprintf(stderr, "Last-seen time for nickname %s is earlier than"
                    " registration time!  Setting last-seen time to"
                    " registration time.\n", ni->nick);
            ni->last_seen = ni->time_registered;
        }

    }  /* for all nicks */
}

/*************************************************************************/

static void sanity_check_nickgroups(void)
{
    NickInfo *ni;
    NickGroupInfo *ngi;
    char *s;
    int i;

    for (ngi = first_nickgroupinfo(); ngi; ngi = next_nickgroupinfo()) {

        /* Make sure nickgroups don't contain extra nicks. */
        ARRAY_FOREACH(i, ngi->nicks) {
            ni = get_nickinfo(ngi->nicks[i]);
            if (!ni) {
                fprintf(stderr, "BUG: Nickgroup %u contains nonexistent"
                        " nickname %s!  Removing nickname from group.\n",
                        ngi->id, ngi->nicks[i]);
                ARRAY_REMOVE(ngi->nicks, i);
                if (ngi->mainnick >= i)
                    ngi->mainnick--;
                i--;  /* to make sure we don't skip any */
            }
        }

        /* Check for empty nickgroups (possibly from extraneous nickname
         * removal above). */
        if (ngi->nicks_count == 0) {
            fprintf(stderr, "Removing empty nickgroup %u.\n", ngi->id);
            del_nickgroupinfo(ngi);
            continue;
        }

        /* Make sure main nick is a valid index. */
        if (ngi->mainnick >= ngi->nicks_count) {
            fprintf(stderr, "Invalid main nick index in nickgroup %u,"
                    " resetting.\n", ngi->id);
            ngi->mainnick = 0;
        }

        /* If an authcode is set, make sure the reason is valid.  If not,
         * clear the auth-related fields. */
        if (ngi->authcode && (ngi->authreason < NICKAUTH_MIN
                           || ngi->authreason > NICKAUTH_MAX)) {
            fprintf(stderr, "Authentication code set for nickgroup %u but"
                    " reason code invalid, setting to SETAUTH.\n", ngi->id);
            ngi->authreason = NICKAUTH_SETAUTH;
        } else {
            ngi->authset = 0;
            ngi->authreason = 0;
        }

        /* Clear all unknown flags. */
        ngi->flags &= NF_ALLFLAGS;

        /* Make sure language setting is in range. */
        if ((ngi->language < 0 || ngi->language >= NUM_LANGS)
         && ngi->language != LANG_DEFAULT
        ) {
            fprintf(stderr, "Invalid language set for nickgroup %u,"
                    " resetting to default.\n", ngi->id);
            ngi->language = LANG_DEFAULT;
        }

        /* Make sure access, autojoin, and ignore counts are non-negative. */
        if (ngi->access_count < 0) {
            fprintf(stderr, "BUG: Access entry count for nickgroup %u is"
                    " negative!  Clearing.\n", ngi->id);
            ngi->access = NULL;
            ngi->access_count = 0;
        }
        if (ngi->ajoin_count < 0) {
            fprintf(stderr, "BUG: Autojoin entry count for nickgroup %u is"
                    " negative!  Clearing.\n", ngi->id);
            ngi->ajoin = NULL;
            ngi->ajoin_count = 0;
        }
        if (ngi->ignore_count < 0) {
            fprintf(stderr, "BUG: Ignore entry count for nickgroup %u is"
                    " negative!  Clearing.\n", ngi->id);
            ngi->ignore = NULL;
            ngi->ignore_count = 0;
        }

        /* Make sure all access entries have non-empty user and host parts. */
        ARRAY_FOREACH (i, ngi->access) {
            if (!ngi->access[i]
             || !(s = strchr(ngi->access[i], '@'))
             || s == ngi->access[i]
             || !s[1]
            ) {
                fprintf(stderr, "Access entry %d for nickgroup %u %s,"
                        " deleting.\n", i, ngi->id,
                        !ngi->access[i] ? "is empty"
                                        : "isn't a valid user@host mask");
                ARRAY_REMOVE(ngi->access, i);
                i--;
            }
        }

        /* Make sure memo count is non-negative. */
        if (ngi->memos.memos_count < 0) {
            fprintf(stderr, "BUG: Memo count for nickgroup %u is negative!"
                    "  Clearing.\n", ngi->id);
            ngi->memos.memos = NULL;
            ngi->memos.memos_count = 0;
        }

        /* Clear unknown flags from memos. */
        ARRAY_FOREACH (i, ngi->memos.memos)
            ngi->memos.memos[i].flags &= MF_ALLFLAGS;

    }  /* for all nickgroups */
}

/*************************************************************************/

static void sanity_check_channels(void)
{
    ChannelInfo *ci;
    char *s;
    int i;

    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {

        /* Forbidden channels should have no other data associated with
         * them. */
        if (ci->flags & CF_VERBOTEN) {
            ci->founder = 0;
            ci->successor = 0;
            init_password(&ci->founderpass);
            ci->desc = NULL;
            ci->url = NULL;
            ci->email = NULL;
            ci->last_used = 0;
            ci->last_topic = NULL;
            memset(ci->last_topic_setter, 0, sizeof(ci->last_topic_setter));
            ci->last_topic_time = 0;
            ci->flags &= ~(CF_VERBOTEN);
            reset_levels(ci);
            ci->access = NULL;
            ci->access_count = 0;
            ci->akick = NULL;
            ci->akick_count = 0;
            ci->mlock.on = NULL;
            ci->mlock.off = NULL;
            ci->mlock.limit = 0;
            ci->mlock.key = NULL;
            ci->mlock.link = NULL;
            ci->mlock.flood = NULL;
            ci->mlock.joindelay = 0;
            ci->mlock.joinrate1 = 0;
            ci->mlock.joinrate2 = 0;
            ci->entry_message = NULL;
            continue;
        }

        /* Channel is not forbidden.  First make sure it has a (valid)
         * founder. */
        if (!ci->founder) {
            fprintf(stderr, "BUG: Channel %s has no founder!  Deleting.\n",
                    ci->name);
            del_channelinfo(ci);
        } else if (!get_nickgroupinfo(ci->founder)) {
            fprintf(stderr, "BUG: Channel %s founder nickgroup %u is"
                    " missing!  Deleting channel.\n", ci->name, ci->founder);
            del_channelinfo(ci);
        }

        /* Make sure that the successor is valid if set. */
        if (ci->successor && !get_nickgroupinfo(ci->successor)) {
            fprintf(stderr, "BUG: Channel %s successor nickgroup %u is"
                    " missing!  Clearing.", ci->name, ci->successor);
            ci->successor = 0;
        }

        /* Make sure last-used time is not earlier than registered time.
         * Allow zero, to accommodate cases where the channel was
         * registered by an outside entity. */
        if (ci->last_used && ci->last_used < ci->time_registered) {
            fprintf(stderr, "Last-used time for channel %s is earlier than"
                    " registration time!  Setting last-used time to"
                    " registration time.\n", ci->name);
            ci->last_used = ci->time_registered;
        }

        /* If there is no saved topic, clear the topic setter and time. */
        if (!ci->last_topic) {
            memset(ci->last_topic_setter, 0, sizeof(ci->last_topic_setter));
            ci->last_topic_time = 0;
        }

        /* Clear unknown flags. */
        ci->flags &= CF_ALLFLAGS;

        /* Check privilege level settings for vaility. */
        for (i = 0; i < CA_SIZE; i++) {
            if (ci->levels[i] != ACCLEV_DEFAULT
             && ci->levels[i] != ACCLEV_INVALID
             && ci->levels[i] != ACCLEV_FOUNDER
             && (ci->levels[i] < ACCLEV_MIN
                 || ci->levels[i] > ACCLEV_MAX)
            ) {
                fprintf(stderr, "Privilege level %d on channel %s is"
                        "invalid, resetting to default.\n", i, ci->name);
                ci->levels[i] = ACCLEV_DEFAULT;
                break;
            }
        }

        /* Check access level nickgroups and levels for validity. */
        ARRAY_FOREACH (i, ci->access) {
            if (!ci->access[i].nickgroup)
                continue;
            if (!get_nickgroupinfo(ci->access[i].nickgroup)) {
                fprintf(stderr, "BUG: Channel %s access entry %d has an"
                        " invalid nickgroup!  Clearing.\n", ci->name, i);
                ci->access[i].nickgroup = 0;
                ci->access[i].level = 0;
            } else if (ci->access[i].level < ACCLEV_MIN
                    || ci->access[i].level > ACCLEV_MAX) {
                fprintf(stderr, "BUG: Channel %s access entry %d has an"
                        "out-of-range level (%d)!  Clearing.\n",
                        ci->name, i, ci->access[i].level);
                ci->access[i].nickgroup = 0;
                ci->access[i].level = 0;
            }
        }

        /* Make sure all autokick entries have non-empty user and host
         * parts. */
        ARRAY_FOREACH (i, ci->akick) {
            if (!ci->akick[i].mask)
                continue;
            s = strchr(ci->akick[i].mask, '@');
            if (!s || s==ci->akick[i].mask || !s[1]) {
                fprintf(stderr, "Autokick entry %d for channel %s isn't a"
                        " valid user@host mask, deleting.\n", i, ci->name);
                ci->akick[i].mask = NULL;
            }
        }

    }  /* for all channels */

    /* Check that channel successors and access list entries have valid
     * nickgroup IDs, and that access levels are in range */
    for (ci = first_channelinfo(); ci; ci = next_channelinfo()) {
        if (ci->flags & CF_VERBOTEN)
            continue;
    }
}

/*************************************************************************/

static void sanity_check_maskdata(void)
{
    MaskData *md;
    char *s;
    int i;

    /* Make sure all MaskData entries actually have masks allocated. */
    for (i = 0; i < 256; i++) {
        for (md = first_maskdata(i); md; md = next_maskdata(i)) {
            if (!md->mask)
                del_maskdata(i, md);
        }
    }

    /* Make sure every autokill has a valid user@host mask and a reason. */
    for (md = first_maskdata(MD_AKILL); md; md = next_maskdata(MD_AKILL)) {
        s = strchr(md->mask, '@');
        if (!s || s==md->mask || !s[1]) {
            fprintf(stderr, "Autokill %s isn't a valid user@host mask,"
                    " deleting.\n", md->mask);
            del_maskdata(MD_AKILL, md);
            continue;
        }
        if (!md->reason)
            md->reason = "Reason unknown";
    }
}

/*************************************************************************/
/****************************** Main program *****************************/
/*************************************************************************/

void usage(const char *progname)
{
    int i;

    fprintf(stderr, "Usage: %s [-v] [+program-name] [options...] dir\n"
                    "The following program names are known:\n", progname);
    for (i = 0; dbtypes[i]; i++)
        fprintf(stderr, "    %s\n", dbtypes[i]->id);
    fprintf(stderr,
            "See the manual for options available for each database type.\n");
    exit(1);
}

/*************************************************************************/

int main(int ac, char **av)
{
    int newac = 0;              /* For passing to processing function */
    char **newav;
    char *dir = NULL;     /* Source data file directory */
    int verbose = 0;            /* Verbose output? */
    void (*load)(const char *, int, int, char **) = NULL;
    int i;
    char oldpath[PATH_MAX+1], newpath[PATH_MAX+1];

#if CLEAN_COMPILE
    free_nickinfo(NULL);
    free_nickgroupinfo(NULL);
    free_channelinfo(NULL);
#endif

    init_password(&supass);

    /* Parse command-line parameters */
    newac = 1;
    newav = malloc(sizeof(*newav) * ac);
    newav[0] = av[0];
    for (i = 1; i < ac; i++) {
        if (strcmp(av[i],"-v") == 0) {
            verbose++;
        } else if (strcmp(av[i],"-h") == 0
                || strcmp(av[i],"-?") == 0
                || strcmp(av[i],"-help") == 0
                || strcmp(av[i],"--help") == 0) {
            usage(av[0]);
        } else if (av[i][0] == '+') {
            int j;
            for (j = 0; dbtypes[j]; j++) {
                if (stricmp(av[i]+1, dbtypes[j]->id) == 0) {
                    load = dbtypes[j]->load;
                    break;
                }
            }
            if (!load) {
                fprintf(stderr, "Unknown database type `%s'\n", av[i]+1);
                usage(av[0]);
            }
        } else if (av[i][0] == '-') {
            newav[newac++] = av[i];
        } else {
            if (dir) {
                fprintf(stderr, "Only one source directory may be specified\n");
                usage(av[0]);
            }
            dir = av[i];
        }
    }

    /* Make sure that we have a source directory and that it's valid */
    if (!dir) {
        fprintf(stderr, "Directory name must be specified\n");
        usage(av[0]);
    }
    if (access(dir, R_OK) < 0) {
        perror(dir);
        exit(1);
    }

    /* If the source directory name is relative, make it absolute */
    if (*dir != '/') {
        if (!getcwd(oldpath, sizeof(oldpath))) {
            perror("Unable to read current directory name");
            fprintf(stderr, "Try using an absolute pathname for the source directory.\n");
            return 1;
        }
        if (strlen(oldpath) + 1 + strlen(dir) + 1 > sizeof(newpath)) {
            fprintf(stderr, "Source directory pathname too long\n");
            return 1;
        }
        sprintf(newpath, "%s/%s", oldpath, dir);
        dir = newpath;
    }

    /* If we weren't given a database type, try to figure one out */
    if (!load) {
        for (i = 0; dbtypes[i]; i++) {
            const char *s;
            if ((s = dbtypes[i]->check(dir)) != NULL) {
                fprintf(stderr, "Found %s databases\n", s);
                load = dbtypes[i]->load;
                break;
            }
        }
        if (!load) {
            fprintf(stderr, "Can't determine database type; use +name option\n");
            usage(av[0]);
        }
    }

    /* Actually load the databases.  If an error occurs, load() will abort the
     * program for us */
    load(dir, verbose, newac, newav);
    if (verbose)
        fprintf(stderr, "Data files successfully loaded.\n");

    /* Do sanity checks. */
    if (verbose)
        fprintf(stderr, "Checking data integrity...\n");
    sanity_checks();

    /* Write the database to standard output in XML format */
    if (verbose)
        fprintf(stderr, "Writing converted data...\n");
    xml_export((xml_writefunc_t)fprintf, stdout);

    /* Success */
    if (verbose)
        fprintf(stderr, "Done.\n");
    return 0;
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
