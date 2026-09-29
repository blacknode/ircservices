/* Initalization and related routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "cache.h"
#include "conffile.h"
#include "databases.h"
#include "db.h"
#include "encrypt.h"
#include "migration.h"
#include "store.h"
#include "language.h"
#include "messages.h"
#include "modules.h"
#include "p10.h"
#include "services.h"
#include "version.h"
#include "worker.h"

#include "drivers.h"

#include <grp.h>
#include <sys/resource.h>
#include <sys/stat.h>

/*************************************************************************/

/* Events announced in this file: */
static Event* command_line_event;

/*************************************************************************/
/************************* Configuration options *************************/
/*************************************************************************/

/* Configurable variables: */

char** LoadModules;
int LoadModules_count;
char** LoadLanguageText;
int LoadLanguageText_count;

char* RemoteServer;
int32 RemotePort;
char* RemotePassword;
char* LocalHost;
int32 LocalPort;

char* ServerName;
char* ServerDesc;
char* ServiceUser;
char* ServiceHost;
char* ServicesChannel;

char* LogFilename;
char PIDFilename[PATH_MAX + 1];
char* MOTDFilename;

int16 DefTimeZone;

int NoBouncyModes;
int NoSplitRecovery;
int StrictPasswords;
int NoAdminPasswordCheck;
int32 BadPassLimit;
time_t BadPassTimeout;
int32 BadPassWarning;
int32 IgnoreDecay;
double IgnoreThreshold;
time_t UpdateTimeout;
int32 WorkerThreads;
int32 WorkerQueueMax;
time_t WarningTimeout;
int32 ReadTimeout;
int32 TimeoutCheck;
time_t PingFrequency;
int32 MergeChannelModes;
int32 TotalNetBufferSize;
int32 NetBufferSize;
int32 NetBufferLimitInactive;
int32 NetBufferLimitIgnore;

char* EncryptionType;
char* GuestNickPrefix;
char** RejectEmail;
int RejectEmail_count;
int32 ListMax;
int LogMaxUsers;
int EnableGetpass;
int WallAdminPrivs;

/* Routines to handle special directives: */
static int do_DefTimeZone(const char* filename, int linenum, char* param);
static int do_IgnoreThreshold(const char* filename, int linenum, char* param);
static int do_LoadLanguageText(const char* filename, int linenum, char* param);
static int do_LoadModule(const char* filename, int linenum, char* param);
static int do_PIDFilename(const char* filename, int linenum, char* param);
static int do_RejectEmail(const char* filename, int linenum, char* param);
static int do_RunGroup(const char* filename, int linenum, char* param);
static int do_ServiceUser(const char* filename, int linenum, char* param);
static int do_ServicesChannel(const char* filename, int linenum, char* param);
static int do_Umask(const char* filename, int linenum, char* param);

/*************************************************************************/

/* Directives of ircservices.conf, by block.  Module blocks (`module
 * "<name>" { ... }') are bound by the module loader, see modules.c. */

/* Top level. */
static ConfigDirective toplevel_directives[] = {
    {"loadlanguage", {{CD_FUNC, CF_MULTI, do_LoadLanguageText}}},
    {"loadmodule", {{CD_FUNC, CF_MULTI, do_LoadModule}}},
    {NULL}};

/* serverinfo { }: how Services present themselves to the network. */
static ConfigDirective serverinfo_directives[] = {
    {"channel", {{CD_FUNC, 0, do_ServicesChannel}}},
    {"description", {{CD_STRING, CF_DIRREQ, &ServerDesc}}},
    {"name", {{CD_STRING, CF_DIRREQ, &ServerName}}},
    {"numeric", {{CD_INT, CF_DIRREQ, &ServerNumeric}}},
    {"user", {{CD_FUNC, CF_DIRREQ, do_ServiceUser}}},
    {NULL}};

/* uplink { }: the ircu2 server Services link to. */
static ConfigDirective uplink_directives[] = {
    {"host", {{CD_STRING, CF_DIRREQ, &RemoteServer}}},
    {"local_address",
     {{CD_STRING, 0, &LocalHost}, {CD_PORT, CF_OPTIONAL, &LocalPort}}},
    {"password", {{CD_STRING, CF_DIRREQ, &RemotePassword}}},
    {"port", {{CD_PORT, CF_DIRREQ, &RemotePort}}},
    {NULL}};

/* files { }: files in the data directory, and the process owning them. */
static ConfigDirective files_directives[] = {
    {"log", {{CD_STRING, CF_DIRREQ, &LogFilename}}},
    {"motd", {{CD_STRING, CF_DIRREQ, &MOTDFilename}}},
    {"pid", {{CD_FUNC, 0, do_PIDFilename}}},
    {"run_group", {{CD_FUNC, 0, do_RunGroup}}},
    {"umask", {{CD_FUNC, 0, do_Umask}}},
    {NULL}};

/* options { }: general behaviour. */
static ConfigDirective options_directives[] = {
    {"bad_pass_limit", {{CD_POSINT, 0, &BadPassLimit}}},
    {"bad_pass_timeout", {{CD_TIME, 0, &BadPassTimeout}}},
    {"bad_pass_warning", {{CD_POSINT, 0, &BadPassWarning}}},
    {"enable_getpass", {{CD_SET, 0, &EnableGetpass}}},
    {"encryption", {{CD_STRING, 0, &EncryptionType}}},
    {"guest_nick_prefix", {{CD_STRING, CF_DIRREQ, &GuestNickPrefix}}},
    {"ignore_decay", {{CD_TIMEMSEC, 0, &IgnoreDecay}}},
    {"ignore_threshold", {{CD_FUNC, 0, do_IgnoreThreshold}}},
    {"list_max", {{CD_POSINT, CF_DIRREQ, &ListMax}}},
    {"log_max_users", {{CD_SET, 0, &LogMaxUsers}}},
    {"merge_channel_modes", {{CD_TIMEMSEC, 0, &MergeChannelModes}}},
    {"no_admin_password_check", {{CD_SET, 0, &NoAdminPasswordCheck}}},
    {"no_bouncy_modes", {{CD_SET, 0, &NoBouncyModes}}},
    {"no_split_recovery", {{CD_SET, 0, &NoSplitRecovery}}},
    {"reject_email", {{CD_FUNC, CF_MULTI, do_RejectEmail}}},
    {"strict_passwords", {{CD_SET, 0, &StrictPasswords}}},
    {"timezone", {{CD_FUNC, 0, do_DefTimeZone}}},
    {"wall_admin_privs", {{CD_SET, 0, &WallAdminPrivs}}},
    {NULL}};

/* timeouts { }: timers. */
static ConfigDirective timeouts_directives[] = {
    {"check", {{CD_TIMEMSEC, CF_DIRREQ, &TimeoutCheck}}},
    {"ping", {{CD_TIME, 0, &PingFrequency}}},
    {"read", {{CD_TIMEMSEC, 0, &ReadTimeout}}},
    {"update", {{CD_TIME, CF_DIRREQ, &UpdateTimeout}}},
    {"warning", {{CD_TIME, CF_DIRREQ, &WarningTimeout}}},
    {NULL}};

/* netbuffer { }: memory for network buffers. */
static ConfigDirective netbuffer_directives[] = {
    {"limit",
     {{CD_POSINT, 0, &NetBufferLimitInactive},
      {CD_POSINT, CF_OPTIONAL, &NetBufferLimitIgnore}}},
    {"size",
     {{CD_POSINT, 0, &TotalNetBufferSize},
      {CD_POSINT, CF_OPTIONAL, &NetBufferSize}}},
    {NULL}};

static ConfigDirective workers_directives[] = {
    {"queue_max", {{CD_POSINT, 0, &WorkerQueueMax}}},
    {"threads", {{CD_POSINT, 0, &WorkerThreads}}},
    {NULL}};

/* The directive tables of the database and redis blocks live beside the
 * code that uses them. */
extern ConfigDirective db_directives[];
extern ConfigDirective cache_directives[];

/* Every block the core reads (a NULL name is the top level). */
static const struct {
    const char* block;
    ConfigDirective* directives;
} core_config[] = {
    {NULL, toplevel_directives},
    {"serverinfo", serverinfo_directives},
    {"uplink", uplink_directives},
    {"files", files_directives},
    {"options", options_directives},
    {"timeouts", timeouts_directives},
    {"netbuffer", netbuffer_directives},
    {"database", db_directives},
    {"redis", cache_directives},
    {"workers", workers_directives},
};

/*************************************************************************/

/* Bind (CONFIGURE_READ and/or CONFIGURE_SET) every core directive table to
 * the current configuration tree.  Every table is read even after an error
 * so that all errors are reported at once. */

static int configure_core(int action)
{
    int i, retval = 1;

    for (i = 0; i < lenof(core_config); i++) {
        if (!configure_block(core_config[i].block, NULL,
                             core_config[i].directives, action))
            retval = 0;
    }
    return retval;
}

/*************************************************************************/

/* Warn about top-level blocks nobody will read: unknown block names,
 * module blocks without a module name, and (debug only) blocks of modules
 * that are not loaded.  Call after the core tables have been set
 * (LoadModules must be current). */

static void check_toplevel_blocks(void)
{
    const ConfNode* root = conf_root();
    const ConfNode* node;
    int i;

    for (node = root ? root->children : NULL; node; node = node->next) {
        if (node->type != CONF_BLOCK)
            continue;
        if (stricmp(node->name, "module") == 0) {
            if (!node->label) {
                config_error(node->file, node->line,
                             "Warning: `module' block without a module name"
                             " ignored");
                continue;
            }
            /* Configuring a module without loading it is normal (the
             * example configuration does it for every module), so this is
             * only worth a debug message. */
            ARRAY_SEARCH_PLAIN(LoadModules, node->label, strcmp, i);
            if (i >= LoadModules_count) {
                log_debug(1, "%s:%d: module `%s' is configured but not"
                          " loaded (no loadmodule for it)",
                          node->file, node->line, node->label);
            }
            continue;
        }
        for (i = 0; i < lenof(core_config); i++) {
            if (core_config[i].block &&
                stricmp(node->name, core_config[i].block) == 0)
                break;
        }
        if (i >= lenof(core_config)) {
            config_error(node->file, node->line,
                         "Warning: unknown block `%s' ignored", node->name);
        }
    }
}

/*************************************************************************/

/* Load the configuration file into a new tree and read the core
 * directives from it.  On success the new tree is committed (modules read
 * their blocks from it) and the new values are set; on failure the
 * previous configuration, if any, is left untouched.  Returns nonzero on
 * success. */

static int load_config(void)
{
    if (!conf_load(IRCSERVICES_CONF))
        return 0;
    if (!configure_core(CONFIGURE_READ) || !cache_config_ok()) {
        conf_discard();
        return 0;
    }
    conf_commit();
    configure_core(CONFIGURE_SET);
    /* Publish the database, cache and worker settings: a changed database
     * block restarts the connection pools, a changed redis block the cache
     * connections, and a changed workers block resizes the pool (which
     * only exists once Services have forked; see init()). */
    db_config_apply();
    cache_config_apply();
    if (WorkerThreads > WORKER_MAX_THREADS) {
        config_error(IRCSERVICES_CONF, 0,
                     "workers: at most %d threads; using %d",
                     WORKER_MAX_THREADS, WORKER_MAX_THREADS);
        WorkerThreads = WORKER_MAX_THREADS;
    }
    worker_configure(WorkerThreads ? WorkerThreads : WORKER_DEFAULT_THREADS,
                     WorkerQueueMax ? WorkerQueueMax
                                    : WORKER_DEFAULT_QUEUE_MAX);
    check_toplevel_blocks();
    return 1;
}

/*************************************************************************/

/* read_config():  Read the main configuration file.  If an error occurs
 *                 while reading the file or a required directive is not
 *                 found, print and log an appropriate error message and
 *                 return 0; otherwise, return 1.  The whole file is parsed
 *                 here, before any module is loaded, so module blocks are
 *                 available to modules from the moment they load.
 */

static int read_config(void)
{
    int retval = 1;

    if (!load_config())
        return 0;

    if (TotalNetBufferSize) {
        if (TotalNetBufferSize < SOCK_MIN_BUFSIZE * 2) {
            config_error(IRCSERVICES_CONF, 0,
                         "netbuffer: total `size' must be at least %d",
                         SOCK_MIN_BUFSIZE * 2);
            retval = 0;
        }
        else {
            /* Make sure it's a multiple of SOCK_MIN_BUFSIZE */
            TotalNetBufferSize /= SOCK_MIN_BUFSIZE;
            TotalNetBufferSize *= SOCK_MIN_BUFSIZE;
            if (NetBufferSize) {
                if (NetBufferSize < SOCK_MIN_BUFSIZE * 2) {
                    config_error(IRCSERVICES_CONF, 0,
                                 "netbuffer: per-connection `size' must be"
                                 " at least %d",
                                 SOCK_MIN_BUFSIZE * 2);
                    retval = 0;
                }
                else if (NetBufferSize > TotalNetBufferSize) {
                    config_error(IRCSERVICES_CONF, 0,
                                 "netbuffer: per-connection `size' must be"
                                 " no more than the total");
                    retval = 0;
                }
                else {
                    NetBufferSize /= SOCK_MIN_BUFSIZE;
                    NetBufferSize *= SOCK_MIN_BUFSIZE;
                }
            }
        }
    } /* if (TotalNetBufferSize) */

    if (NetBufferLimitInactive) {
        if (!TotalNetBufferSize) {
            NetBufferLimitInactive = NetBufferLimitIgnore = 0;
        }
        else {
            if (NetBufferLimitInactive > 99 || NetBufferLimitIgnore > 99) {
                config_error(IRCSERVICES_CONF, 0,
                             "netbuffer: `limit' thresholds must be between"
                             " 1 and 99 inclusive");
                retval = 0;
            }
            if (NetBufferLimitIgnore &&
                NetBufferLimitIgnore < NetBufferLimitInactive) {
                config_error(IRCSERVICES_CONF, 0,
                             "netbuffer: `limit' ignore threshold must be"
                             " greater than or equal to inactive threshold");
                retval = 0;
            }
        }
    }

    return retval;
}

/*************************************************************************/
/*************************************************************************/

/* Configuration directive callback functions: */

/*************************************************************************/

#define TZ_MAXLEN 16

static int do_DefTimeZone(const char* filename, int linenum, char* param)
{
    static const char* origTZ = NULL;
    static char newTZ[TZ_MAXLEN + 1];
    static char tzbuf[TZ_MAXLEN + 4]; /* for setting TZ */

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                /* Prepare for reading config file */
                *newTZ = 0;
                if (!origTZ) {
                    /* Obtain current value of TZ environment variable (once
                     * only, at start of program), and truncate to TZ_MAXLEN */
                    origTZ = getenv("TZ");
                    if (!origTZ)
                        origTZ = "";
                    if (strlen(origTZ) > TZ_MAXLEN) {
                        static char new_origTZ[TZ_MAXLEN + 1];
                        memcpy(new_origTZ, origTZ, TZ_MAXLEN);
                        new_origTZ[TZ_MAXLEN] = 0;
                        origTZ = new_origTZ;
                    }
                }
                break;
            case CDFUNC_SET:
                /* Copy data to config variables */
                if (!origTZ) {
                    log("BUG: origTZ not set in do_DefTimeZone/CDFUNC_SET");
                    break;
                }
                snprintf(tzbuf, sizeof(tzbuf), "TZ=%s",
                         *newTZ ? newTZ : origTZ);
                if (putenv(tzbuf) < 0) {
                    log("Warning: putenv(%s) failed, time zone may be "
                        "incorrect",
                        tzbuf);
                }
                break;
            case CDFUNC_DECONFIG:
                /* Reset to initial values */
                if (!origTZ) {
                    log("BUG: origTZ not set in "
                        "do_DefTimeZone/CDFUNC_DECONFIG");
                    break;
                }
                snprintf(tzbuf, sizeof(tzbuf), "TZ=%s", origTZ);
                if (putenv(tzbuf) < 0) {
                    log("Warning: putenv(%s) failed, time zone may be "
                        "incorrect",
                        tzbuf);
                }
                break;
        } /* switch (linenum) */
    }
    else { /* filename != NULL, process parameter */
        if (strlen(param) > TZ_MAXLEN) {
            config_error(filename, linenum,
                         "DefTimeZone parameter must not"
                         " be longer than %d characters",
                         TZ_MAXLEN);
            return 0;
        }
        else {
            strbcpy(newTZ, param);
        }
    }
    return 1;
}

/*************************************************************************/

static int do_IgnoreThreshold(const char* filename, int linenum, char* param)
{
    static double new_IgnoreThreshold = 0;

    if (filename) {
        new_IgnoreThreshold = strtod(param, &param);
        if (*param || new_IgnoreThreshold <= 0) {
            config_error(filename, linenum,
                         "Parameter for IgnoreThreshold"
                         " must be a positive number");
            return 0;
        }
    }
    else if (linenum == CDFUNC_INIT) {
        new_IgnoreThreshold = 0;
    }
    else if (linenum == CDFUNC_SET) {
        IgnoreThreshold = new_IgnoreThreshold;
    }
    else if (linenum == CDFUNC_DECONFIG) {
        IgnoreThreshold = 0;
    }
    return 1;
}

/*************************************************************************/

static int do_LoadLanguageText(const char* filename, int linenum, char* param)
{
    static char** new_LoadLanguageText = NULL;
    static int new_LoadLanguageText_count = 0;
    int i;

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                /* Prepare for reading config file: clear out "new" array */
                ARRAY_FOREACH(i, new_LoadLanguageText)
                free(new_LoadLanguageText[i]);
                free(new_LoadLanguageText);
                new_LoadLanguageText = NULL;
                new_LoadLanguageText_count = 0;
                break;
            case CDFUNC_SET:
                /* Copy data to config variables */
                ARRAY_FOREACH(i, LoadLanguageText)
                free(LoadLanguageText[i]);
                free(LoadLanguageText);
                LoadLanguageText = new_LoadLanguageText;
                LoadLanguageText_count = new_LoadLanguageText_count;
                new_LoadLanguageText = NULL;
                new_LoadLanguageText_count = 0;
                break;
            case CDFUNC_DECONFIG:
                /* Clear out config variables */
                ARRAY_FOREACH(i, LoadLanguageText)
                free(LoadLanguageText[i]);
                free(LoadLanguageText);
                LoadLanguageText = NULL;
                LoadLanguageText_count = 0;
                break;
        }
        return 1;
    } /* if (!filename) */

    /* We can't use ARRAY_EXTEND because SIGUSR1 (for srealloc()) may not
     * be ready yet */
    if (new_LoadLanguageText_count + 1 < new_LoadLanguageText_count) {
        config_error(filename, linenum, "LoadLanguageText: too many files!");
        return 0;
    }
    new_LoadLanguageText =
        realloc(new_LoadLanguageText,
                sizeof(char*) * (new_LoadLanguageText_count + 1));
    param = strdup(param);
    if (!new_LoadLanguageText || !param) {
        config_error(filename, linenum, "LoadLanguageText: out of memory!");
        return 0;
    }
    new_LoadLanguageText[new_LoadLanguageText_count++] = param;
    return 1;
}

/*************************************************************************/

static int do_LoadModule(const char* filename, int linenum, char* param)
{
    static char** new_LoadModules = NULL;
    static int new_LoadModules_count = 0;
    int i;

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                /* Prepare for reading config file: clear out "new" array */
                ARRAY_FOREACH(i, new_LoadModules)
                free(new_LoadModules[i]);
                free(new_LoadModules);
                new_LoadModules = NULL;
                new_LoadModules_count = 0;
                break;
            case CDFUNC_SET:
                /* Copy data to config variables */
                ARRAY_FOREACH(i, LoadModules)
                free(LoadModules[i]);
                free(LoadModules);
                LoadModules = new_LoadModules;
                LoadModules_count = new_LoadModules_count;
                new_LoadModules = NULL;
                new_LoadModules_count = 0;
                break;
            case CDFUNC_DECONFIG:
                /* Clear out config variables */
                ARRAY_FOREACH(i, LoadModules)
                free(LoadModules[i]);
                free(LoadModules);
                LoadModules = NULL;
                LoadModules_count = 0;
                break;
        }
        return 1;
    } /* if (!filename) */

    /* We can't use ARRAY_EXTEND because SIGUSR1 (for srealloc()) may not
     * be ready yet */
    if (new_LoadModules_count + 1 < new_LoadModules_count) {
        config_error(filename, linenum, "LoadModule: too many modules!");
        return 0;
    }
    new_LoadModules =
        realloc(new_LoadModules, sizeof(char*) * (new_LoadModules_count + 1));
    param = strdup(param);
    if (!new_LoadModules || !param) {
        config_error(filename, linenum, "LoadModule: out of memory!");
        return 0;
    }
    new_LoadModules[new_LoadModules_count++] = param;
    return 1;
}

/*************************************************************************/

static int do_PIDFilename(const char* filename, int linenum, char* param)
{
    static char* new_PIDFilename = NULL;

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                /* Prepare for reading config file */
                free(new_PIDFilename);
                new_PIDFilename = NULL;
                break;
            case CDFUNC_SET:
                /* Copy data to config variables */
                if (new_PIDFilename) {
                    strbcpy(PIDFilename, new_PIDFilename);
                }
                else {
                    *PIDFilename = 0;
                }
                free(new_PIDFilename);
                new_PIDFilename = NULL;
                break;
            case CDFUNC_DECONFIG:
                /* Clear out config variables.  For PIDFilename, however, we
                 * leave the value in place so the file can be removed on exit
                 * properly. */
                break;
        }
        return 1;
    } /* if (!filename) */

    if (strlen(param) >= sizeof(PIDFilename)) {
        config_error(filename, linenum, "PIDFilename: Path too long");
        return 0;
    }
    new_PIDFilename = strdup(param);
    if (!new_PIDFilename) {
        config_error(filename, linenum, "PIDFilename: Out of memory!");
        return 0;
    }
    return 1;
}

/*************************************************************************/

static int do_RejectEmail(const char* filename, int linenum, char* param)
{
    static char** new_RejectEmail = NULL;
    static int new_RejectEmail_count = 0;
    int i;

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                /* Prepare for reading config file: clear out "new" array */
                ARRAY_FOREACH(i, new_RejectEmail)
                free(new_RejectEmail[i]);
                free(new_RejectEmail);
                new_RejectEmail = NULL;
                new_RejectEmail_count = 0;
                break;
            case CDFUNC_SET:
                /* Copy data to config variables */
                ARRAY_FOREACH(i, RejectEmail)
                free(RejectEmail[i]);
                free(RejectEmail);
                RejectEmail = new_RejectEmail;
                RejectEmail_count = new_RejectEmail_count;
                new_RejectEmail = NULL;
                new_RejectEmail_count = 0;
                break;
            case CDFUNC_DECONFIG:
                /* Clear out config variables */
                ARRAY_FOREACH(i, RejectEmail)
                free(RejectEmail[i]);
                free(RejectEmail);
                RejectEmail = NULL;
                RejectEmail_count = 0;
                break;
        }
        return 1;
    } /* if (!filename) */

    ARRAY_EXTEND(new_RejectEmail);
    new_RejectEmail[new_RejectEmail_count - 1] = sstrdup(param);
    return 1;
}

/*************************************************************************/

static int do_RunGroup(const char* filename, int linenum, char* param)
{
#ifndef SIZEOF_GID_T
    config_error(filename, linenum,
                 "RunGroup: groups not supported on this system");
    return 0;
#else
    static gid_t groupnum = -1;
    long tmp;

    if (filename) {

        if (*param == '=') {
            if (!param[1]) {
                config_error(filename, linenum,
                             "RunGroup: group number required after `='");
                return 0;
            }
            errno = 0;
            tmp = strtol(param + 1, &param, 0);
#if SIZEOF_GID_T >= 4
            if (errno == ERANGE)
#else
            if (tmp < -32768 || tmp > 65535)
#endif
            {
                config_error(filename, linenum,
                             "RunGroup: group number out of range (0..%ld)",
#if SIZEOF_GID_T >= 4
                             0x7FFFFFFFL
#else
                             65535L
#endif
                );
                return 0;
            }
            groupnum = tmp;
        }
        else { /* *param != '=' */
            struct group* gr;
            setgrent();
            while ((gr = getgrent()) != NULL) {
                if (strcmp(gr->gr_name, param) == 0)
                    break;
            }
            endgrent();
            if (!gr) {
                config_error(filename, linenum, "RunGroup: unknown group `%s'",
                             param);
                return 0;
            }
            groupnum = gr->gr_gid;
        }
    }
    else { /* !filename */

        if (setregid(groupnum, groupnum) < 0) {
            config_error(filename, linenum,
                         "RunGroup: unable to set group: %s", strerror(errno));
            return 0;
        }

    } /* if (filename) */

    return 1;

#endif /* have gid_t? */
}

/*************************************************************************/

static int do_ServiceUser(const char* filename, int linenum, char* param)
{
    char* s;
    static char *new_ServiceUser = NULL, *new_ServiceHost = NULL;

    if (filename) {
        /* Make sure the string has an @ and there's at least one character
         * on each side */
        s = strchr(param, '@');
        if (!s || s == param || s[1] == 0) {
            config_error(filename, linenum,
                         "`user@host' string required for ServiceUser");
            return 0;
        }
        /* Store user and hostname parts (after freeing any old ones) */
        *s++ = 0;
        free(new_ServiceUser);
        free(new_ServiceHost);
        new_ServiceUser = strdup(param);
        new_ServiceHost = strdup(s);
        if (!new_ServiceUser || !new_ServiceHost) {
            free(new_ServiceUser); /* still alloc'ed if ServiceHost failed */
            new_ServiceUser = NULL;
            config_error(filename, linenum, "Out of memory");
            return 0;
        }
    }
    else if (linenum == CDFUNC_SET) {
        /* Copy new values to config variables and clear */
        if (new_ServiceUser && new_ServiceHost) { /* paranoia */
            free(ServiceUser);
            free(ServiceHost);
            ServiceUser = new_ServiceUser;
            ServiceHost = new_ServiceHost;
        }
        else {
            free(new_ServiceUser);
            free(new_ServiceHost);
        }
        new_ServiceUser = new_ServiceHost = NULL;
    }
    else if (linenum == CDFUNC_DECONFIG) {
        /* Reset to defaults */
        free(ServiceUser);
        free(ServiceHost);
        ServiceUser = NULL;
        ServiceHost = NULL;
    }
    return 1;
}

/*************************************************************************/

/* serverinfo { channel }: the channel every pseudoclient joins once it is
 * introduced.  Optional; without it the pseudoclients join no channel. */

static int do_ServicesChannel(const char* filename, int linenum, char* param)
{
    static char* new_ServicesChannel = NULL;

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                free(new_ServicesChannel);
                new_ServicesChannel = NULL;
                break;
            case CDFUNC_SET:
                free(ServicesChannel);
                ServicesChannel = new_ServicesChannel;
                new_ServicesChannel = NULL;
                break;
            case CDFUNC_DECONFIG:
                free(ServicesChannel);
                ServicesChannel = NULL;
                break;
        }
        return 1;
    } /* if (!filename) */

    /* A network-wide channel (ircu2 has only `#' ones; `&' ones are local
     * to a server) with none of the characters ircu refuses in a name:
     * space, comma (which would also make the JOIN a list of channels)
     * and ^G. */
    if (*param != '#' || !param[1] ||
        param[strcspn(param, " ,\a")] != 0) {
        config_error(filename, linenum,
                     "serverinfo channel: `%s' is not a channel name", param);
        return 0;
    }
    if (strlen(param) >= CHANMAX) {
        config_error(filename, linenum,
                     "serverinfo channel: name too long (maximum %d)",
                     CHANMAX - 1);
        return 0;
    }
    free(new_ServicesChannel);
    new_ServicesChannel = sstrdup(param);
    return 1;
}

/*************************************************************************/

static int do_Umask(const char* filename, int linenum, char* param)
{
    char* s;
    static int umask_val = -1;

    if (filename) {
        umask_val = strtol(param, &s, 8);
        if (*s || umask_val < 0 || umask_val > 0777) {
            config_error(filename, linenum,
                         "Expected an octal value between 000 and 777");
            return 0;
        }
    }
    else {
        if (umask_val >= 0)
            umask(umask_val);
        umask_val = -1;
    }
    return 1;
}

/*************************************************************************/
/*********************** Introducing pseudoclients ***********************/
/*************************************************************************/

/*************************************************************************/
/************************* Command-line parsing **************************/
/*************************************************************************/

/* Parse command-line options.  If `call_modules' is zero, parse main
 * options and ignore unrecognized ones; if nonzero, ignore main options,
 * emit the "core.command_line" event to check module options, and fail on
 * unrecognized ones.  Return 0 if all went well, -1 for an error with an
 * option, or 1 if Services should exit successfully.  Calls exit(0) if
 * "-help", "--help", or "-h" is present.
 */

static int parse_options(int ac, char** av, int call_modules)
{
    int i;
    char *s, *t;

    if (!call_modules) /* only initialize it once */
        debug = 0;

    for (i = 1; i < ac; i++) {
        s = av[i];
        if (*s == '-') {
            s++;
            if (*s == '-')
                s++;
            if (strncmp(s, "dir", 3) == 0 && (!s[3] || s[3] == '=')) {
                if (!call_modules) {
                    if (!s[3]) {
                        fprintf(stderr, "-dir requires a parameter\n");
                        return -1;
                    }
                    services_dir = s + 4;
                }
            }
            else if (strncmp(s, "remote", 6) == 0 && (!s[6] || s[6] == '=')) {
                if (!call_modules) {
                    if (!s[6]) {
                        fprintf(stderr, "-remote requires hostname[:port]\n");
                        return -1;
                    }
                    s += 7;
                    t = strchr(s, ':');
                    if (t) {
                        *t = 0;
                        if (atoi(t + 1) > 0)
                            RemotePort = atoi(t + 1);
                        else {
                            fprintf(stderr,
                                    "-remote: port number must be a"
                                    " positive integer.  Using default.\n");
                            return -1;
                        }
                    }
                    free(RemoteServer);
                    RemoteServer = strdup(s);
                    if (!RemoteServer) {
                        fprintf(stderr, "Out of memory\n");
                        exit(-1);
                    }
                    if (t) /* put the colon back for next time around */
                        *t = ':';
                }
            }
            else if (strncmp(s, "log", 3) == 0 && (!s[3] || s[3] == '=')) {
                if (!call_modules) {
                    if (!s[3]) {
                        fprintf(stderr, "-log requires a parameter\n");
                        return -1;
                    }
                    free(LogFilename);
                    LogFilename = sstrdup(s + 4);
                }
            }
            else if (strcmp(s, "debug") == 0) {
                if (!call_modules)
                    debug++;
            }
            else if (strcmp(s, "readonly") == 0) {
                if (!call_modules)
                    readonly = 1;
            }
            else if (strcmp(s, "nofork") == 0) {
                if (!call_modules)
                    nofork = 1;
            }
            else if (strcmp(s, "noexpire") == 0) {
                if (!call_modules)
                    noexpire = 1;
            }
            else if (strcmp(s, "noakill") == 0) {
                if (!call_modules)
                    noakill = 1;
            }
            else if (strcmp(s, "h") == 0 || strcmp(s, "help") == 0 ||
                     strcmp(s, "-help") == 0) {
                fputs("The following options are recognized:\n"
                      "       -dir=directory          Directory containing "
                      "Services' data files\n"
                      "                                   (e.g. "
                      "/usr/local/lib/ircservices)\n"
                      "       -remote=server[:port]   Remote server to "
                      "connect to\n"
                      "       -log=filename           Services log filename "
                      "(e.g. services.log)\n"
                      "       -debug                  Enable debugging "
                      "mode--more info sent to log\n"
                      "                                   (give option more "
                      "times for more info)\n"
                      "       -readonly               Enable read-only "
                      "mode--no changes to\n"
                      "                                   databases allowed, "
                      ".db files and log\n"
                      "                                   not written\n"
                      "       -nofork                 Do not fork after "
                      "startup; log messages will\n"
                      "                                   be written to "
                      "terminal (as well as to\n"
                      "                                   the log file if not "
                      "in read-only mode)\n"
                      "       -noexpire               Prevents all "
                      "expirations (nicknames, channels,\n"
                      "                                   akills, session "
                      "limit exceptions, etc.)\n"
                      "       -noakill                Disables autokill "
                      "checking\n"
                      "Other options may be available depending on loaded "
                      "modules; see the manual\n"
                      "for details.\n",
                      stdout);
                exit(0);
            }
            else if (call_modules) {
                int res;
                t = strchr(s, '=');
                if (t)
                    *t++ = 0;
                res = event_emit(command_line_event, s, t);
                switch (res) {
                    case 0:
                        fprintf(stderr,
                                "Unknown option -%s.  Use \"-help\" for"
                                " help.\n",
                                s);
                        return -1;
                    case 1:
                        break;
                    case 2:
                        return -1;
                    case 3:
                        return 1;
                    default:
                        log("init: bad return value (%d) from command line"
                            " callback for `-%s%s%s'",
                            res, s, t ? "=" : "", t);
                        return -1;
                }
            }
        }
        else {
            fprintf(stderr, "Non-option arguments not allowed\n");
            return -1;
        }
    }
    return 0;
}

/*************************************************************************/
/*************************** PID file handling ***************************/
/*************************************************************************/

/* Remove our PID file.  Done at exit. */

static void remove_pidfile(void)
{
    if (*PIDFilename)
        remove(PIDFilename);
}

/*************************************************************************/

/* Create our PID file and write the PID to it.  Returns nonzero on
 * success, zero on failure.
 */

static int write_pidfile(void)
{
    FILE* pidfile;

    pidfile = fopen(PIDFilename, "w");
    if (!pidfile)
        return 0;
    fprintf(pidfile, "%d\n", (int)getpid());
    fclose(pidfile);
    atexit(remove_pidfile);
    return 1;
}

/*************************************************************************/
/********************** Main initialization routine **********************/
/*************************************************************************/

/* Overall initialization routine.  Returns 0 on success, -1 on failure.
 * Never returns failure after forking / closing standard file descriptors.
 */

int init(int ac, char** av)
{
    int i;
    int started_from_term = isatty(0) && isatty(1) && isatty(2);

    /* Initialize memory log, to catch messages written before the log file
       is opened (if any). */
    open_memory_log();

    /* Account for runtime memory. */
    init_memory();

    /* Parse command-line options; exit if an error occurs. */
    if (parse_options(ac, av, 0) < 0)
        return -1;

    /* Chdir to Services data directory. */
    if (chdir(services_dir) < 0) {
        fprintf(stderr, "chdir(%s): %s\n", services_dir, strerror(errno));
        return -1;
    }

    /* Read configuration file; exit if there are problems. */
    if (!read_config())
        return -1;

    /* Re-parse command-line options (to override configuration file). */
    parse_options(ac, av, 0);

    /* Open logfile, and complain if we couldn't. */
    set_logfile(LogFilename);
    if (!open_log()) {
        if (started_from_term) {
            fprintf(stderr, "Warning: unable to open log file %s: %s\n",
                    LogFilename, strerror(errno));
        }
    }

    /* Announce ourselves to the logfile. */
    if (debug || readonly || noexpire) {
        log("IRC Services %s starting up (options:%s%s%s)", version_number,
            debug ? " debug" : "", readonly ? " readonly" : "",
            noexpire ? " noexpire" : "");
    }
    else {
        log("IRC Services %s starting up", version_number);
    }
    start_time = time(NULL);

    /* If in read-only mode, close the logfile again. */
    if (readonly)
        close_log();

    /* If DUMPCORE is set and the OS supports get/setrlimit(), then attempt
     * to remove, or at least raise to maximum, the core file size limit. */
#if DUMPCORE
    {
        struct rlimit rl = {RLIM_INFINITY, RLIM_INFINITY};
        if (setrlimit(RLIMIT_CORE, &rl) < 0) {
            log_perror("setrlimit(RLIMIT_CORE, RLIM_INFINITY)");
            if ((i = getrlimit(RLIMIT_CORE, &rl)) < 0 ||
                rl.rlim_cur >= rl.rlim_max) {
                if (i < 0)
                    log_perror("getrlimit(RLIMIT_CORE)");
                log("Unable to set core file size limit; core files %s.",
                    i < 0 || rl.rlim_cur == 0 ? "will not be generated"
                                              : "may be truncated");
            }
            else {
                rl.rlim_cur = rl.rlim_max;
                if (setrlimit(RLIMIT_CORE, &rl) < 0) {
                    log_perror("setrlimit(RLIMIT_CORE, %ld)",
                               (long)rl.rlim_cur);
                    log("Unable to set core file size limit; core files may"
                        " be truncated.");
                }
                else {
                    log("Core file size limited to %ldkB; core files may be"
                        " truncated.",
                        (long)(rl.rlim_cur < 1024 ? 1 : rl.rlim_cur / 1024));
                }
            }
        } /* if (setrlimit(...) < 0) */
    } /* setrlimit() block */
#endif

    /* Initialize pseudo-random number generator. */
    srand((unsigned)time(NULL) ^ (unsigned)getppid() ^
          (unsigned)getpid() << 16);

    /* Initialize socket system. */
    sock_set_buflimits(NetBufferSize, TotalNetBufferSize);
    sock_set_rto(ReadTimeout);

    /* Initialize the module system. */
    if (!module_system_init())
        return -1;

    /* Declare our (and main.c's and databases.c's) events. */
    command_line_event = event_declare(NULL, EVENT_COMMAND_LINE);
    uplink_linked_event = event_declare(NULL, EVENT_UPLINK_LINKED);
    save_complete_event = event_declare(NULL, EVENT_SAVE_COMPLETE);
    service_introduced_event = event_declare(NULL, EVENT_SERVICE_INTRODUCED);
    if (!command_line_event || !uplink_linked_event || !save_complete_event
        || !service_introduced_event) {
        log("init(): Unable to declare events");
        return -1;
    }

    /* Switch on the database and cache drivers, so that modules can use
     * db.h and cache.h from their `init' on.  Their connection
     * threads only start once Services have forked (see below). */
    if (!pg_driver_init() || !redis_driver_init()) {
        log("init(): Unable to register the database drivers");
        return -1;
    }

    /* Call other initialization routines.  These are mainly (right now
     * only) for adding callbacks.  database_init() opens the database, and
     * fails if it cannot be reached. */
    if (!user_init(ac, av) || !channel_init(ac, av) || !server_init(ac, av) ||
        !process_init(ac, av) || !messages_init(ac, av) ||
        !actions_init(ac, av) || !database_init(ac, av)) {
        return -1;
    }

    /* The entity store: Redis in front of PostgreSQL, for the modules'
     * records.  After the database (which it needs claimed), before the
     * modules (which register their types when they load). */
    if (!store_init()) {
        log("init(): Unable to start the entity store");
        return -1;
    }

    /* Initialize mode handling routines. */
    mode_setup();

    /* Initialize multi-language support. */
    if (!lang_init())
        return -1;
    log_debug(1, "Loaded languages");

    /* Set up the P10 protocol layer (modes, messages, callbacks). */
    if (!p10_init())
        return -1;

    /* Load modules. */
    ARRAY_FOREACH(i, LoadModules)
    {
        if (!module_load(LoadModules[i])) {
            log("Error loading modules, aborting");
            return -1;
        }
    }
    log_debug(1, "Loaded modules");
    event_report_undeclared();
    check_encryption();

    /* Load external language files (now that modules have had a chance to
     * add their own strings). */
    ARRAY_FOREACH(i, LoadLanguageText)
    load_ext_lang(LoadLanguageText[i]);

    /* Check command-line arguments in modules, and exit if a module directs
     * us to. */
    i = parse_options(ac, av, 1);
    if (i != 0) {
        if (i < 0) {
            cleanup();
            return -1;
        }
        else {
            save_all_dbtables();
            cleanup();
            exit(0);
        }
    }

    /* So far so good; let the user know everything is okay. */
    if (!nofork)
        fprintf(stderr, "Initialization successful, starting IRC Services.\n");

    /* Detach ourselves if requested. */
    if (!nofork) {
        if ((i = fork()) < 0) {
            perror("fork()");
            return -1;
        }
        else if (i != 0) {
#if MEMCHECKS
            /* Avoid a bogus "XXX bytes leaked on exit" message for the
             * parent. */
            uninit_memory();
#endif
            exit(0);
        }
        if (started_from_term) {
            close(0);
            close(1);
            close(2);
        }
        if (setpgid(0, 0) < 0) {
            perror("setpgid()");
            return -1;
        }
    }

    /*
     * Everything from here down needs to be done in the child process
     * (when forking).
     */

    /* Start the worker threads: the database and cache connections, table
     * saves, migrations.  Not before, because a thread does not survive a
     * fork; connections asked for while the modules were loading were held
     * until now (see worker_spawn_owned()). */
    if (!worker_init(WorkerThreads ? WorkerThreads : WORKER_DEFAULT_THREADS,
                     WorkerQueueMax ? WorkerQueueMax
                                    : WORKER_DEFAULT_QUEUE_MAX)) {
        log("init(): Unable to start the worker threads");
        return -1;
    }

    /* Write our PID to the PID file (if one is configured). */
    if (*PIDFilename && !write_pidfile())
        log_perror("Warning: cannot write to PID file %s", PIDFilename);

    /* Set up signal handlers. */
    init_signals();

    /* Connect to the remote server. */
    servsock = sock_new();
    if (!servsock)
        fatal_perror("Can't create server socket");
    sock_setcb(servsock, SCB_CONNECT, connect_callback);
    sock_setcb(servsock, SCB_DISCONNECT, disconnect_callback);
    if (conn(servsock, RemoteServer, RemotePort, LocalHost, LocalPort) < 0)
        fatal_perror("Can't connect to server (%s:%d)", RemoteServer,
                     RemotePort);
    log_debug(1, "Initiated connection to %s:%d", RemoteServer, RemotePort);

    /* Return success (connect_callback() will handle the rest). */
    return 0;
}

/*************************************************************************/
/**************************** Reconfiguration ****************************/
/*************************************************************************/

int reconfigure(void)
{
    char *old_RemoteServer, *old_RemotePassword, *old_LocalHost;
    int old_RemotePort, old_LocalPort;
    char *old_ServerName, *old_ServerDesc, *old_ServiceUser, *old_ServiceHost;
    char* old_ServicesChannel;
    char *old_LogFilename, *old_PIDFilename;
    char** old_LoadModules;
    int old_LoadModules_count;
    int LoadModules_insert; /* where to insert unloadable modules */
    int i, j;
    int retval = 1;

    /* First save any data that we need after re-reading the conf file */
    old_RemoteServer = sstrdup(RemoteServer);
    old_RemotePort = RemotePort;
    old_RemotePassword = sstrdup(RemotePassword);
    old_LocalHost = LocalHost ? sstrdup(LocalHost) : NULL;
    old_LocalPort = LocalPort;
    old_ServerName = sstrdup(ServerName);
    old_ServerDesc = sstrdup(ServerDesc);
    old_ServiceUser = sstrdup(ServiceUser);
    old_ServiceHost = sstrdup(ServiceHost);
    old_ServicesChannel = ServicesChannel ? sstrdup(ServicesChannel) : NULL;
    old_LogFilename = sstrdup(LogFilename);
    old_PIDFilename = sstrdup(PIDFilename);
    old_LoadModules = LoadModules;
    old_LoadModules_count = LoadModules_count;

    /* Re-read the configuration.  CONFIGURE_SET frees the current
     * LoadModules, so hand it over to old_LoadModules first; if the new
     * configuration is rejected, nothing has changed and it goes back. */
    LoadModules = NULL;
    LoadModules_count = 0;
    if (!load_config()) {
        LoadModules = old_LoadModules;
        LoadModules_count = old_LoadModules_count;
        free(old_RemoteServer);
        free(old_RemotePassword);
        free(old_LocalHost);
        free(old_ServerName);
        free(old_ServerDesc);
        free(old_ServiceUser);
        free(old_ServiceHost);
        free(old_ServicesChannel);
        free(old_LogFilename);
        free(old_PIDFilename);
        return 0;
    }

    /* Deal with configuration changes */
    if (stricmp(RemoteServer, old_RemoteServer) != 0 ||
        RemotePort != old_RemotePort ||
        strcmp(RemotePassword, old_RemotePassword) != 0)
        log("warning: reconfigure: new uplink host/port/password will not"
            " take effect until restart");
    if ((!old_LocalHost && LocalHost) || (old_LocalHost && !LocalHost) ||
        (LocalHost && stricmp(LocalHost, old_LocalHost) != 0) ||
        LocalPort != old_LocalPort)
        log("warning: reconfigure: new uplink local_address will not take"
            " effect until restart");
    if (strcmp(ServerName, old_ServerName) != 0)
        log("warning: reconfigure: new serverinfo name will not take"
            " effect until restart");
    if (strcmp(ServerDesc, old_ServerDesc) != 0)
        log("warning: reconfigure: new serverinfo description will not"
            " take effect until restart");
    if ((!old_ServiceUser && ServiceUser) ||
        (!old_ServiceHost && ServiceHost) ||
        (ServiceUser && strcmp(ServiceUser, old_ServiceUser) != 0) ||
        (ServiceHost && strcmp(ServiceHost, old_ServiceHost) != 0))
        log("warning: reconfigure: new serverinfo user will not take"
            " effect until restart");
    /* The pseudoclients already on the old channel stay there, so the
     * ones introduced from now on (modules loaded later, a pseudoclient
     * brought back after a KILL) keep joining it too. */
    if ((!old_ServicesChannel) != (!ServicesChannel) ||
        (ServicesChannel && irc_stricmp(ServicesChannel,
                                        old_ServicesChannel) != 0)) {
        log("warning: reconfigure: new serverinfo channel will not take"
            " effect until restart");
        free(ServicesChannel);
        ServicesChannel = old_ServicesChannel;
        old_ServicesChannel = NULL; /* don't free it below */
    }
    if (strcmp(LogFilename, old_LogFilename) != 0) {
        log("reconfigure: log file changed, closing log file");
        set_logfile(LogFilename);
        if (reopen_log()) {
            log("reconfigure: log file changed, writing to new log file");
        }
        else {
            log("warning: reconfigure: unable to open new log file `%s',"
                " reverting to old file `%s'",
                LogFilename, old_LogFilename);
            free(LogFilename);
            LogFilename = old_LogFilename;
            old_LogFilename = NULL; /* don't free it below */
        }
    }
    if (strcmp(PIDFilename, old_PIDFilename) != 0) {
        if (!*PIDFilename) {
            remove(old_PIDFilename);
        }
        else if (write_pidfile()) {
            /* Successfully wrote the new PID file, so delete the old one */
            remove(old_PIDFilename);
        }
        else {
            log("warning: reconfigure: unable to write new PID file `%s',"
                " reverting to old file `%s'",
                PIDFilename, old_PIDFilename);
            strbcpy(PIDFilename, old_PIDFilename);
        }
    }

    /* Reset language data, then reload any new language files */
    reset_ext_lang();
    ARRAY_FOREACH(i, LoadLanguageText)
    load_ext_lang(LoadLanguageText[i]);

    /* For modules, we need to:
     *    - first unload any modules which don't have loadmodule entries
     *          anymore--this has to be done in reverse order to avoid
     *          dependency problems;
     *    - next reconfigure any still-loaded modules (because newly-loaded
     *          modules in the next step may depend on the new settings);
     *    - finally load every module that has a loadmodule entry and is
     *          not loaded (new entries, and any a previous REHASH could
     *          not load).
     * A module that cannot be unloaded (another one requires it) does
     * not stop the other two steps.
     */
    LoadModules_insert = LoadModules_count;
    for (i = old_LoadModules_count - 1; i >= 0; i--) {
        ARRAY_SEARCH_PLAIN(LoadModules, old_LoadModules[i], strcmp, j);
        if (j >= LoadModules_count) {
            Module* mod = module_find(old_LoadModules[i]);
            if (!mod) {
                log("BUG: reconfigure: module `%s' not available",
                    old_LoadModules[i]);
                retval = 0;
            }
            else if (!module_unload(mod)) {
                log("warning: reconfigure: module `%s' could not be unloaded",
                    old_LoadModules[i]);
                ARRAY_INSERT(LoadModules, LoadModules_insert);
                LoadModules[LoadModules_insert] = sstrdup(old_LoadModules[i]);
                retval = 0;
            }
        }
    }
    if (!module_reconfigure_all()) {
        log("warning: reconfigure: module reconfiguration failed");
        retval = 0;
    }
    ARRAY_FOREACH(i, LoadModules)
    {
        if (module_find(LoadModules[i]))
            continue;
        if (!module_load(LoadModules[i])) {
            log("warning: reconfigure: module `%s' could not be loaded",
                LoadModules[i]);
            ARRAY_REMOVE(LoadModules, i);
            i--;
            retval = 0;
        }
    }

    check_encryption();

    /* Free old configuration data and return */
    free(old_RemoteServer);
    free(old_RemotePassword);
    free(old_LocalHost);
    free(old_ServerName);
    free(old_ServerDesc);
    free(old_ServiceUser);
    free(old_ServiceHost);
    free(old_ServicesChannel);
    free(old_LogFilename);
    free(old_PIDFilename);
    ARRAY_FOREACH(i, old_LoadModules)
    free(old_LoadModules[i]);
    free(old_LoadModules);
    return retval;
}

/*************************************************************************/
/******************************** Cleanup ********************************/
/*************************************************************************/

void cleanup(void)
{
    if (!*quitmsg)
        strbcpy(quitmsg, "Terminating, reason unknown");
    log("%s", quitmsg);
    set_cmode(NULL, NULL);
    /* Let the last save finish while its tables are still registered, and
     * write every record that changed. */
    database_flush();
    store_collect();
    store_flush_all();
    store_wait(db_conf_save_timeout());
    module_unload_all();
    store_collect();
    store_wait(db_conf_save_timeout());
    if (servsock) {
        if (sock_isconn(servsock)) {
            send_cmd(ServerName, "SQUIT %s :%s", ServerName, quitmsg);
            disconn(servsock);
        }
        sock_free(servsock);
    }
    lang_cleanup();
    /* The drivers stop their connections (failing whatever is still in
     * flight) before the worker threads go, and the database is closed
     * last, releasing this copy's claim on it. */
    store_shutdown();
    cache_shutdown();
    db_shutdown();
    migration_shutdown();
    worker_shutdown();
    database_cleanup();
    p10_cleanup();
    actions_cleanup();
    messages_cleanup();
    process_cleanup();
    server_cleanup();
    channel_cleanup();
    user_cleanup();
    event_retract(service_introduced_event);
    event_retract(save_complete_event);
    event_retract(uplink_linked_event);
    event_retract(command_line_event);
    service_introduced_event = NULL;
    save_complete_event = uplink_linked_event = command_line_event = NULL;
    module_system_cleanup();
    event_system_cleanup();
    conf_cleanup();
    close_log();
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
