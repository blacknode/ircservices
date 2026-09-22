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

/*************************************************************************/
/****************** START OF USER-CONFIGURABLE SECTION *******************/
/*************************************************************************/

/******* General configuration *******/

/* Name of configuration file (in Services directory) */
#define IRCSERVICES_CONF        PROGRAM ".conf"

/* Name of module configuration file (in Services directory) */
#define MODULES_CONF            "modules.conf"

/* Maximum number of parameters for a configuration directive */
#define CONFIG_MAXPARAMS        8

/* Maximum number of channels to buffer modes for (for MergeChannelModes) */
#define MERGE_CHANMODES_MAX     3


/******* NickServ configuration *******/

/* Default language for newly registered nicks; see language.h for
 * available languages (LANG_* constants).  Unless you're running a
 * regional network, you should probably leave this at LANG_EN_US. */
#define DEF_LANGUAGE    LANG_EN_US


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
#define BUFSIZE         1024

/* Maximum length of a configuration file line */
#define CONFIG_LINEMAX  4096

/* Size of memory-based log buffer (only used with SHOWALLOCS) */
#define LOGMEMSIZE      65536

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
#define CHANMAX         64

/* Maximum length of a nickname, including the trailing null.  This MUST be
 * at least one greater than the maximum allowable nickname length on your
 * network, or people will run into problems using Services!  The default
 * (32) should work for all current servers. */
#define NICKMAX         32

/* Maximum length of an unencrypted password, including the trailing null. */
#define PASSMAX         32

/*************************************************************************/

/* For convert-db, we redefine the above values to be large enough for all
 * potential strings.  Do not modify these or convert-db will explode in
 * your face, painfully. */

#ifdef CONVERT_DB
# undef NICKMAX
# undef CHANMAX
# undef PASSMAX
# define NICKMAX 256
# define CHANMAX 512
# define PASSMAX 256
#endif

/*************************************************************************/
/*************************************************************************/

/* ---- There should be no need to modify anything below this line. ---- */

/*************************************************************************/
/*************************************************************************/

/* Common includes/prototypes. */


/* glibc seems to require this for the prototype of strsignal(). */
#define _GNU_SOURCE

/* Some AIX boxes define int16 and int32 on their own.  Blarph. */
#if INTTYPE_WORKAROUND
# define int16 builtin_int16
# define int32 builtin_int32
#endif


/* We have our own encrypt(). */
#define encrypt encrypt_

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#undef DEFS_H  /* kludge to work around Cygwin string.h kludge to work
                * around problem compiling in gdb... this is stupid */
#include <string.h>
#define DEFS_H
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>

#undef encrypt

#if HAVE_STDINT_H
# include <stdint.h>
#endif

#if HAVE_STRINGS_H
# include <strings.h>
#endif

#if HAVE_SYS_SELECT_H
/* FreeBSD (4.4-STABLE) defines LIST_REMOVE and LIST_FOREACH in
 * <sys/queue.h>, which is included by <sys/select.h>, so make sure we
 * include it here before list-array.h defines our own versions of those. */
# include <sys/select.h>
#endif

#ifdef _AIX
/* Some AIX boxes seem to have bogus includes that don't have these
 * prototypes. */
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
# if 0  /* These break on some AIX boxes (4.3.1 reported). */
extern int gettimeofday(struct timeval *, struct timezone *);
extern int socket(int, int, int);
extern int bind(int, struct sockaddr *, int);
extern int connect(int, struct sockaddr *, int);
extern int shutdown(int, int);
# endif
# undef FD_ZERO
# define FD_ZERO(p) memset((p), 0, sizeof(*(p)))
#endif /* _AIX */

/* Alias stricmp/strnicmp to strcasecmp/strncasecmp if we have the latter
 * but not the former. */
#if !HAVE_STRICMP && HAVE_STRCASECMP
# define stricmp strcasecmp
# define strnicmp strncasecmp
#endif

/* socklen_t for systems without it. */
#if !HAVE_SOCKLEN_T
typedef int socklen_t;
#endif


#if INTTYPE_WORKAROUND
# undef int16
# undef int32
#endif

/*************************************************************************/

/* System/compiler sanity checks. */


/* Filename and pathname maximum lengths: (these are usually defined in
 * limits.h, but check just in case) */
#ifndef NAME_MAX
# define NAME_MAX 255
#endif
#ifndef PATH_MAX
# define PATH_MAX 1023
#endif

/* Number of signals available: */
#ifndef NSIG
# define NSIG 32
#endif

/*************************************************************************/

/* Various generally useful macros. */


/* Make sizeof() return an int regardless of compiler (avoids printf
 * argument type warnings). */
#define sizeof(v)       ((int)sizeof(v))

/* Length of an array: */
#define lenof(a)        (sizeof(a) / sizeof(*(a)))

/* Sign of a number: (-1, 0, or 1) */
#define sgn(n)          ((n)<0 ? -1 : ((n)>0))

/* Telling compilers about printf()-like functions: */
#ifdef __GNUC__
# define FORMAT(type,fmt,start) __attribute__((format(type,fmt,start)))
#else
# define FORMAT(type,fmt,start)
#endif

/* Macros to define a function pointer (E_FUNCPTR declares it extern).
 * This is needed because GCC doesn't seem to like defining a pointer to a
 * function with __attribute__s in a single statement. */
#ifdef __GNUC__
# define FUNCPTR(type,name,rest) \
    type _##name##_t rest; \
    typeof(_##name##_t) *name
# define E_FUNCPTR(type,name,rest) \
    type _##name##_t rest; \
    extern typeof(_##name##_t) *name
#else
# define FUNCPTR(type,name,rest)          type (*name) rest
# define E_FUNCPTR(type,name,rest) extern type (*name) rest
#endif

/*************************************************************************/

/* Generic "invalid" pointer value.  For use when an "invalid" value is
 * needed and NULL cannot be used. */

#define PTR_INVALID     ((const char *)-1)

/*************************************************************************/

#endif  /* DEFS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
