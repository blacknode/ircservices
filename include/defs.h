/* Basic constants, macros and prototypes.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef DEFS_H
#define DEFS_H

#include "config.h"

/*************************************************************************/
/****************** START OF USER-CONFIGURABLE SECTION *******************/
/*************************************************************************/

/******* General configuration *******/

/* Name of configuration file (in Services directory) */
#define IRCSERVICES_CONF PROGRAM ".conf"

/* Name of module configuration file (in Services directory) */
#define MODULES_CONF "modules.conf"

/* Maximum number of parameters for a configuration directive */
#define CONFIG_MAXPARAMS 8

/* Maximum number of channels to buffer modes for (for MergeChannelModes) */
#define MERGE_CHANMODES_MAX 3

/******* NickServ configuration *******/

/* Default language for newly registered nicks; see language.h for
 * available languages (LANG_* constants).  Unless you're running a
 * regional network, you should probably leave this at LANG_EN_US. */
#define DEF_LANGUAGE LANG_EN_US

/******* OperServ configuration *******/

/* Define this to enable OperServ's debugging commands (Services root
 * only).  These commands are undocumented; "use the source, Luke!" */
/* #define DEBUG_COMMANDS */

/*************************************************************************/
/******************* END OF USER-CONFIGURABLE SECTION ********************/
/*************************************************************************/

/* Various buffer sizes */

/* Size of input buffer (note: this is different from BUFSIZ)
 * This MUST be big enough to hold at least one full IRC message, or Bad
 * Things will happen. */
#define BUFSIZE 1024

/* Maximum length of a configuration file line */
#define CONFIG_LINEMAX 4096

/* Size of memory-based log buffer (only used with SHOWALLOCS) */
#define LOGMEMSIZE 65536

/*************************************************************************/

/*
 * The following constants define the sizes of channel name, nickname, and
 * password buffers used in Services.  These should only be adjusted if
 * your IRC network allows longer nicknames or channel names _and_ you wish
 * to allow such names to be used with Services.  If your IRC network has
 * smaller limits, you do not need to change these values; Services will
 * still work fine, albeit with a tiny amount of wasted memory for each
 * nickname and channel.
 *
 * WARNING:  If you change these, you MUST back up your data to an XML file
 * before making the change, and re-import the data afterwards.  Database
 * files created with different calues of CHANMAX/NICKMAX/PASSMAX are not
 * compatible!
 */

/* Maximum length of a channel name, including the trailing null.  Any
 * channels with a length longer than CHANMAX-1 (including the leading #)
 * will not be usable with ChanServ. */
#define CHANMAX 64

/* Maximum length of a nickname, including the trailing null.  This MUST be
 * at least one greater than the maximum allowable nickname length on your
 * network, or people will run into problems using Services!  The default
 * (32) should work for all current servers. */
#define NICKMAX 32

/* Maximum length of an unencrypted password, including the trailing null. */
#define PASSMAX 32

/*************************************************************************/

/* For convert-db, we redefine the above values to be large enough for all
 * potential strings.  Do not modify these or convert-db will explode in
 * your face, painfully. */

#ifdef CONVERT_DB
#undef NICKMAX
#undef CHANMAX
#undef PASSMAX
#define NICKMAX 256
#define CHANMAX 512
#define PASSMAX 256
#endif

/*************************************************************************/
/*************************************************************************/

/* ---- There should be no need to modify anything below this line. ---- */

/*************************************************************************/
/*************************************************************************/

/* Common includes.  The code base targets C99 on a POSIX system. */

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/* We have our own encrypt() (see encrypt.h); keep <unistd.h>'s out of the
 * way. */
#define encrypt encrypt_
#include <string.h>
#include <strings.h>
#include <unistd.h>
#undef encrypt
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

/* Historical names for the case-insensitive comparisons. */
#define stricmp  strcasecmp
#define strnicmp strncasecmp

/*************************************************************************/

/* System/compiler sanity checks. */

/* Filename and pathname maximum lengths: (these are usually defined in
 * limits.h, but check just in case) */
#ifndef NAME_MAX
#define NAME_MAX 255
#endif
#ifndef PATH_MAX
#define PATH_MAX 1023
#endif

/* Number of signals available: */
#ifndef NSIG
#define NSIG 32
#endif

/*************************************************************************/

/* Various generally useful macros. */

/* Make sizeof() return an int regardless of compiler (avoids printf
 * argument type warnings). */
#define sizeof(v) ((int)sizeof(v))

/* Length of an array: */
#define lenof(a) (sizeof(a) / sizeof(*(a)))

/* Sign of a number: (-1, 0, or 1) */
#define sgn(n) ((n) < 0 ? -1 : ((n) > 0))

/* Telling compilers about printf()-like functions: */
#ifdef __GNUC__
#define FORMAT(type, fmt, start) __attribute__((format(type, fmt, start)))
#else
#define FORMAT(type, fmt, start)
#endif

/* Macros to define a function pointer (E_FUNCPTR declares it extern).
 * This is needed because GCC doesn't seem to like defining a pointer to a
 * function with __attribute__s in a single statement. */
#ifdef __GNUC__
#define FUNCPTR(type, name, rest)                                             \
    type _##name##_t rest;                                                    \
    typeof(_##name##_t)* name
#define E_FUNCPTR(type, name, rest)                                           \
    type _##name##_t rest;                                                    \
    extern typeof(_##name##_t)* name
#else
#define FUNCPTR(type, name, rest)   type(*name) rest
#define E_FUNCPTR(type, name, rest) extern type(*name) rest
#endif

/*************************************************************************/

/* Generic "invalid" pointer value.  For use when an "invalid" value is
 * needed and NULL cannot be used. */

#define PTR_INVALID ((const char*)-1)

/*************************************************************************/

#endif /* DEFS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
