/* Routines for handling mode flags and strings.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"

/*************************************************************************/

/* List of user/channel modes and associated mode letters. */

ModeData usermodes[] = {
    ['o'] = {0x00000001},
    ['i'] = {0x00000002},
    ['w'] = {0x00000004},
};

ModeData chanmodes[] = {
    ['i'] = {0x00000001, 0, 0},
    ['m'] = {0x00000002, 0, 0},
    ['n'] = {0x00000004, 0, 0},
    ['p'] = {0x00000008, 0, 0},
    ['s'] = {0x00000010, 0, 0},
    ['t'] = {0x00000020, 0, 0},
    ['k'] = {0x00000040, 1, 1},
    ['l'] = {0x00000080, 1, 0},
    ['b'] = {0x80000000, 1, 1, 0, MI_MULTIPLE},
};

ModeData chanusermodes[] = {
    ['o'] = {0x00000001, 1, 1, '@'},
    ['v'] = {0x00000002, 1, 1, '+'},
};

/* The following are initialized by mode_setup(): */
int32 usermode_reg;             /* Usermodes applied to registered nicks */
int32 chanmode_reg;             /* Chanmodes applied to registered chans */
int32 chanmode_regonly;         /* Chanmodes indicating regnick-only channels*/
int32 chanmode_opersonly;       /* Chanmodes indicating oper-only channels */
char  chanmode_multiple[257];   /* Chanmodes that can be set multiple times */

/* Flag tables, used internally to speed up flag lookups.  0 indicates a
 * flag with no associated mode. */
static char userflags[31], chanflags[31], chanuserflags[31];

/* Table of channel user mode prefixes, used like flag tables above. */
static int32 prefixtable[256];

/* Tables used for fast lookups. */
static ModeData *modetable[] = {usermodes, chanmodes, chanusermodes};
static char *flagtable[] = {userflags, chanflags, chanuserflags};

/*************************************************************************/
/*************************************************************************/

/* Initialize flag tables and flag sets from mode tables.  Must be called
 * before any other mode_* function, and should be called again if the mode
 * tables are modified.
 */

void mode_setup(void)
{
    int i;
    ModeData *modelist;
    char *flaglist;
    int multi_index = 0;   /* index into chanmode_multiple[] */

    modelist = usermodes;
    flaglist = userflags;
    for (i = 0; i < 256; i++) {
        if (modelist[i].flag) {
            int n = 0;
            uint32 tmp = (uint32) modelist[i].flag;
            if (modelist[i].info & MI_REGISTERED)
                usermode_reg |= tmp;
            while (tmp >>= 1)
                n++;
            if (n < 31)
                flaglist[n] = (char)i;
        }
    }

    modelist = chanmodes;
    flaglist = chanflags;
    for (i = 0; i < 256; i++) {
        if (modelist[i].flag) {
            int n = 0;
            uint32 tmp = (uint32) modelist[i].flag;
            if (modelist[i].info & MI_REGISTERED)
                chanmode_reg |= tmp;
            if (modelist[i].info & MI_REGNICKS_ONLY)
                chanmode_regonly |= tmp;
            if (modelist[i].info & MI_OPERS_ONLY)
                chanmode_opersonly |= tmp;
            if (modelist[i].info & MI_MULTIPLE)
                chanmode_multiple[multi_index++] = i;
            while (tmp >>= 1)
                n++;
            if (n < 31)
                flaglist[n] = (char)i;
        }
    }
    chanmode_multiple[multi_index] = 0;

    modelist = chanusermodes;
    flaglist = chanuserflags;
    for (i = 0; i < 256; i++) {
        if (modelist[i].flag) {
            int n = 0;
            uint32 tmp = (uint32) modelist[i].flag;
            prefixtable[ (uint8)modelist[i].prefix ] = tmp;
            while (tmp >>= 1)
                n++;
            if (n < 31)
                flaglist[n] = (char)i;
            if (modelist[i].plus_params!=1 || modelist[i].minus_params!=1) {
                log("modes: Warning: channel user mode `%c' takes %d/%d"
                    " parameters (should be 1/1)",
                    i, modelist[i].plus_params, modelist[i].minus_params);
            }
        }
    }
}

/*************************************************************************/

/* Return the flag corresponding to the given mode character, or 0 if no
 * such mode exists.  Return MODE_INVALID if the mode exists but has no
 * assigned flag.  `which' indicates the mode set to be used: MODE_USER,
 * MODE_CHANNEL, or MODE_CHANUSER.
 */

int32 mode_char_to_flag(char c, int which)
{
    if (which != MODE_USER && which != MODE_CHANNEL && which != MODE_CHANUSER){
        log("mode_char_to_flag(): bad `which' value %d", which);
        return MODE_INVALID;
    }
    return modetable[which][(uint8)c].flag;
}

/*************************************************************************/

/* Return the number of parameters the given mode takes, as
 * (plus_params<<8) | (minus_params).  Return -1 if there is no such mode.
 */

int mode_char_to_params(char c, int which)
{
    ModeData *ptr;

    if (which != MODE_USER && which != MODE_CHANNEL && which != MODE_CHANUSER){
        log("mode_char_to_params(): bad `which' value %d", which);
        return -1;
    }
    ptr = &modetable[which][(uint8)c];
    return ptr->plus_params<<8 | ptr->minus_params;
}

/*************************************************************************/

/* Return the mode character corresponding to the given flag, or 0 if no
 * such mode exists.  If more than one bit is set in `f', then the mode
 * character corresponding to the highest bit is used.
 */

char mode_flag_to_char(int32 f, int which)
{
    char *flaglist;
    int n = 0, tmp = f;

    if (which != MODE_USER && which != MODE_CHANNEL && which != MODE_CHANUSER){
        log("mode_flag_to_char(): bad `which' value %d", which);
        return 0;
    }
    flaglist = flagtable[which];

    while (tmp >>= 1)
        n++;
    if (n >= 31)
        return 0;
    return flaglist[n];
}

/*************************************************************************/

/* Return the flag set corresponding to the given string of mode characters
 * in the given mode set.  If MODE_NOERROR is set in `which', invalid mode
 * characters are ignored; if not set, (CMODE_INVALID | modechar) is
 * returned for the first invalid mode character found.  
 */

int32 mode_string_to_flags(const char *s, int which)
{
    int noerror = (which & MODE_NOERROR);
    int32 flags = 0;
    const ModeData *modelist;

    which &= ~MODE_NOERROR;
    if (which != MODE_USER && which != MODE_CHANNEL && which != MODE_CHANUSER){
        log("mode_string_to_flags(): bad `which' value %d", which|noerror);
        return 0;
    }
    modelist = modetable[which];

    while (*s) {
        int f = modelist[(uint8)*s].flag;
        if (!f) {
            if (noerror) {
                s++;
                continue;
            }
            return MODE_INVALID | *s;
        }
        if (f != MODE_INVALID)
            flags |= f;
        s++;
    }
    return flags;
}

/*************************************************************************/

/* Return the string of mode characters corresponding to the given flag
 * set.  If the flag set has invalid flags in it, they are ignored.
 * The returned string is stored in a static buffer which will be
 * overwritten on the next call.
 */

char *mode_flags_to_string(int32 flags, int which)
{
    static char buf[32];
    char *s = buf;
    int n = 0;
    const char *flaglist;

    if (which != MODE_USER && which != MODE_CHANNEL && which != MODE_CHANUSER){
        log("mode_flags_to_string(): bad `which' value %d", which);
        *buf = 0;
        return buf;
    }
    flaglist = flagtable[which];

    flags &= ~MODE_INVALID;
    while (flags) {
        if ((flags & 1) && flaglist[n])
            *s++ = flaglist[n];
        n++;
        flags >>= 1;
    }
    *s = 0;
    return buf;
}

/*************************************************************************/

/* Return the flag corresponding to the given channel user mode prefix, or
 * 0 if no such mode exists.
 */

int32 cumode_prefix_to_flag(char c)
{
    return prefixtable[(uint8)c];
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
