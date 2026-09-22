/* External declarations for convert-db.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CONVERT_DB_H
#define CONVERT_DB_H

/*************************************************************************/

/* Common includes. */

#include "services.h"
#include "language.h"
#include "encrypt.h"
#include "modules/database/fileutil.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"
#include "modules/memoserv/memoserv.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/operserv/news.h"
#include "modules/statserv/statserv.h"

/*************************************************************************/
/*************************************************************************/

/* Structure with information about each supported database type. */

typedef struct {
    /* Identifier (used with +xyz command-line option) */
    const char *id;
    /* Routine to check whether databases for this program are in the given
     * directory; returns a program/database-type name for use in the
     * "Found XYZ databases" message if databases are found, NULL if not */
    const char *(*check)(const char *dir);
    /* Routine to load databases from the given directory; prints an
     * appropriate error message and calls exit(1) if an error occurs.
     * ac/av are loaded with all option arguments passed to the program
     * not parsed out by the main code; av[0] will contain the program
     * name as usual */
    void (*load)(const char *dir, int verbose, int ac, char **av);
} DBTypeInfo;

/* List of all available types.  This macro is used twice in convert-db.c:
 * once with an empty PREFIX to declare all of the structures extern, and
 * once more with a PREFIX of & to define an array of pointers to the
 * structures. */

#define ALL_DB_TYPES(PREFIX)    \
    PREFIX dbtype_anope,        \
    PREFIX dbtype_auspice,      \
    PREFIX dbtype_bolivia,      \
    PREFIX dbtype_cygnus,       \
    PREFIX dbtype_daylight,     \
    PREFIX dbtype_epona,        \
    PREFIX dbtype_hybserv,      \
    PREFIX dbtype_ircs_1_2,     \
    PREFIX dbtype_magick_14b2,  \
    PREFIX dbtype_ptlink,       \
    PREFIX dbtype_sirv,         \
    PREFIX dbtype_trircd_4_26,  \
    PREFIX dbtype_wrecked_1_2

/*************************************************************************/
/*************************************************************************/

/* Converted data. */
extern NickGroupInfo *ngi_list;
extern NickInfo *ni_list;
extern ChannelInfo *ci_list;
extern NewsItem *news_list;
extern MaskData *md_list[256];
extern ServerStats *ss_list;
extern int32 maxusercnt;
extern time_t maxusertime;
extern Password supass;
extern int no_supass;

/*************************************************************************/

/* Safe memory allocation and string duplication. */
extern void *smalloc(long size);
extern void *scalloc(long els, long elsize);
extern void *srealloc(void *ptr, long size);
extern char *sstrdup(const char *s);
/* Initialize or clear a Password structure. */
extern void init_password(Password *password);
extern void clear_password(Password *password);

/* Allocate a new, initialized NickInfo (and possibly NickGroupInfo)
 * structure. */
extern NickInfo *makenick(const char *nick, NickGroupInfo **nickgroup_ret);
/* Allocate a new, initialized ChannelInfo structure. */
extern ChannelInfo *makechan(const char *name);

/* Open a (Services pre-5.0 style) data file and check the version number.
 * Prints an error message and exits with 1 if either the file cannot be
 * opened or the version number is wrong.  If `version_ret' is non-NULL,
 * the version number is stored there. */
extern dbFILE *open_db_ver(const char *dir, const char *name,
                           int32 min_version, int32 max_version,
                           int32 *version_ret);


/* Retrieve the NickGroupInfo structure for the given nick.  Returns NULL
 * if the nick does not exist or is forbidden (i.e. has no NickGroupInfo). */
extern NickGroupInfo *get_nickgroupinfo_by_nick(const char *nick);

/* Set the OperServ privilege level (os_priv) for the nickgroup associated
 * with `nick' to `level', if it is not already greater than `level'.  Does
 * nothing if `nick' is NULL or does not have an associated nickgroup. */
extern void set_os_priv(const char *nick, int16 level);

/* Return the Services 5.0 channel access level that corresponds to the
 * given pre-5.0 level. */
extern int16 convert_acclev(int16 old);

/* Add or remove various things to or from the appropriate lists. */
#define add_nickgroupinfo(ngi)  LIST_INSERT((ngi), ngi_list)
#define add_nickinfo(ni)        LIST_INSERT((ni), ni_list)
#define add_channelinfo(ci)     LIST_INSERT((ci), ci_list)
#define add_news(news)          LIST_INSERT((news), news_list)
#define add_maskdata(type,md)   LIST_INSERT((md), md_list[(type)])
#define add_serverstats(ss)     LIST_INSERT((ss), ss_list)
#define del_nickgroupinfo(ngi)  LIST_REMOVE((ngi), ngi_list)
#define del_nickinfo(ni)        LIST_REMOVE((ni), ni_list)
#define del_channelinfo(ci)     LIST_REMOVE((ci), ci_list)
#define del_news(news)          LIST_REMOVE((news), news_list)
#define del_maskdata(type,md)   LIST_REMOVE((md), md_list[(type)])
#define del_serverstats(ss)     LIST_REMOVE((ss), ss_list)

/*************************************************************************/

#ifdef CONVERT_DB_MAIN

/* Macro to handle write errors. */
#define SAFE(x) do {                                            \
    if ((x) < 0) {                                              \
        fprintf(stderr, "Write error on %s: %s\n",              \
                f->filename, strerror(errno));                  \
        exit(1);                                                \
    }                                                           \
} while (0)

#else /* !CONVERT_DB_MAIN */

/* Macro to handle read errors. */
#define SAFE(x) do {                                            \
    if ((x) < 0) {                                              \
        fprintf(stderr, "Read error on %s\n", f->filename);     \
        exit(1);                                                \
    }                                                           \
} while (0)

/* Macro to read in a variable in machine format. */
#define read_variable(var,f)  (read_db((f),&(var),sizeof(var)) == sizeof(var))

#endif  /* CONVERT_DB_MAIN */

/*************************************************************************/

/* Miscellaneous externs. */

extern void usage(const char *progname);

/*************************************************************************/

#endif  /* CONVERT_DB_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
