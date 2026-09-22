/* Mode flag definitions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MODES_H
#define MODES_H

/*************************************************************************/

/* Special flag constants. */

#define MODE_INVALID    0x80000000      /* Used as error flag, or as "this
                                         *    isn't an on/off mode" flag */
#define MODE_ALL        (~MODE_INVALID) /* All possible modes */

/*************************************************************************/

/* Data for mode characters.  If a mode character is given the MODE_INVALID
 * flag, it indicates that that mode is a valid mode but is assigned no
 * flag.  (For example, channel mode +b uses this.) */

typedef struct {
    int32 flag;
    uint8 plus_params;  /* Number of parameters when adding */
    uint8 minus_params; /* Number of characters when deleting */
    char prefix;        /* Prefix for channel user mode (e.g. +o -> '@') */
    uint32 info;        /* What kind of mode this is (MI_* below) */
} ModeData;

#define MI_MULTIPLE     0x01  /*      Can be set multiple times (+b etc) */
#define MI_REGISTERED   0x02  /* [UC] Applied to all registered nicks/chans */
#define MI_OPERS_ONLY   0x04  /* [ C] Only opers may join */
#define MI_REGNICKS_ONLY 0x08 /* [ C] Only registered/ID'd nicks may join */

/* These bits are available for private use by protocol modules: (see
 * Unreal module for an example of usage) */
#define MI_LOCAL_MASK   0xFF000000

/*************************************************************************/

/*
 * Arrays of mode characters--one entry for each possible character.  These
 * are declared extern to allow modules to add entries; read access should
 * be done through the functions below.
 *
 * The following modes are predefined:
 *    User: o, i, w
 *    Channel: i, k, l, m, n, p, s, t, b
 *    Channel user (modes applied to individual users on a channel): o, v
 */
extern ModeData usermodes[256], chanmodes[256], chanusermodes[256];

/* The following are initialized by mode_setup(): */
extern int32 usermode_reg;      /* Usermodes applied to registered nicks */
extern int32 chanmode_reg;      /* Chanmodes applied to registered chans */
extern int32 chanmode_regonly;  /* Chanmodes indicating regnick-only channels*/
extern int32 chanmode_opersonly;/* Chanmodes indicating oper-only channels */
extern char chanmode_multiple[];/* Chanmodes that can be set multiple times */

/*************************************************************************/

/* Initialize flag tables and flag sets from mode tables.  Must be called
 * before any other mode_* function. */
extern void mode_setup(void);

/* Return the flag corresponding to the given mode character, or 0 if no
 * such mode exists.  Return MODE_INVALID if the mode exists but has no
 * assigned flag.  See below for the meaning of "which". */
extern int32 mode_char_to_flag(char c, int which);

/* Return the number of parameters the given mode takes, as
 * (plus_params<<8) | (minus_params).  Return -1 if there is no such mode. */
extern int mode_char_to_params(char c, int which);

/* Return the mode character corresponding to the given flag, or 0 if no
 * such mode exists. */
extern char mode_flag_to_char(int32 f, int which);

/* Return the flag set corresponding to the given string of mode
 * characters, or (CMODE_INVALID | modechar) if an invalid mode character
 * is found. */
extern int32 mode_string_to_flags(const char *s, int which);

/* Return the string of mode characters corresponding to the given flag
 * set.  If the flag set has invalid flags in it, they are ignored.
 * The returned string is stored in a static buffer which will be
 * overwritten on the next call. */
extern char *mode_flags_to_string(int32 flags, int which);

/* Return the flag corresponding to the given channel user mode prefix, or
 * 0 if no such mode exists. */
extern int32 cumode_prefix_to_flag(char c);

/*************************************************************************/

/* Values for "which" parameter to mode_* functions: */

#define MODE_USER       0       /* UMODE_* (user modes) */
#define MODE_CHANNEL    1       /* CMODE_* (binary channel modes) */
#define MODE_CHANUSER   2       /* CUMODE_* (channel modes for users) */

#define MODE_NOERROR    0x8000  /* Ignore bad chars in string_to_flags() */

/*************************************************************************/

/* User modes: */
#define UMODE_o         0x00000001
#define UMODE_i         0x00000002
#define UMODE_w         0x00000004

/* Channel modes: */
#define CMODE_i         0x00000001
#define CMODE_m         0x00000002
#define CMODE_n         0x00000004
#define CMODE_p         0x00000008
#define CMODE_s         0x00000010
#define CMODE_t         0x00000020
#define CMODE_k         0x00000040
#define CMODE_l         0x00000080

/* Modes for users on channels: */
#define CUMODE_o        0x00000001
#define CUMODE_v        0x00000002

/*************************************************************************/
/*************************************************************************/

#endif  /* MODES_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
