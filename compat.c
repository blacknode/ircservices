/* Compatibility routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"

#if !HAVE_HSTRERROR
# include <netdb.h>
#endif

/*************************************************************************/

#if !HAVE_HSTRERROR

/* hstrerror: return an error message for the given h_errno code. */

const char *hstrerror(int h_errnum)
{
    switch (h_errnum) {
        case HOST_NOT_FOUND: return "Host not found";
        case TRY_AGAIN:      return "Host not found, try again";
        case NO_RECOVERY:    return "Nameserver error";
        case NO_DATA:        return "No data of requested type";
        default:             return "Unknown error";
    }
}

#endif /* !HAVE_HSTRERROR */

/*************************************************************************/

#if !HAVE_SNPRINTF

/* [v]snprintf: Like [v]sprintf, but don't write more than len bytes
 *              (including null terminator).  Return the number of bytes
 *              written.
 */

#if BAD_SNPRINTF
int vsnprintf(char *buf, size_t len, const char *fmt, va_list args)
{
    if (len <= 0)
        return 0;
    *buf = 0;
#undef vsnprintf
    vsnprintf(buf, len, fmt, args);
#define vsnprintf my_vsnprintf
    buf[len-1] = 0;
    return strlen(buf);
}
#endif  /* BAD_SNPRINTF */

int snprintf(char *buf, size_t len, const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vsnprintf(buf, len, fmt, args);
    va_end(args);
    return ret;
}

#endif /* !HAVE_SNPRINTF */

/*************************************************************************/

#if !HAVE_STRTOK

/* glibc 2.2 (RedHat 7.0) has a broken strtok--it dies if called with a
 * NULL parameter after returning NULL once.  glibc and possibly other
 * libraries also return non-NULL for strtok(NULL, "") even after
 * returning NULL for strtok(NULL, " ").
 */

char *strtok(char *str, const char *delim)
{
    static char *current = NULL;
    char *ret;

    if (str)
        current = str;
    if (!current)
        return NULL;
    current += strspn(current, delim);
    ret = *current ? current : NULL;
    current += strcspn(current, delim);
    if (!*current)
        current = NULL;
    else
        *current++ = 0;
    return ret;
}

#endif /* !HAVE_STRTOK */

/*************************************************************************/

#if !HAVE_STRICMP && !HAVE_STRCASECMP

/* stricmp, strnicmp:  Case-insensitive versions of strcmp() and
 *                     strncmp().
 */

int stricmp(const char *s1, const char *s2)
{
    register int c;

    while ((c = tolower(*s1)) == tolower(*s2)) {
        if (c == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}

int strnicmp(const char *s1, const char *s2, size_t len)
{
    register int c;

    if (!len)
        return 0;
    while ((c = tolower(*s1)) == tolower(*s2) && len > 0) {
        if (c == 0 || --len == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}
#endif

/*************************************************************************/

#if !HAVE_STRDUP && !MEMCHECKS
char *strdup(const char *s)
{
    char *new = malloc(strlen(s)+1);
    if (new)
        strcpy(new, s);
    return new;
}
#endif

/*************************************************************************/

#if !HAVE_STRSPN

size_t strspn(const char *s, const char *accept)
{
    size_t i = 0;

    while (*s && strchr(accept, *s))
        ++i, ++s;
    return i;
}

size_t strcspn(const char *s, const char *reject)
{
    size_t i = 0;

    while (*s && !strchr(reject, *s))
        ++i, ++s;
    return i;
}

#endif

/*************************************************************************/

#if !HAVE_STRERROR
# if HAVE_SYS_ERRLIST
extern char *sys_errlist[];
# endif

char *strerror(int errnum)
{
# if HAVE_SYS_ERRLIST
    return sys_errlist[errnum];
# else
    static char buf[20];
    snprintf(buf, sizeof(buf), "Error %d", errnum);
    return buf;
# endif
}
#endif

/*************************************************************************/

#if !HAVE_STRSIGNAL
char *strsignal(int signum)
{
    static char buf[32];
    switch (signum) {
        case SIGHUP:    strbcpy(buf, "Hangup");                         break;
        case SIGINT:    strbcpy(buf, "Interrupt");                      break;
        case SIGQUIT:   strbcpy(buf, "Quit");                           break;
#ifdef SIGILL
        case SIGILL:    strbcpy(buf, "Illegal instruction");            break;
#endif
#ifdef SIGABRT
        case SIGABRT:   strbcpy(buf, "Abort");                          break;
#endif
#if defined(SIGIOT) && (!defined(SIGABRT) || SIGIOT != SIGABRT)
        case SIGIOT:    strbcpy(buf, "IOT trap");                       break;
#endif
#ifdef SIGBUS
        case SIGBUS:    strbcpy(buf, "Bus error");                      break;
#endif
        case SIGFPE:    strbcpy(buf, "Floating point exception");       break;
        case SIGKILL:   strbcpy(buf, "Killed");                         break;
        case SIGUSR1:   strbcpy(buf, "User signal 1");                  break;
        case SIGSEGV:   strbcpy(buf, "Segmentation fault");             break;
        case SIGUSR2:   strbcpy(buf, "User signal 2");                  break;
        case SIGPIPE:   strbcpy(buf, "Broken pipe");                    break;
        case SIGALRM:   strbcpy(buf, "Alarm clock");                    break;
        case SIGTERM:   strbcpy(buf, "Terminated");                     break;
        case SIGSTOP:   strbcpy(buf, "Stopped (signal)");               break;
        case SIGTSTP:   strbcpy(buf, "Stopped");                        break;
        case SIGIO:     strbcpy(buf, "I/O error");                      break;
        default:        snprintf(buf, sizeof(buf), "Signal %d\n", signum);
                        break;
    }
    return buf;
}
#endif

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
