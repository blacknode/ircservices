/* Unreal protocol module for IRC Services.
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
#include "language.h"
#include "messages.h"
#include "timeout.h"
#include "modules/operserv/operserv.h"
#include "modules/operserv/maskdata.h"
#include "modules/nickserv/nickserv.h"
#include "modules/chanserv/chanserv.h"

#define UNREAL_HACK  /* For SJOIN; see comments in sjoin.c */
#include "banexcept.c"
#include "chanprot.c"
#include "halfop.c"
#include "invitemask.c"
#include "sjoin.c"
#include "svsnick.c"
#include "token.c"

/*************************************************************************/

/* Variables for modules referenced by this one (set at initialization or
 * within the load-module callback): */

static Module *module_operserv;
static Module *module_chanserv;


/* Variables set by configuration directives: */

/* Numeric to send, 0 for none */
static int32 ServerNumeric = 0;
/* Should we use TSCTL SVSTIME to set server timestamps? */
static int SetServerTimes = 0;
/* Frequency to send TSCTL SVSTIME at (0 = startup only) */
static time_t SVSTIMEFrequency = 0;
/* Timeout for sending SVSTIME */
static Timeout *to_svstime = NULL;


/* The following flag sets are set by init_modes().  We could have done
 * this using #defines as well, but this way is cleaner (there's no need
 * to make sure these values are synchronized with the list of modes
 * below). */
static int32 usermode_admin = 0;        /* +aANT */
static int32 usermode_secure = 0;       /* +z */
static int32 usermode_hiding = 0;       /* +I */
static int32 chanmode_admins_only = 0;  /* +A */
static int32 chanmode_secure_only = 0;  /* +z */
static int32 chanmode_no_hiding = 0;    /* +H */

/* This variable holds the version of the Unreal protocol the remote server
 * is using, extracted from its SERVER message. */
static int unreal_version = 0;

/* Does the remote server support NICKIP? */
static int has_nickip = 0;

/*************************************************************************/
/****************** Local interface to external symbols ******************/
/*************************************************************************/

/*
 * We import `s_ChanServ' from the "chanserv/main" module; this variable
 * holds a pointer to `s_ChanServ', obtained via get_module_symbol().  When
 * the ChanServ module is not loaded, this holds a pointer to `ServerName'
 * instead as a default value.
 *
 * Note that we use a #define here to substitute `*p_s_ChanServ' for
 * `s_ChanServ', so subsequent code can simply use `s_ChanServ' as if it
 * were declared normally.
 */

static char **p_s_ChanServ = &ServerName;
#define s_ChanServ (*p_s_ChanServ)

/*************************************************************************/

/*
 * We also import `is_services_admin' from the "operserv/main" module.
 * As with `s_ChanServ', the value is obtained via get_module_symbol(),
 * and defaults to NULL when the module is not loaded.  We access the
 * function via a wrapper (local_is_services_admin()), which checks
 * whether the function pointer is non-NULL before calling it, and returns
 * a default value if the pointer is NULL (in this case, if the OperServ
 * module is not loaded then we assume no one is a Services admin and
 * return false).
 *
 * Here, too, we use a #define to substitute `local_is_services_admin' for
 * `is_services_admin', so we can use the latter as if it were declared
 * normally.
 */

static int (*p_is_services_admin)(User *u) = NULL;

static int local_is_services_admin(User *u)
{
    return p_is_services_admin && (*p_is_services_admin)(u);
}
#define is_services_admin local_is_services_admin

/*************************************************************************/
/************************** User/channel modes ***************************/
/*************************************************************************/

/*
 * Here we define modes specific to this IRC protocol.  To simplify the
 * initialization code, we define each category of modes (user modes,
 * channel modes, and channel user modes) using the structure below, which
 * holds a mode character (typed as uint8 because it is used as an array
 * index) and a ModeData structure (defined in modes.h).
 */

struct modedata_init {
    uint8 mode;
    ModeData data;
};

/*
 * We also define new MI_ flags for certain modes; these are used by the
 * initialization code to set up the usermode_* and chanmode_* variables
 * above.  These flags must use only the bits defined in modes.h for local
 * use (MI_LOCAL_FLAGS, 0xFF000000).
 *
 * Note that the user mode flags and channel mode flags overlap; this is
 * okay, since they will never be used together.
 */

#define MI_ADMIN        0x01000000  /* Usermode given to admins */
#define MI_SECURE       0x02000000  /* Usermode for users on secure conn's */
#define MI_HIDING       0x04000000  /* Usermode for hiding users */
#define MI_ADMINS_ONLY  0x01000000  /* Chanmode for admin-only channels */
#define MI_SECURE_ONLY  0x02000000  /* Chanmode for secure-user-only chans */
#define MI_NO_HIDING    0x04000000  /* Chanmode for no-hiding channels */

/*************************************************************************/

/* New user modes: */

static const struct modedata_init new_usermodes[] = {
    {'g', {0x00000008}},        /* Receive globops */
    {'h', {0x00000010}},        /* Helpop */
    {'r', {0x00000020,0,0,0,MI_REGISTERED}},
                                /* Registered nick */
    {'a', {0x00000040,0,0,0,MI_ADMIN}},
                                /* Services admin */
    {'A', {0x00000080,0,0,0,MI_ADMIN}},
                                /* Server admin */
    {'N', {0x00000100,0,0,0,MI_ADMIN}},
                                /* Network admin */
    {'T', {0x00000200,0,0,0,MI_ADMIN}},
                                /* Technical admin */
    {'x', {0x00000400}},        /* Mask hostname */
    {'S', {0x00000800}},        /* Services client */
    {'q', {0x00001000}},        /* Cannot be kicked from chans */
    {'I', {0x00002000,0,0,0,MI_HIDING}},
                                /* Completely invisible (joins etc) */
    {'d', {0x00004000}},        /* Deaf */
    {'z', {0x00008000,0,0,0,MI_SECURE}},
                                /* Using secure connection */
};

/*************************************************************************/

/* New channel modes.  Note that since earlier versions of the `version4'
 * database module used raw bitmasks rather than mode strings to store
 * ChanServ mode locks, the mode flag values below must not be changed.
 * This restriction will be lifted when the version 4 database format is
 * obsoleted, hopefully sometime around version 6.0.
 */

static const struct modedata_init new_chanmodes[] = {
    {'R', {0x00000100,0,0,0,MI_REGNICKS_ONLY}},
                                /* Only identified users can join */
    {'r', {0x00000200,0,0,0,MI_REGISTERED}},
                                /* Set for all registered channels */
    {'c', {0x00000400,0,0}},    /* No ANSI colors in channel */
    {'O', {0x00000800,0,0,0,MI_OPERS_ONLY}},
                                /* Only opers can join channel */
    {'A', {0x00001000,0,0,0,MI_OPERS_ONLY|MI_ADMINS_ONLY}},
                                /* Only admins can join channel */
    {'z', {0x00002000,0,0,0,MI_SECURE_ONLY}},
                                /* Only secure (+z) users allowed */
    {'Q', {0x00004000,0,0}},    /* No kicks */
    {'K', {0x00008000,0,0}},    /* No knocks */
    {'V', {0x00010000,0,0}},    /* No invites */
    {'H', {0x00020000,0,0,0,MI_NO_HIDING}},
                                /* No hiding (+I) users */
    {'C', {0x00040000,0,0}},    /* No CTCPs */
    {'N', {0x00080000,0,0}},    /* No nick changing */
    {'S', {0x00100000,0,0}},    /* Strip colors */
    {'G', {0x00200000,0,0}},    /* Strip "bad words" */
    {'u', {0x00400000,0,0}},    /* Auditorium mode */
    {'f', {0x00800000,1,0}},    /* Flood limit */
    {'L', {0x01000000,1,0}},    /* Channel link */
    {'M', {0x02000000,0,0}},    /* Moderated to unregged nicks */
    {'T', {0x04000000,0,0}},    /* Disable notices to channel */
    {'j', {0x08000000,1,0}},    /* Join throttling */
    {'e', {0x80000000,1,1,0,MI_MULTIPLE}}, /* Ban exceptions */
    {'I', {0x80000000,1,1,0,MI_MULTIPLE}}, /* INVITE hosts */
};

/*************************************************************************/

/* New channel user modes: */

static const struct modedata_init new_chanusermodes[] = {
    {'h', {0x00000004,1,1,'%'}},        /* Half-op */
    {'a', {0x00000008,1,1,'~'}},        /* Protected (no kick or deop by +o) */
    {'q', {0x00000010,1,1,'*'}},        /* Channel owner */
};

/*************************************************************************/

/*
 * This routine inserts the data listed above into the global usermodes[],
 * chanmodes[], and chanusermodes[] arrays, initializes the local mode
 * variables (usermode_* and chanmode_*), and calls mode_setup().
 */

static void init_modes(void)
{
    int i;

    for (i = 0; i < lenof(new_usermodes); i++) {
        usermodes[new_usermodes[i].mode] = new_usermodes[i].data;
        if (new_usermodes[i].data.info & MI_ADMIN)
            usermode_admin |= new_usermodes[i].data.flag;
        if (new_usermodes[i].data.info & MI_SECURE)
            usermode_secure |= new_usermodes[i].data.flag;
        if (new_usermodes[i].data.info & MI_HIDING)
            usermode_hiding |= new_usermodes[i].data.flag;
    }
    for (i = 0; i < lenof(new_chanmodes); i++) {
        chanmodes[new_chanmodes[i].mode] = new_chanmodes[i].data;
        if (new_chanmodes[i].data.info & MI_ADMINS_ONLY)
            chanmode_admins_only |= new_chanmodes[i].data.flag;
        if (new_chanmodes[i].data.info & MI_SECURE_ONLY)
            chanmode_secure_only |= new_chanmodes[i].data.flag;
        if (new_chanmodes[i].data.info & MI_NO_HIDING)
            chanmode_no_hiding |= new_chanmodes[i].data.flag;
    }
    for (i = 0; i < lenof(new_chanusermodes); i++)
        chanusermodes[new_chanusermodes[i].mode] = new_chanusermodes[i].data;

    mode_setup();
};

/*************************************************************************/
/************************* IRC message receiving *************************/
/*************************************************************************/

/* Handler for the NICK message. */

static void m_nick(char *source, int ac, char **av)
{
    char *ipaddr;
    char *newav[11];

    if (*source) {
        /* Old user changing nicks. */
        if (ac != 2) {
            module_log_debug(1, "NICK message: wrong number of parameters"
                             " (%d) for source `%s'", ac, source);
        } else {
            do_nick(source, ac, av);
        }
        return;
    }

    /*
     * New user.  Unreal's NICK message takes 10 parameters (11 for servers
     * supporting NICKIP):
     *
     *     NICK <nick> <hopcount> <timestamp> <username> <hostname>
     *          <server> <servicestamp> <modes> <fakehost> [<IP>] :<realname>
     *
     * Processing consists of:
     *     - Making sure the message has the right number of parameters
     *     - Rearranging parameters for use by do_nick()
     *     - Calling do_nick()
     *
     * In previous versions of Services, we also had to assign a Services
     * stamp (if the remote server gave us a stamp of zero) and set the
     * user's modes; these are now handled by do_nick in users.c, so we
     * just pass along what we got.
     */

    if (ac != (has_nickip ? 11 : 10)) {
        module_log_debug(1, "NICK message: wrong number of parameters (%d)"
                         " for new user", ac);
        return;
    }

    /* If this is a NICKIP server, decode and save the IP address, then
     * strip it out of the parameter list so that the code below sees a
     * standard set of parameters regardless of NICKIP support. */
    if (has_nickip) {
        if (strcmp(av[9],"*") == 0) {
            ipaddr = "0.0.0.0";
        } else {
            unsigned char tmpbuf[16];
            const char *badip = NULL;  /* error message, or NULL if no error */
            int len = decode_base64(av[9], tmpbuf, sizeof(tmpbuf));
            if (len < 0 || len > sizeof(tmpbuf)) {
                badip = "Corrupt IP address";
            } else if (len == 4) {
                ipaddr = unpack_ip(tmpbuf);
                if (!ipaddr)
                    badip = "Internal error decoding IPv4 address";
            } else if (len == 16) {
                ipaddr = unpack_ip6(tmpbuf);
                if (!ipaddr)
                    badip = "Internal error decoding IPv6 address";
            } else {
                badip = "Unrecognized IP address format";
            }
            if (badip) {
                static int warned_no_nickip = 0;
                if (!warned_no_nickip) {
                    wallops(NULL, "\2WARNING\2: %s for new nick %s", badip,
                            av[0]);
                    warned_no_nickip = 1;
                }
                module_log("WARNING: %s for new nick %s", badip, av[0]);
                ipaddr = "0.0.0.0";
            }
            av[9] = av[10];
        }
    } else {
        ipaddr = NULL;
    }

    /* Set up parameter list for do_nick():
     *     (current)     (desired)
     *  0: nick          nick
     *  1: hopcount      hopcount
     *  2: timestamp     timestamp
     *  3: username      username
     *  4: hostname      hostname
     *  5: server        server
     *  6: servicestamp  realname
     *  7: modes         servicestamp
     *  8: fakehost      ipaddr
     *  9: realname      modes
     * 10: ---           fakehost
     */
    newav[ 0] = av[0];
    newav[ 1] = av[1];
    newav[ 2] = av[2];
    newav[ 3] = av[3];
    newav[ 4] = av[4];
    newav[ 5] = av[5];
    newav[ 6] = av[9];
    newav[ 7] = av[6];
    newav[ 8] = ipaddr;
    newav[ 9] = av[7];
    newav[10] = av[8];

    /* Record the new nick */
    do_nick(source, 11, newav);
}

/*************************************************************************/

/* PROTOCTL message handler. */

static void m_protoctl(char *source, int ac, char **av)
{
    static int got_nickv2 = 0;
    int i;

    for (i = 0; i < ac; i++) {
        if (stricmp(av[i], "NICKv2") == 0)
            got_nickv2 = 1;
        if (stricmp(av[i], "NOQUIT") == 0)
            protocol_features |= PF_NOQUIT;
        if (stricmp(av[i], "NICKIP") == 0)
            has_nickip = 1;
        if (strnicmp(av[i], "NICKCHARS=", 10) == 0) {
            /* Character sets taken from Unreal 3.2.3 */
            char *s;
            for (s = strtok(av[i]+10, ","); s; s = strtok(NULL, ",")) {
                if (strcmp(s, "cat") == 0) {
                    valid_nick_table[0xC0] |= 3;
                    valid_nick_table[0xC8] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xCF] |= 3;
                    valid_nick_table[0xD2] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xDA] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xE0] |= 3;
                    valid_nick_table[0xE8] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xEF] |= 3;
                    valid_nick_table[0xF2] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xFA] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "chi-j") == 0) {
                    int c;
                    for (c = 0xA4; c <= 0xA5; c++)
                        valid_nick_table[c] |= 0x04;
                    for (c = 0xA1; c <= 0xFE; c++)
                        valid_nick_table[c] |= 0x08;
                    memset(valid_nick_table+0xA4A1, 0x03, 0xA4F3-0xA4A1+1);
                    memset(valid_nick_table+0xA5A1, 0x03, 0xA5F6-0xA4A1+1);
                } else if (strcmp(s, "chi-s") == 0) {
                    int c;
                    for (c = 0xB0; c <= 0xF7; c++)
                        valid_nick_table[c] |= 0x04;
                    for (c = 0xA1; c <= 0xFE; c++)
                        valid_nick_table[c] |= 0x08;
                    for (c = 0xB0; c <= 0xF7; c++) {
                        memset(valid_nick_table+(c<<8)+0xA1, 0x03,
                               (c==0xD7 ? 0xF9 : 0xFE)-0xA1+1);
                    }
                } else if (strcmp(s, "chi-t") == 0) {
                    int c;
                    for (c = 0x81; c <= 0xA0; c++)
                        valid_nick_table[c] |= 0x04;
                    for (c = 0xAA; c <= 0xFE; c++)
                        valid_nick_table[c] |= 0x04;
                    for (c = 0x40; c <= 0x7E; c++)
                        valid_nick_table[c] |= 0x08;
                    for (c = 0x80; c <= 0xFE; c++)
                        valid_nick_table[c] |= 0x08;
                    for (c = 0x81; c <= 0xA0; c++) {
                        memset(valid_nick_table+(c<<8)+0x40, 0x03,
                               (0x7E)-0x40+1);
                        memset(valid_nick_table+(c<<8)+0x80, 0x03,
                               (0xFE)-0x80+1);
                    }
                    for (c = 0xAA; c <= 0xFE; c++) {
                        memset(valid_nick_table+(c<<8)+0x40, 0x03,
                               (0x7E)-0x40+1);
                        memset(valid_nick_table+(c<<8)+0x80, 0x03,
                               (0xA0)-0x80+1);
                    }
                } else if (strcmp(s, "cze-m") == 0) {
                    valid_nick_table[0x8A] |= 3;
                    valid_nick_table[0x8D] |= 3;
                    valid_nick_table[0x8E] |= 3;
                    valid_nick_table[0x9A] |= 3;
                    valid_nick_table[0x9D] |= 3;
                    valid_nick_table[0x9E] |= 3;
                    valid_nick_table[0xC1] |= 3;
                    valid_nick_table[0xC8] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCC] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xCF] |= 3;
                    valid_nick_table[0xD2] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xD8] |= 3;
                    valid_nick_table[0xD9] |= 3;
                    valid_nick_table[0xDA] |= 3;
                    valid_nick_table[0xDD] |= 3;
                    valid_nick_table[0xE1] |= 3;
                    valid_nick_table[0xE8] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xEC] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xEF] |= 3;
                    valid_nick_table[0xF2] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xF8] |= 3;
                    valid_nick_table[0xF9] |= 3;
                    valid_nick_table[0xFA] |= 3;
                    valid_nick_table[0xFD] |= 3;
                } else if (strcmp(s, "dut") == 0) {
                    valid_nick_table[0xE8] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xEB] |= 3;
                    valid_nick_table[0xEF] |= 3;
                    valid_nick_table[0xF6] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "fre") == 0) {
                    valid_nick_table[0xC0] |= 3;
                    valid_nick_table[0xC2] |= 3;
                    valid_nick_table[0xC7] |= 3;
                    valid_nick_table[0xC8] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCA] |= 3;
                    valid_nick_table[0xCB] |= 3;
                    valid_nick_table[0xCE] |= 3;
                    valid_nick_table[0xCF] |= 3;
                    valid_nick_table[0xD4] |= 3;
                    valid_nick_table[0xD9] |= 3;
                    valid_nick_table[0xDB] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xE0] |= 3;
                    valid_nick_table[0xE2] |= 3;
                    valid_nick_table[0xE7] |= 3;
                    valid_nick_table[0xE8] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xEA] |= 3;
                    valid_nick_table[0xEB] |= 3;
                    valid_nick_table[0xEE] |= 3;
                    valid_nick_table[0xEF] |= 3;
                    valid_nick_table[0xF4] |= 3;
                    valid_nick_table[0xF9] |= 3;
                    valid_nick_table[0xFB] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "ger") == 0) {
                    valid_nick_table[0xC4] |= 3;
                    valid_nick_table[0xD6] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xDF] |= 3;
                    valid_nick_table[0xE4] |= 3;
                    valid_nick_table[0xF6] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "gre") == 0) {
                    int c;
                    for (c = 0xB6; c <= 0xF4; c++) {
                        if (c!=0xB7 && c!=0xBB && c!=0xBD && c!=0xD2)
                            valid_nick_table[c] |= 3;
                    }
                } else if (strcmp(s, "heb") == 0) {
                    int c;
                    for (c = 0xE0; c <= 0xFE; c++)
                        valid_nick_table[c] |= 3;
                } else if (strcmp(s, "hun") == 0) {
                    valid_nick_table[0xC1] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xD5] |= 3;
                    valid_nick_table[0xD6] |= 3;
                    valid_nick_table[0xDA] |= 3;
                    valid_nick_table[0xDB] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xE1] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xF5] |= 3;
                    valid_nick_table[0xF6] |= 3;
                    valid_nick_table[0xFA] |= 3;
                    valid_nick_table[0xFB] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "ice") == 0) {
                    valid_nick_table[0xC1] |= 3;
                    valid_nick_table[0xC6] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xD0] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xD6] |= 3;
                    valid_nick_table[0xDA] |= 3;
                    valid_nick_table[0xDD] |= 3;
                    valid_nick_table[0xDE] |= 3;
                    valid_nick_table[0xE1] |= 3;
                    valid_nick_table[0xE6] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xF0] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xF6] |= 3;
                    valid_nick_table[0xFA] |= 3;
                    valid_nick_table[0xFD] |= 3;
                    valid_nick_table[0xFE] |= 3;
                } else if (strcmp(s, "ita") == 0) {
                    valid_nick_table[0xC0] |= 3;
                    valid_nick_table[0xC8] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCC] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xD2] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xD9] |= 3;
                    valid_nick_table[0xDA] |= 3;
                    valid_nick_table[0xE0] |= 3;
                    valid_nick_table[0xE8] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xEC] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xF2] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xF9] |= 3;
                    valid_nick_table[0xFA] |= 3;
                } else if (strcmp(s, "pol") == 0) {
                    valid_nick_table[0xA1] |= 3;
                    valid_nick_table[0xA3] |= 3;
                    valid_nick_table[0xA6] |= 3;
                    valid_nick_table[0xAC] |= 3;
                    valid_nick_table[0xAF] |= 3;
                    valid_nick_table[0xB1] |= 3;
                    valid_nick_table[0xB3] |= 3;
                    valid_nick_table[0xB6] |= 3;
                    valid_nick_table[0xBC] |= 3;
                    valid_nick_table[0xBF] |= 3;
                    valid_nick_table[0xC6] |= 3;
                    valid_nick_table[0xCA] |= 3;
                    valid_nick_table[0xD1] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xE6] |= 3;
                    valid_nick_table[0xEA] |= 3;
                    valid_nick_table[0xF1] |= 3;
                    valid_nick_table[0xF3] |= 3;
                } else if (strcmp(s, "pol-m") == 0) {
                    valid_nick_table[0xA5] |= 3;
                    valid_nick_table[0xA3] |= 3;
                    valid_nick_table[0x8C] |= 3;
                    valid_nick_table[0x8F] |= 3;
                    valid_nick_table[0xAF] |= 3;
                    valid_nick_table[0xB9] |= 3;
                    valid_nick_table[0xB3] |= 3;
                    valid_nick_table[0x9C] |= 3;
                    valid_nick_table[0x9F] |= 3;
                    valid_nick_table[0xBF] |= 3;
                    valid_nick_table[0xC6] |= 3;
                    valid_nick_table[0xCA] |= 3;
                    valid_nick_table[0xD1] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xE6] |= 3;
                    valid_nick_table[0xEA] |= 3;
                    valid_nick_table[0xF1] |= 3;
                    valid_nick_table[0xF3] |= 3;
                } else if (strcmp(s, "rum") == 0) {
                    valid_nick_table[0xAA] |= 3;
                    valid_nick_table[0xBA] |= 3;
                    valid_nick_table[0xC2] |= 3;
                    valid_nick_table[0xC3] |= 3;
                    valid_nick_table[0xCE] |= 3;
                    valid_nick_table[0xDE] |= 3;
                    valid_nick_table[0xE2] |= 3;
                    valid_nick_table[0xE3] |= 3;
                    valid_nick_table[0xEE] |= 3;
                    valid_nick_table[0xFE] |= 3;
                } else if (strcmp(s, "rus") == 0) {
                    int c;
                    valid_nick_table[0xA8] |= 3;
                    valid_nick_table[0xB8] |= 3;
                    for (c = 0xC0; c <= 0xFF; c++)
                        valid_nick_table[c] |= 3;
                } else if (strcmp(s, "slo-m") == 0) {
                    valid_nick_table[0x8A] |= 3;
                    valid_nick_table[0x8D] |= 3;
                    valid_nick_table[0x8E] |= 3;
                    valid_nick_table[0x9A] |= 3;
                    valid_nick_table[0x9D] |= 3;
                    valid_nick_table[0x9E] |= 3;
                    valid_nick_table[0xBC] |= 3;
                    valid_nick_table[0xBE] |= 3;
                    valid_nick_table[0xC0] |= 3;
                    valid_nick_table[0xC1] |= 3;
                    valid_nick_table[0xC4] |= 3;
                    valid_nick_table[0xC5] |= 3;
                    valid_nick_table[0xC8] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xCF] |= 3;
                    valid_nick_table[0xE0] |= 3;
                    valid_nick_table[0xE1] |= 3;
                    valid_nick_table[0xE4] |= 3;
                    valid_nick_table[0xE5] |= 3;
                    valid_nick_table[0xE8] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xEF] |= 3;
                    valid_nick_table[0xF2] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xF4] |= 3;
                    valid_nick_table[0xFA] |= 3;
                    valid_nick_table[0xFD] |= 3;
                } else if (strcmp(s, "spa") == 0) {
                    valid_nick_table[0xC1] |= 3;
                    valid_nick_table[0xC9] |= 3;
                    valid_nick_table[0xCD] |= 3;
                    valid_nick_table[0xD1] |= 3;
                    valid_nick_table[0xD3] |= 3;
                    valid_nick_table[0xDA] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xE1] |= 3;
                    valid_nick_table[0xE9] |= 3;
                    valid_nick_table[0xED] |= 3;
                    valid_nick_table[0xF1] |= 3;
                    valid_nick_table[0xF3] |= 3;
                    valid_nick_table[0xFA] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "swe") == 0) {
                    valid_nick_table[0xC4] |= 3;
                    valid_nick_table[0xC5] |= 3;
                    valid_nick_table[0xD6] |= 3;
                    valid_nick_table[0xE4] |= 3;
                    valid_nick_table[0xE5] |= 3;
                    valid_nick_table[0xF6] |= 3;
                } else if (strcmp(s, "swg") == 0) {
                    valid_nick_table[0xC4] |= 3;
                    valid_nick_table[0xD6] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xE4] |= 3;
                    valid_nick_table[0xF6] |= 3;
                    valid_nick_table[0xFC] |= 3;
                } else if (strcmp(s, "tur") == 0) {
                    valid_nick_table[0xC7] |= 3;
                    valid_nick_table[0xD0] |= 3;
                    valid_nick_table[0xD6] |= 3;
                    valid_nick_table[0xDC] |= 3;
                    valid_nick_table[0xDE] |= 3;
                    valid_nick_table[0xE7] |= 3;
                    valid_nick_table[0xF0] |= 3;
                    valid_nick_table[0xF6] |= 3;
                    valid_nick_table[0xFC] |= 3;
                    valid_nick_table[0xFD] |= 3;
                    valid_nick_table[0xFE] |= 3;
                } else {
                    send_error("Unknown NICKCHARS language: %s", s);
                    snprintf(quitmsg, sizeof(quitmsg),
                             "Unknown NICKCHARS language: %s", s);
                    quitting = 1;
                }
            }
        }  /* if NICKCHARS */
    }  /* for each parameter */

    /* Make sure we got a NICKv2 from the remote server; if not, abort. */
    if (!got_nickv2) {
        send_error("Need NICKv2 protocol");
        strbcpy(quitmsg, "Remote server doesn't support NICKv2");
        quitting = 1;
    }
}

/*************************************************************************/

/* UMODE2 (set user modes) message handler. */

static void m_umode2(char *source, int ac, char **av)
{
    char *new_av[2];

    if (ac != 1)
        return;
    new_av[0] = source;
    new_av[1] = av[0];
    do_umode(source, 2, new_av);
}

/*************************************************************************/

/* SETHOST/CHGHOST message handlers. */

static void m_sethost(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return;
    u = get_user(source);
    if (!u) {
        module_log("m_sethost: user record for %s not found", source);
        return;
    }
    free(u->fakehost);
    u->fakehost = sstrdup(av[0]);
}

static void m_chghost(char *source, int ac, char **av)
{
    if (ac == 2) {
        module_log_debug(1, "m_chghost from %s calling m_sethost for %s",
                         source, av[0]);
        m_sethost(av[0], ac-1, av+1);
    }
}

/*************************************************************************/

/* SETIDENT/CHGIDENT message handlers. */

static void m_setident(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return;
    u = get_user(source);
    if (!u) {
        module_log("m_setident: user record for %s not found", source);
        return;
    }
    free(u->username);
    u->username = sstrdup(av[0]);
}

static void m_chgident(char *source, int ac, char **av)
{
    if (ac == 2) {
        module_log_debug(1, "m_chgident from %s calling m_setident for %s",
                         source, av[0]);
        m_setident(av[0], ac-1, av+1);
    }
}

/*************************************************************************/

/* SETNAME/CHGNAME message handlers. */

static void m_setname(char *source, int ac, char **av)
{
    User *u;

    if (ac != 1)
        return;
    u = get_user(source);
    if (!u) {
        module_log("m_setname: user record for %s not found", source);
        return;
    }
    free(u->realname);
    u->realname = sstrdup(av[0]);
}

static void m_chgname(char *source, int ac, char **av)
{
    if (ac == 2) {
        module_log_debug(1, "m_chgname from %s calling m_setname for %s",
                         source, av[0]);
        m_setname(av[0], ac-1, av+1);
    }
}

/*************************************************************************/

/* SJOIN message handler.  The actual code is in sjoin.c (note that we use
 * UNREAL_HACK, so the routine actually called is do_sjoin_unreal()).
 */

static void m_sjoin(char *source, int ac, char **av)
{
    if (ac < 3) {
        module_log_debug(1, "SJOIN: expected >=3 params, got %d", ac);
        return;
    }
    do_sjoin(source, ac, av);
}

/*************************************************************************/

/* SVSMODE message handler. */

static void m_svsmode(char *source, int ac, char **av)
{
    if (*av[0] == '#') {
        if (ac >= 3 && strcmp(av[1],"-b") == 0) {
            Channel *c = get_channel(av[0]);
            User *u = get_user(av[2]);
            if (c && u)
                clear_channel(c, CLEAR_BANS, u);
        } else {
            module_log("Invalid SVS[2]MODE from %s for channel %s: %s",
                       source, av[0], merge_args(ac-1,av+1));
        }
    } else if (*av[0] == '&') {
        module_log("SVS[2]MODE from %s for invalid target (channel %s): %s",
                   source, av[0], merge_args(ac-1,av+1));
    } else {
        if (ac < 2)
            return;
        do_umode(source, ac, av);
    }
}

/*************************************************************************/

/* SVSNLINE message handler.  Format of an SVSNLINE message:
 *    SVSNLINE + reason_without_spaces :mask
 * or SVSNLINE - :mask
 */

static void m_svsnline(char *source, int ac, char **av)
{
    /* Remove SVSNLINE masks that are not in our database (see m_tkl()
     * below for an explanation) */

    typeof(get_maskdata) *p_get_maskdata = NULL;
    typeof(put_maskdata) *p_put_maskdata = NULL;

    if (ac < 3 || *av[0] != '+')
        return;
    if (!(p_get_maskdata = get_module_symbol(NULL, "get_maskdata")))
        return;
    if (!(p_put_maskdata = get_module_symbol(NULL, "put_maskdata")))
        return;
    if ((*p_put_maskdata)((*p_get_maskdata)(MD_SGLINE, av[2])))
        return;
    send_cmd(ServerName, "SVSNLINE - :%s", av[2]);
}

/*************************************************************************/

/* TKL message handler.  Format of a TKL message:
 *    TKL + type user host set-by expire-time set-time reason
 * or TKL - type user host removed-by
 */

static void m_tkl(char *source, int ac, char **av)
{
    /* If we get a TKL + that claims to have been set by Services, but we
     * don't have a matching mask in our database, send a TKL - for the
     * same mask to remove it.  This is needed in the case that a TKL is
     * removed by Services while one or more servers are split; in such a
     * case, the split server would, when it relinked, propogate the TKL
     * back out across the network even though it was no longer "valid"
     * from Services' perspective. */

    int type = -1;

    if (ac < 5 || *av[0] != '+' || strcmp(av[4],ServerName) != 0)
        return;
    switch (*av[1]) {
        case 'G': type = MD_AKILL;   break;
        case 'E': type = MD_EXCLUDE; break;
        case 'Q': type = MD_SQLINE;  break;
        case 'Z': type = MD_SZLINE;  break;
    }
    if (type != -1) {
        /* Pointer to the `get_maskdata' and `put_maskdata' functions
         * defined in the database module.  It would be more efficient to
         * store these statically, but doing so would require watching for
         * the database module to be unloaded, and as we should not
         * normally receive many TKL messages anyway, the additional CPU
         * burden is negligible. */
        typeof(get_maskdata) *p_get_maskdata = NULL;
        typeof(put_maskdata) *p_put_maskdata = NULL;
        if (!(p_get_maskdata = get_module_symbol(NULL, "get_maskdata")))
            return;
        if (!(p_put_maskdata = get_module_symbol(NULL, "put_maskdata")))
            return;
        if ((*p_put_maskdata)((*p_get_maskdata)(type, av[3])))
            return;
    }
    send_cmd(ServerName, "TKL - %c %s %s %s",
             *av[1], av[2], av[3], ServerName);
}

/*************************************************************************/

/* List of all Unreal-specific messages (handlers defined above). */

static Message unreal_messages[] = {
    { "AKILL",     NULL },
    { "CHGHOST",   m_chghost },
    { "CHGIDENT",  m_chgident },
    { "CHGNAME",   m_chgname },
    { "EOS",       NULL },
    { "GLOBOPS",   NULL },
    { "GNOTICE",   NULL },
    { "GOPER",     NULL },
    { "HELP",      NULL },
    { "NETINFO",   NULL },
    { "NICK",      m_nick },
    { "PROTOCTL",  m_protoctl },
    { "RAKILL",    NULL },
    { "SENDSNO",   NULL },
    { "SETHOST",   m_sethost },
    { "SETIDENT",  m_setident },
    { "SETNAME",   m_setname },
    { "SILENCE",   NULL },
    { "SJOIN",     m_sjoin },
    { "SMO",       NULL },
    { "SQLINE",    NULL },
    { "SVSMODE",   m_svsmode },
    { "SVS2MODE",  m_svsmode },
    { "SVSNLINE",  m_svsnline },
    { "SWHOIS",    NULL },
    { "TKL",       m_tkl },
    { "UMODE2",    m_umode2 },
    { NULL }
};

/*************************************************************************/

/* List of all Unreal tokens (passed to init_token()). */

static TokenInfo tokens[] = {
    /* 0x21 */
    { "!",  "PRIVMSG" },
    { "\"", "WHO" },
    { "#",  "WHOIS" },
    { "$",  "WHOWAS" },
    { "%",  "USER" },
    { "&",  "NICK" },
    { "'",  "SERVER" },
    { "(",  "LIST" },
    { ")",  "TOPIC" },
    { "*",  "INVITE" },
    { "+",  "VERSION" },
    { ",",  "QUIT" },
    { "-",  "SQUIT" },
    { ".",  "KILL" },
    { "/",  "INFO" },

    /* 0x30 */
    { "0",  "LINKS" },
    { "1",  "SUMMON" },
    { "2",  "STATS" },
    { "3",  "USERS" },
    { "4",  "HELP" },
    { "5",  "ERROR" },
    { "6",  "AWAY" },
    { "7",  "CONNECT" },
    { "8",  "PING" },
    { "9",  "PONG" },
    { ":",  NULL },
    { ";",  "OPER" },
    { "<",  "PASS" },
    { "=",  "WALLOPS" },
    { ">",  "TIME" },
    { "?",  "NAMES" },

    /* 0x40 */
    { "@",  "ADMIN" },
    { "A",  NULL },
    { "B",  "NOTICE" },
    { "C",  "JOIN" },
    { "D",  "PART" },
    { "E",  "LUSERS" },
    { "F",  "MOTD" },
    { "G",  "MODE" },
    { "H",  "KICK" },
    { "I",  "SERVICE" },
    { "J",  "USERHOST" },
    { "K",  "ISON" },
    { "L",  NULL },
    { "M",  NULL },
    { "N",  NULL },
    { "O",  "REHASH" },

    /* 0x50 */
    { "P",  "RESTART" },
    { "Q",  "CLOSE" },
    { "R",  "DIE" },
    { "S",  "HASH" },
    { "T",  "DNS" },
    { "U",  "SILENCE" },
    { "V",  "AKILL" },
    { "W",  "KLINE" },
    { "X",  "UNKLINE" },
    { "Y",  "RAKILL" },
    { "Z",  "GNOTICE" },
    { "[",  "GOPER" },
    { "\\", NULL },
    { "]",  "GLOBOPS" },
    { "^",  "LOCOPS" },
    { "_",  "PROTOCTL" },

    /* 0x60 */
    { "`",  "WATCH" },
    { "a",  NULL },
    { "b",  "TRACE" },
    { "c",  "SQLINE" },
    { "d",  "UNSQLINE" },
    { "e",  "SVSNICK" },
    { "f",  "SVSNOOP" },
    { "g",  "IDENTIFY" },
    { "h",  "SVSKILL" },
    { "i",  "NICKSERV" },
    { "j",  "CHANSERV" },
    { "k",  "OPERSERV" },
    { "l",  "MEMOSERV" },
    { "m",  "SERVICES" },
    { "n",  "SVSMODE" },
    { "o",  "SAMODE" },

    /* 0x70 */
    { "p",  "CHATOPS" },
    { "q",  "ZLINE" },
    { "r",  "UNZLINE" },
    { "s",  "HELPSERV" },
    { "t",  "RULES" },
    { "u",  "MAP" },
    { "v",  "SVS2MODE" },
    { "w",  "DALINFO" },
    { "x",  "ADCHAT" },
    { "y",  "MKPASSWD" },
    { "z",  "ADDLINE" },
    { "{",   NULL },
    { "|",  "UMODE2" },
    { "}",  "GLINE" },
    { "~",  "SJOIN" },

    /* 0x41xx */
    { "AA", "SETHOST" },
    { "AB", "TECHAT" },
    { "AC", "NACHAT" },
    { "AD", "SETIDENT" },
    { "AE", "SETNAME" },
    { "AF", "LAG" },
    { "AG", "SDESC" },
    { "AH", "STATSERV" },
    { "AI", "KNOCK" },
    { "AJ", "CREDITS" },
    { "AK", "LICENSE" },
    { "AL", "CHGHOST" },
    { "AM", "RPING" },
    { "AN", "RPONG" },
    { "AO", "NETINFO" },
    { "AP", "SENDUMODE" },
    { "AQ", "ADDMOTD" },
    { "AR", "ADDOMOTD" },
    { "AS", "SVSMOTD" },
    { "AT", NULL },
    { "AU", "SMO" },
    { "AV", "OPERMOTD" },
    { "AW", "TSCTL" },
    { "AX", "SAJOIN" },
    { "AY", "SAPART" },
    { "AZ", "CHGIDENT" },

    /* 0x42xx */
    { "BA", "SWHOIS" },
    { "BB", "SVSO" },
    { "BC", "SVSFLINE" },
    { "BD", "TKL" },
    { "BE", "VHOST" },
    { "BF", "BOTMOTD" },
    { "BG", "REMGLINE" },
    { "BH", "HTM" },
    { "BI", "DCCDENY" },
    { "BJ", "UNDCCDENY" },
    { "BK", "CHGNAME" },  /* also SVSNAME */
    { "BL", "SHUN" },
    { "BM", NULL },
    { "BN", "POST" },
    { "BO", "INFOSERV" },
    { "BP", "CYCLE" },
    { "BQ", "MODULE" },
    { "BR", "SVSJOIN" },
    { "BS", "BOTSERV" },
    { "BT", "SVSPART" },

    /* 0x45xx */
    { "ES", "EOS" },

    /* 0x53xx */
    { "Ss", "SENDSNO" },

    /* Fencepost */
    { NULL }
};

/*************************************************************************/
/************************** IRC message sending **************************/
/*************************************************************************/

/* Send a TSCTL SVSTIME command (internal use). */

static void do_send_svstime(Timeout *to)
{
    send_cmd(ServerName, "TSCTL SVSTIME %ld", (long)time(NULL));
}

/*************************************************************************/

/* Send a NICK command for a new user. */

static void do_send_nick(const char *nick, const char *user, const char *host,
                         const char *server, const char *name,
                         const char *modes)
{
    /* NICK <nick> <hops> <TS> <user> <host> <server> <svsid> <mode>
     *      <fakehost> [<IP>] :<ircname>
     * The <IP> parameter is only included if the remote server specified
     * NICKIP in its PROTOCTL line.
     *
     * Note that if SETHOST has not been used, <fakehost> is <host> for VHP
     * servers but "*" for others.  We send <host> because that works in
     * both cases.
     */
    send_cmd(NULL, "NICK %s 1 %ld %s %s %s 0 +%s %s%s :%s", nick,
             (long)time(NULL), user, host, server, modes, host,
             has_nickip ? " *" : "", name);
}

/*************************************************************************/

/* Send a NICK command to change an existing user's nick. */

static void do_send_nickchange(const char *nick, const char *newnick)
{
    send_cmd(nick, "NICK %s %ld", newnick, (long)time(NULL));
}

/*************************************************************************/

/* Send a command to change a user's "real name". */

static void do_send_namechange(const char *nick, const char *newname)
{
    send_cmd(nick, "SETNAME :%s", newname);
}

/*************************************************************************/

/* Send a SERVER command, and anything else needed at the beginning of the
 * connection.
 */

static void do_send_server(void)
{
    send_cmd(NULL, "PROTOCTL SJOIN SJOIN2 SJ3 NICKv2 VHP VL NOQUIT UMODE2"
             " TOKEN NICKIP");
    send_cmd(NULL, "PASS :%s", RemotePassword);
    /* Syntax:
     *     SERVER servername hopcount :U<protocol>-flags-numeric serverdesc
     * We use protocol 0, as we are protocol independent, and flags are *,
     * to prevent matching with version denying lines. */
    send_cmd(NULL, "SERVER %s 1 :U0-*-%d %s", ServerName, ServerNumeric,
             ServerDesc);
    /* If requested, send TSCTL SVSTIME and start timeout. */
    if (SetServerTimes) {
        do_send_svstime(NULL);
        if (SVSTIMEFrequency)
            to_svstime = add_timeout(SVSTIMEFrequency, do_send_svstime, 1);
    }
}

/*************************************************************************/

/* Send a SERVER command for a remote (juped) server. */

static void do_send_server_remote(const char *server, const char *reason)
{
    send_cmd(NULL, "SERVER %s 2 :%s", server, reason);
}

/*************************************************************************/

/* Send a WALLOPS (really a GLOBOPS). */

static void do_wallops(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    send_cmd(source ? source : ServerName, "GLOBOPS :%s", buf);
}

/*************************************************************************/

/* Send a NOTICE to all users on the network. */

static void do_notice_all(const char *source, const char *fmt, ...)
{
    va_list args;
    char buf[BUFSIZE];

    va_start(args, fmt);
    snprintf(buf, sizeof(buf), "NOTICE $* :%s", fmt);
    vsend_cmd(source, buf, args);
    va_end(args);
}

/*************************************************************************/

/* Send a command which modifies channel status. */

static void do_send_channel_cmd(const char *source, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsend_cmd(source, fmt, args);
    va_end(args);
}

/*************************************************************************/
/******************************* Callbacks *******************************/
/*************************************************************************/

/* Callback for messages received from the uplink server; we use this to
 * check for inappropriate servicestamp changes and reverse them.
 * Normally, this would be done with a user mode change callback instead,
 * but Unreal uses the +d mode without a parameter to mean "deaf", so we
 * have to check here for +d with a parameter.  We also watch for the
 * SERVER message from the uplink server to determine what protocol
 * version is in use.
 */

static int do_receive_message(char *source, char *cmd, int ac, char **av)
{
    if (((stricmp(cmd, "MODE") == 0 || strcmp(cmd, "G") == 0)
         && ac > 2 && *av[0] != '#' && strchr(av[1],'d'))
     || ((stricmp(cmd, "UMODE2") == 0 || strcmp(cmd, "|") == 0)
         && ac > 1 && strchr(av[0],'d'))
    ) {
        User *user = get_user(*cmd=='U' ? source : av[0]);
        if (user) {
            module_log("%s tried to change services stamp for %s",
                       cmd, user->nick);
            send_cmd(ServerName, "SVSMODE %s +d %u", user->nick,
                     user->servicestamp);
        }
    } else if (stricmp(cmd, "SERVER") == 0) {
        const char *s, *t;
        int ver;
        if (ac < 3) {
            module_log("SERVER: not enough parameters");
        } else if (*av[2] != 'U'
                   || !(s = strchr(av[2], '-'))
                   || (ver = strtoul(av[2]+1, (char **)&t, 10), t != s)
        ) {
            module_log("SERVER: bad/missing protocol ID");
        } else {
            /* Successfully parsed */
            unreal_version = ver;
        }
    }
    return 0;
}

/*************************************************************************/

/*
 * Callback for new users; we copy the fake hostname parameter from the
 * user argument area to the User structure.
 */

static int do_user_create(User *user, int ac, char **av)
{
    user->fakehost = sstrdup(av[10]);
    return 0;
}

/*************************************************************************/

/*
 * Callback for users who get assigned new Services stamps: send the change
 * out to the network (MODE +d <stamp>).
 */

static int do_user_servicestamp_change(User *user)
{
    send_cmd(ServerName, "SVSMODE %s +d %u", user->nick, user->servicestamp);
    return 0;
}

/*************************************************************************/

/*
 * User mode change callback; we check for inappropriate mode r/a changes
 * and reverse them, and give newly-opered identified Services admins +a.
 */

static int do_user_mode(User *user, int modechar, int add, char **av)
{
    switch (modechar) {
      case 'o':
        if (add) {
            /* Since is_services_admin() returns true only for opered
             * users, and +o is not yet actually set in the User structure,
             * we have to set it temporarily to get the information we
             * want.  We clear it again afterwards because another callback
             * may be expecting it to still be unset. */
            user->mode |= UMODE_o;
            if (user_identified(user) && is_services_admin(user))
                send_cmd(ServerName, "SVSMODE %s +a", user->nick);
            user->mode &= ~UMODE_o;
        }
        return 0;

      case 'r':
        if (user_identified(user)) {
            if (!add)
                send_cmd(ServerName, "SVSMODE %s +r", user->nick);
        } else {
            if (add)
                send_cmd(ServerName, "SVSMODE %s -r", user->nick);
        }
        return 1;

      case 'a':
        if (is_oper(user)) {
            if (is_services_admin(user)) {
                if (!add)
                    send_cmd(ServerName, "SVSMODE %s +a", user->nick);
            } else {
                if (add)
                    send_cmd(ServerName, "SVSMODE %s -a", user->nick);
            }
            return 1;
        }
    }
    return 0;
}

/*************************************************************************/

/*
 * Channel mode change callback; we handle modes L/f/j here.
 */

static int do_channel_mode(const char *source, Channel *channel,
                           int modechar, int add, char **av)
{
    int32 flag = mode_char_to_flag(modechar, MODE_CHANNEL);

    switch (modechar) {

      case 'L':
        free(channel->link);
        if (add) {
            channel->mode |= flag;
            channel->link = sstrdup(av[0]);
        } else {
            channel->mode &= ~flag;
            channel->link = NULL;
        }
        return 1;

      case 'f':
        free(channel->flood);
        if (add) {
            channel->mode |= flag;
            channel->flood = sstrdup(av[0]);
        } else {
            channel->mode &= ~flag;
            channel->flood = NULL;
        }
        return 1;

      case 'j':
        if (add) {
            int ok = 0;
            char *s;
            int joinrate1 = strtol(av[0], &s, 0);
            if (*s == ':') {
                int joinrate2 = strtol(s+1, &s, 0);
                if (!*s) {
                    if (joinrate1 && joinrate2) {
                        channel->mode |= flag;
                        channel->joinrate1 = joinrate1;
                        channel->joinrate2 = joinrate2;
                    } else {
                        channel->mode &= ~flag;
                        channel->joinrate1 = 0;
                        channel->joinrate2 = 0;
                        ok = 1;
                    }
                    ok = 1;
                }
            } else if (joinrate1 == 0) {
                channel->mode &= ~flag;
                channel->joinrate1 = 0;
                channel->joinrate2 = 0;
                ok = 1;
            }
            if (!ok) {
                module_log("warning: invalid MODE +j %s for %s", av[0],
                           channel->name);
            }
        } else {
            channel->mode &= ~flag;
            channel->joinrate1 = 0;
            channel->joinrate2 = 0;
        }
        return 1;

    } /* switch (mode) */

    return 0;
}

/*************************************************************************/

/* Callback to handle clearing bans and exceptions for clear_channel().
 * Normally, the versions in actions.c and modules/protocol/banexcept.c
 * will do the job, but Unreal has "extended ban types" which confuse the
 * usermask comparison, so we need to write our own versions of the
 * routines.  Hooray for feeping creaturism... */

static void unreal_clear_bans_excepts(const char *sender, Channel *chan,
                                      int what, const User *u);
static int do_clear_channel(const char *sender, Channel *chan, int what,
                            const void *param)
{
    if (what & (CLEAR_USERS | CLEAR_BANS | CLEAR_EXCEPTS)) {
        unreal_clear_bans_excepts(sender, chan, what,
                                  (what & CLEAR_USERS) ? NULL : param);
    }
    return 0;
}


static int unreal_match_ban(const char *ban, const User *u)
{
    if (*ban == '~'
     && ((ban[1] == '!' && ban[2] && ban[3] == ':')
      || (ban[1] && ban[2] == ':'))
    ) {
        if (ban[1] == '!')
            ban += 4;
        else
            ban += 3;
        if (ban[-2] == 'c') {
            /* Ignore channel user mode prefix, if any--too much of a
             * bother to parse */
            ban = strchr(ban, '#');
            if (!ban)
                return 0;
            struct u_chanlist *uc;
            for (uc = u->chans; uc; uc = uc->next) {
                if (irc_stricmp(uc->chan->name, ban) == 0)
                    return 1;
            }
            return 0;
        } else if (ban[-2] == 'r') {
            return match_wild_nocase(ban, u->realname);
        } else {
            return match_usermask(ban, u);
        }
    } else {
        return match_usermask(ban, u);
    }
}


static void unreal_clear_bans_excepts(const char *sender, Channel *chan,
                                      int what, const User *u)
{
    int i, count;
    char **list;

    if (what & (CLEAR_USERS | CLEAR_BANS)) {
        if (chan->bans_count) {
            count = chan->bans_count;
            list = smalloc(sizeof(char *) * count);
            memcpy(list, chan->bans, sizeof(char *) * count);
            for (i = 0; i < count; i++) {
                if (!u || unreal_match_ban(list[i], u))
                    set_cmode(sender, chan, "-b", list[i]);
            }
            free(list);
        }
    }

    if (what & (CLEAR_USERS | CLEAR_EXCEPTS)) {
        if (chan->excepts_count) {
            count = chan->excepts_count;
            list = smalloc(sizeof(char *) * count);
            memcpy(list, chan->excepts, sizeof(char *) * count);
            for (i = 0; i < count; i++) {
                if (!u || unreal_match_ban(list[i], u))
                    set_cmode(sender, chan, "-e", list[i]);
            }
            free(list);
        }
    }
}

/*************************************************************************/

/*
 * NickServ post-IDENTIFY callback; we set user mode +a for Services
 * admins if they're opered.
 */

static int do_nick_identified(User *u, int old_status)
{
    if (is_oper(u) && is_services_admin(u))
        send_cmd(ServerName, "SVSMODE %s +a", u->nick);
    return 0;
}

/*************************************************************************/

/*
 * Set-topic callback; we use this to adjust timestamps as necessary to
 * force a topic change.
 */

static int do_set_topic(const char *source, Channel *c, const char *topic,
                        const char *setter, time_t t)
{
    if (setter)
        return 0;
    if (t <= c->topic_time)
        t = c->topic_time + 1;  /* Force topic change */
    c->topic_time = t;
    send_cmd(source, "TOPIC %s %s %ld :%s", c->name, c->topic_setter,
             (long)c->topic_time, c->topic ? c->topic : "");
    return 1;
}

/*************************************************************************/

/*
 * ChanServ channel mode check callback; we handle setting modes +L, +f,
 * and +j. (-L/-f/-j are handled by the default handler, which just sends a
 * "MODE -L/-f/-j" as appropriate.)
 */

static int do_check_modes(Channel *c, ChannelInfo *ci, int add, int32 flag)
{
    if (add) {
        switch (mode_flag_to_char(flag, MODE_CHANNEL)) {

          case 'L':
            if (!ci->mlock.link) {
                module_log("warning: removing +L from channel %s mode lock"
                           " (missing parameter)", ci->name);
                ci->mlock.on &= ~mode_char_to_flag('L', MODE_CHANNEL);
            } else {
                if (!c->link || irc_stricmp(ci->mlock.link, c->link) != 0)
                    set_cmode(s_ChanServ, c, "+L", ci->mlock.link);
            }
            return 1;

          case 'f':
            if (!ci->mlock.flood) {
                module_log("warning: removing +f from channel %s mode lock"
                           " (missing parameter)", ci->name);
                ci->mlock.on &= ~mode_char_to_flag('f', MODE_CHANNEL);
            } else {
                if (!c->flood || irc_stricmp(ci->mlock.flood, c->flood) != 0)
                    set_cmode(s_ChanServ, c, "+f", ci->mlock.flood);
            }
            return 1;

          case 'j':
            if (sgn(ci->mlock.joinrate1) != sgn(ci->mlock.joinrate2)) {
                module_log("warning: removing +j from channel %s mode lock"
                           " (invalid parameter: %d:%d)", ci->name,
                           ci->mlock.joinrate1, ci->mlock.joinrate2);
                ci->mlock.on &= ~mode_char_to_flag('j', MODE_CHANNEL);
                ci->mlock.joinrate1 = ci->mlock.joinrate2 = 0;
            } else if (ci->mlock.joinrate1 < 0) {
                if (c->joinrate1 || c->joinrate2)
                    set_cmode(s_ChanServ, c, "-j");
            } else {
                if (c->joinrate1 != ci->mlock.joinrate1
                 || c->joinrate2 != ci->mlock.joinrate2
                ) {
                    char buf[BUFSIZE];
                    snprintf(buf, sizeof(buf), "%d:%d",
                             ci->mlock.joinrate1, ci->mlock.joinrate2);
                    set_cmode(s_ChanServ, c, "+j", buf);
                }
            }
            return 1;

        } /* switch (modechar) */
    } /* if (add) */
    return 0;
}

/*************************************************************************/

/*
 * Callback for ChanServ channel user mode checking; we prevent all
 * automatic mode changes for hiding (+I) users (since the mode changes
 * would give away their presence) or service pseudoclients (+S).
 */

static int do_check_chan_user_modes(const char *source, User *user,
                                    Channel *c, int32 modes)
{
    /* Don't do anything to service pseudoclients */
    if (user->mode & mode_char_to_flag('S', MODE_USER))
        return 1;

    /* Don't do anything to hiding users either */
    return ((user->mode & usermode_hiding) ? 1 : 0);
}

/*************************************************************************/

/*
 * Callback for ChanServ channel entrance checking; we enforce admin-only,
 * secure-only, and no-hiding channel modes here.  (This is necessary
 * despite checks made by the IRC server to handle the case of clients
 * entering empty channels with one of the above modes locked on, since
 * the servers don't know about the mode lock.)
 */

static int do_check_kick(User *user, const char *chan, ChannelInfo *ci,
                         char **mask_ret, const char **reason_ret)
{
    /* Retrieve the channel's Channel record, if present */
    Channel *c = get_channel(chan);

    /* Don't do anything to service pseudoclients */
    if (user->mode & mode_char_to_flag('S', MODE_USER))
        return 2;

    /* Don't let plain opers into +A (admin only) channels */
    if ((((c?c->mode:0) | (ci?ci->mlock.on:0)) & chanmode_admins_only)
     && !(user->mode & usermode_admin)
    ) {
        *mask_ret = create_mask(user, 1);
        *reason_ret = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        return 1;
    }
    /* Don't let users not on secure connections (-z) into +z channels */
    if ((((c?c->mode:0) | (ci?ci->mlock.on:0)) & chanmode_secure_only)
     && !(user->mode & usermode_secure)
    ) {
        *mask_ret = create_mask(user, 1);
        *reason_ret = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        return 1;
    }
    /* Don't let hiding users into no-hiding channels */
    if ((((c?c->mode:0) | (ci?ci->mlock.on:0)) & chanmode_no_hiding)
     && (user->mode & usermode_hiding)
    ) {
        *mask_ret = create_mask(user, 1);
        *reason_ret = getstring(user->ngi, CHAN_NOT_ALLOWED_TO_JOIN);
        return 1;
    }

    /* Let other processing continue as usual */
    return 0;
}

/*************************************************************************/

/*
 * ChanServ SET MLOCK callback; we handle locking modes f, j, K, and L.
 */

static int do_set_mlock(User *u, ChannelInfo *ci, int mode, int add, char **av)
{
    if (!mode) {
        /* Final check of new mode lock */
        if ((ci->mlock.on & mode_char_to_flag('K',MODE_CHANNEL))
         && !(ci->mlock.on & CMODE_i)
        ) {
            /* +K requires +i */
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_REQUIRES, 'K', 'i');
            return 1;
        }
        if (ci->mlock.link && !ci->mlock.limit) {
            /* +L requires +l */
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_REQUIRES, 'L', 'l');
            return 1;
        }
        return 0;
    }

    /* Single mode set/clear */

    if (add) {

        switch (mode) {

          case 'L':
            if (!valid_chan(av[0])) {
                /* Invalid channel name */
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_LINK_BAD, mode);
                return 1;
            }
            if (irc_stricmp(av[0], ci->name) == 0) {
                /* Trying to link to the same channel */
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_LINK_SAME, mode);
                return 1;
            }
            ci->mlock.link = sstrdup(av[0]);
            break;

          case 'f': {
            char *s, *t;
            /* Legal format for flood mode is:
             *    "digits:digits"
             * or "*digits:digits"
             * or "[TYPE,TYPE...]:digits"
             *    where TYPE is 1-999 followed by one of cjkmnt possibly
             *    followed by # and (a letter possibly followed by digits) */
            s = av[0];
            if (*s == '[') {
                int ok = 1;
                char c;
                s++;
                do {
                    if (!strchr("0123456789", *s)) {
                        ok = 0;
                        break;
                    }
                    t = s;
                    s += strspn(s, "0123456789");
                    c = *s;
                    *s = 0;
                    if (atoi(t) < 1 || atoi(t) > 999) {
                        ok = 0;
                        break;
                    }
                    *s = c;
                    if (!strchr("cjkmnt", *s++)) {
                        ok = 0;
                        break;
                    }
                    if (*s == '#') {
                        s++;
                        if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", *s++)) {
                            ok = 0;
                            break;
                        }
                        s += strspn(s, "0123456789");
                    }
                    if (*s != ',' && *s != ']')
                        ok = 0;
                    s++;
                } while (ok && s[-1] != ']');
                if (ok && *s++ != ':')
                    ok = 0;
                if (ok && s[strspn(s,"0123456789")] == 0) {
                    /* String is valid */
                    ci->mlock.flood = sstrdup(av[0]);
                    break;
                }
            } else {
                if (*s == '*')
                    s++;
                t = strchr(s, ':');
                if (t) {
                    t++;
                    if (s[strspn(s,"0123456789")] == ':'
                     && t[strspn(t,"0123456789")] == 0
                    ) {
                        /* String is valid */
                        ci->mlock.flood = sstrdup(av[0]);
                        break;
                    }
                }
            }
            /* String is invalid */
            notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_BAD_PARAM, mode);
            return 1;
          } /* case 'f' */

          case 'j': {
            int ok = 0;
            char *s;
            ci->mlock.joinrate1 = strtol(av[0], &s, 0);
            if (ci->mlock.joinrate1 > 0 && *s == ':') {
                ci->mlock.joinrate2 = strtol(s+1, &s, 0);
                if (ci->mlock.joinrate2 > 0 && !*s)
                    ok = 1;
            }
            if (!ok) {
                notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_BAD_PARAM, mode);
                return 1;
            }
            break;
          } /* case 'j' */

        } /* switch (mode) */

    } else {  /* !add -> lock off */

        switch (mode) {
          case 'j':
            ci->mlock.joinrate1 = ci->mlock.joinrate2 = -1;
            break;
        } /* switch (mode) */

    } /* if (add) */

    return 0;
}

/*************************************************************************/

/*
 * Callback for sending a forced join; required for SVSJOIN-supporting
 * protocols.
 */

static int do_send_svsjoin(const char *nick, const char *channel)
{
    send_cmd(ServerName, "SVSJOIN %s %s", nick, channel);
    return 1;
}

/*************************************************************************/

/*
 * Callback for sending autokills; required for autokill-supporting
 * protocols.
 */

static int do_send_akill(const char *username, const char *host,
                         time_t expires, const char *who, const char *reason)
{
    send_cmd(ServerName, "TKL + G %s %s %s %ld %ld :%s", username, host,
             ServerName, (long)expires, (long)time(NULL), reason);
    return 1;
}

/*************************************************************************/

/*
 * Callback for removing autokills; required for autokill-supporting
 * protocols.
 */

static int do_cancel_akill(const char *username, const char *host)
{
    send_cmd(ServerName, "TKL - G %s %s %s", username, host, ServerName);
    return 1;
}

/*************************************************************************/

/*
 * Callback for sending autokill exclusions; required for
 * exclusion-supporting protocols.
 */

static int do_send_exclude(const char *username, const char *host,
                           time_t expires, const char *who, const char *reason)
{
    send_cmd(ServerName, "TKL + E %s %s %s %ld %ld :%s", username, host,
             ServerName, (long)expires, (long)time(NULL), reason);
    return 1;
}

/*************************************************************************/

/*
 * Callback for removing autokill exclusions; required for
 * exclusion-supporting protocols.
 */

static int do_cancel_exclude(const char *username, const char *host)
{
    send_cmd(ServerName, "TKL - E * %s %s %s", username, host, ServerName);
    return 1;
}

/*************************************************************************/

/*
 * Callbacks for sending S-lines; required for S*LINE-supporting
 * protocols.
 */

static int do_send_sgline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    char buf[BUFSIZE], *s;

    s = buf;
    while (*reason && s-buf < sizeof(buf)-1) {
        if (*reason == ' ')
            *s = '_';
        else
            *s = *reason;
        reason++;
        s++;
    }
    *s = 0;
    send_cmd(ServerName, "SVSNLINE + %s :%s", buf, mask);
    return 1;
}

static int do_send_sqline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    send_cmd(ServerName, "TKL + Q * %s %s %ld %ld :%s", mask,
             ServerName, (long)expires, (long)time(NULL), reason);
    return 1;
}

static int do_send_szline(const char *mask, time_t expires, const char *who,
                          const char *reason)
{
    send_cmd(ServerName, "TKL + Z * %s %s %ld %ld :%s", mask,
             ServerName, (long)expires, (long)time(NULL), reason);
    return 1;
}

/*************************************************************************/

/*
 * Callbacks for removing S-lines; required for S*LINE-supporting
 * protocols.
 */

static int do_cancel_sgline(const char *mask)
{
    send_cmd(ServerName, "SVSNLINE - :%s", mask);
    return 1;
}

static int do_cancel_sqline(const char *mask)
{
    send_cmd(ServerName, "TKL - Q * %s %s", mask, ServerName);
    return 1;
}

static int do_cancel_szline(const char *mask)
{
    send_cmd(ServerName, "TKL - Z * %s %s", mask, ServerName);
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

/*
 * Configuration directives for this module (required--if the module has
 * no configuration directives, this consists of a single NULL entry).
 * Note that we include configuration directives for SJOIN which are
 * defined in sjoin.h.
 */
ConfigDirective module_config[] = {
    { "ServerNumeric",    { { CD_POSINT, 0, &ServerNumeric } } },
    { "SetServerTimes",   { { CD_SET, 0, &SetServerTimes },
                            { CD_TIME, CF_OPTIONAL, &SVSTIMEFrequency } } },
    SJOIN_CONFIG,
    { NULL }
};

/*************************************************************************/

/*
 * Load-module callback, used to look up symbols and add callbacks in
 * other modules.
 */

static int do_load_module(Module *mod, const char *modname)
{
    if (strcmp(modname, "operserv/main") == 0) {
        module_operserv = mod;
        p_is_services_admin = get_module_symbol(mod, "is_services_admin");
        if (!p_is_services_admin) {
            module_log("warning: unable to look up symbol `is_services_admin'"
                       " in module `operserv/main'");
        }
    } else if (strcmp(modname, "operserv/akill") == 0) {
        if (!add_callback(mod, "send_akill", do_send_akill))
            module_log("Unable to add send_akill callback");
        if (!add_callback(mod, "cancel_akill", do_cancel_akill))
            module_log("Unable to add cancel_akill callback");
        if (unreal_version >= 2303) {
            if (!add_callback(mod, "send_exclude", do_send_exclude))
                module_log("Unable to add send_exclude callback");
            if (!add_callback(mod, "cancel_exclude", do_cancel_exclude))
                module_log("Unable to add cancel_exclude callback");
        }
    } else if (strcmp(modname, "operserv/sline") == 0) {
        if (!add_callback(mod, "send_sgline", do_send_sgline))
            module_log("Unable to add send_sgline callback");
        if (!add_callback(mod, "send_sqline", do_send_sqline))
            module_log("Unable to add send_sqline callback");
        if (!add_callback(mod, "send_szline", do_send_szline))
            module_log("Unable to add send_szline callback");
        if (!add_callback(mod, "cancel_sgline", do_cancel_sgline))
            module_log("Unable to add cancel_sgline callback");
        if (!add_callback(mod, "cancel_sqline", do_cancel_sqline))
            module_log("Unable to add cancel_sqline callback");
        if (!add_callback(mod, "cancel_szline", do_cancel_szline))
            module_log("Unable to add cancel_szline callback");
    } else if (strcmp(modname, "nickserv/main") == 0) {
        if (!add_callback(mod, "identified", do_nick_identified))
            module_log("Unable to add NickServ identified callback");
    } else if (strcmp(modname, "nickserv/autojoin") == 0) {
        if (!add_callback(mod, "send_svsjoin", do_send_svsjoin))
            module_log("Unable to add NickServ send_svsjoin callback");
    } else if (strcmp(modname, "chanserv/main") == 0) {
        module_chanserv = mod;
        p_s_ChanServ = get_module_symbol(mod, "s_ChanServ");
        if (!p_s_ChanServ)
            p_s_ChanServ = &ServerName;
        if (!add_callback(mod, "check_modes", do_check_modes))
            module_log("Unable to add ChanServ check_modes callback");
        if (!add_callback(mod, "check_chan_user_modes",
                          do_check_chan_user_modes))
            module_log("Unable to add ChanServ check_chan_user_modes"
                       " callback");
        if (!add_callback(mod, "check_kick", do_check_kick))
            module_log("Unable to add ChanServ check_kick callback");
        if (!add_callback(mod, "SET MLOCK", do_set_mlock))
            module_log("Unable to add ChanServ SET MLOCK callback");
    }
    return 0;
}

/*************************************************************************/

/*
 * Unload-module callback, used to clear pointers to symbols in modules
 * about to be unloaded.
 */

static int do_unload_module(Module *mod)
{
    if (mod == module_operserv) {
        module_operserv = NULL;
        p_is_services_admin = NULL;
    } else if (mod == module_chanserv) {
        module_chanserv = NULL;
        p_s_ChanServ = &ServerName;
    }
    return 0;
}

/*************************************************************************/

/* Module initialization routine (required). */

int init_module(void)
{
    unsigned char c;


    /* Set protocol information variables. */
    protocol_name     = "Unreal";
    protocol_version  = "3.1.1+";
    protocol_features = PF_SZLINE | PF_SVSJOIN;
    protocol_nickmax  = 30;

    /* Make sure the ServerNumeric configuration setting is valid.  This
     * check is "<0" instead of "<1" because the numeric may not be
     * specified at all; conffile.c will check to ensure that 0 is not
     * given. */
    if (ServerNumeric < 0 || ServerNumeric > 254) {
        config_error(MODULES_CONF, 0,
                     "ServerNumeric must be in the range 1..254");
        return 0;
    }

    /* Register messages.  Note that this must be done before calling
     * init_token(), which depends on all needed messages being registered
     * (in order to look up message handlers for tokens). */
    if (!register_messages(unreal_messages)) {
        module_log("Unable to register messages");
        return 0;
    }

    /* Add callbacks. */
    if (!add_callback(NULL, "load module", do_load_module)
     || !add_callback(NULL, "unload module", do_unload_module)
     || !add_callback(NULL, "receive message", do_receive_message)
     || !add_callback(NULL, "user create", do_user_create)
     || !add_callback(NULL, "user servicestamp change",
                      do_user_servicestamp_change)
     || !add_callback(NULL, "user MODE", do_user_mode)
     || !add_callback(NULL, "channel MODE", do_channel_mode)
     || !add_callback(NULL, "clear channel", do_clear_channel)
     || !add_callback(NULL, "set topic", do_set_topic)
    ) {
        module_log("Unable to add callbacks");
        return 0;
    }

    /* Initialize subsystems in separate files. */
    if (!init_banexcept()
     || !init_chanprot()
     || !init_halfop()
     || !init_invitemask()
     || !init_sjoin()
     || !init_svsnick("SVSNICK")
     || !init_token(tokens)
    ) {
        return 0;
    }

    /* Initialize mode data. */
    init_modes();

    /* Adjust irc_stricmp() behavior (Unreal treats [ \ ] and { | } as
     * distinct, unlike RFC 1459). */
    irc_lowertable['['] = '[';
    irc_lowertable['\\'] = '\\';
    irc_lowertable[']'] = ']';

    /* Adjust valid_chan() behavior (Unreal disallows control characters
     * and the nonbreaking space 0xA0, and uses ':' as a "channel mask"
     * separator). */
    for (c = 0; c < 32; c++)
        valid_chan_table[c] = 0;
    valid_chan_table[':'] = 0;
    valid_chan_table[160] = 0;

    /* Set up function pointers and mode strings for send.c. */
    send_nick          = do_send_nick;
    send_nickchange    = do_send_nickchange;
    send_namechange    = do_send_namechange;
    send_server        = do_send_server;
    send_server_remote = do_send_server_remote;
    wallops            = do_wallops;
    notice_all         = do_notice_all;
    send_channel_cmd   = do_send_channel_cmd;
    pseudoclient_modes = "Sqd";
    enforcer_modes     = "d";
    pseudoclient_oper  = 0;

    /* Unreal has U:lines, so adjust the "bouncy modes" message. */
    mapstring(OPER_BOUNCY_MODES, OPER_BOUNCY_MODES_U_LINE);

    /* Return success. */
    return 1;
}

/*************************************************************************/

/*
 * Module cleanup routine (required).  We never permit ourselves to be
 * unloaded except during final shutdown; thus we don't worry about
 * restoring things to their initial state.
 */

int exit_module(int shutdown)
{
    if (!shutdown) {
        /* Do not allow removal of this module (see section 5.1 of the
         * technical manual) */
        return 0;
    }

    /* Clean things up in reverse order.  We don't worry about fixing up
     * irc_lowertable and so on because we're shutting down anyway;
     * however, we do need to make sure to at least remove all callbacks
     * before exiting, to avoid invalid function pointers being called
     * (e.g. for the unload module callback). */

    if (to_svstime) {
        del_timeout(to_svstime);
        to_svstime = NULL;
    }

    exit_token();
    exit_svsnick();
    exit_sjoin();
    exit_invitemask();
    exit_halfop();
    exit_chanprot();
    exit_banexcept();

    remove_callback(NULL, "set topic", do_set_topic);
    remove_callback(NULL, "clear channel", do_clear_channel);
    remove_callback(NULL, "channel MODE", do_channel_mode);
    remove_callback(NULL, "user MODE", do_user_mode);
    remove_callback(NULL, "user servicestamp change",
                    do_user_servicestamp_change);
    remove_callback(NULL, "user create", do_user_create);
    remove_callback(NULL, "receive message", do_receive_message);
    remove_callback(NULL, "unload module", do_unload_module);
    remove_callback(NULL, "load module", do_load_module);

    unregister_messages(unreal_messages);

    /* All finished, return success */
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
