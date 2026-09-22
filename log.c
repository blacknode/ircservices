/* Logging routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include <netdb.h>  /* for hstrerror() */

/* Pattern for generating logfile names */
static char logfile_pattern[PATH_MAX+1];

/* Currently open log file */
static FILE *logfile;
static char current_filename[PATH_MAX+1];

/* Memory log buffer (see open_memory_log()) */
static char logmem[LOGMEMSIZE];
static char *logmemptr = NULL;

/* Are we in fatal() or fatal_perror()?  (This is used to avoid infinite
 * recursion if wallops() does a fatal().) */
static int in_fatal = 0;

/*************************************************************************/
/*************************************************************************/

/* Local routine to generate a filename from a filename pattern (possibly
 * containing %y/%m/%d for year/month/day).  Result is returned in a static
 * buffer, and will not be longer than PATH_MAX characters.
 */

static char *gen_log_filename(void)
{
    static char result[PATH_MAX+1];
    const char *s, *from;
    char *to;

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    tm->tm_year += 1900;
    tm->tm_mon++;

    from = LogFilename;
    if (!*from) {
        *result = 0;
        return result;
    }
    to = result;

    while ((s = strchr(from, '%')) != NULL) {
        to += snprintf(to, sizeof(result)-(to-result), "%.*s", s-from, from);
        s++;
        switch (*s) {
          case 'y':
            to += snprintf(to, sizeof(result)-(to-result), "%d", tm->tm_year);
            break;
          case 'm':
            to += snprintf(to, sizeof(result)-(to-result), "%02d", tm->tm_mon);
            break;
          case 'd':
            to += snprintf(to, sizeof(result)-(to-result), "%02d",tm->tm_mday);
            break;
          default:
            if (to-result < sizeof(result)-1)
                *to++ = *s;
            break;
        }
        from = s+1;
    }
    to += snprintf(to, sizeof(result)-(to-result), "%s", from);

    *to = 0;
    return result;
}

/*************************************************************************/

/* Local routine to check whether the log file needs to be rotated, and
 * rotate it if so.  Assumes the log file is already open.
 */

static void check_log_rotate(void)
{
    char *newname = gen_log_filename();
    if (strlen(newname) > sizeof(current_filename)-1)
        newname[sizeof(current_filename)-1] = 0;
    if (strcmp(current_filename, gen_log_filename()) != 0) {
        if (!reopen_log())
            log("Warning: Unable to rotate log file: %s", strerror(errno));
    }
}

/*************************************************************************/
/*************************************************************************/

/* Local routines to write text to the log file and/or stderr as needed. */

static void vlogprintf(const char *fmt, va_list args)
{
    if (nofork) {
        va_list args_copy;
        va_copy(args_copy, args);
        vfprintf(stderr, fmt, args_copy);
        va_end(args_copy);
    }
    if (logfile) {
        vfprintf(logfile, fmt, args);
    } else if (logmemptr) {
        char tmpbuf[BUFSIZE];
        int len = vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, args);
        if (len > LOGMEMSIZE - (logmemptr-logmem)) {
            int oldlen = len;
            len = LOGMEMSIZE - (logmemptr-logmem);
            if (len > 0) {
                if (tmpbuf[oldlen-1] == '\n') {
                    tmpbuf[len-1] = '\n';  /* always end with a newline */
                } else {
                    len--;
                }
            }
        }
        if (len > 0) {
            memcpy(logmemptr, tmpbuf, len);
            logmemptr += len;
        }
    }
}

static void logprintf(const char *fmt, ...) FORMAT(printf,1,2);
static void logprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vlogprintf(fmt, args);
    va_end(args);
}

static void logputs(const char *str)
{
    logprintf("%s", str);
}

/*************************************************************************/

/* Local routine to write the time of day to the log. */

static void write_time(void)
{
    time_t t;
    struct tm tm;
    char buf[256];

    time(&t);
    tm = *localtime(&t);
#if HAVE_GETTIMEOFDAY
    if (debug) {
        char *s;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        strftime(buf, sizeof(buf)-1, "[%b %d %H:%M:%S", &tm);
        s = buf + strlen(buf);
        s += snprintf(s, sizeof(buf)-(s-buf), ".%06d", (int)tv.tv_usec);
        strftime(s, sizeof(buf)-(s-buf)-1, " %Y] ", &tm);
    } else {
#endif
        strftime(buf, sizeof(buf)-1, "[%b %d %H:%M:%S %Y] ", &tm);
#if HAVE_GETTIMEOFDAY
    }
#endif
    logputs(buf);
}

/*************************************************************************/
/*************************************************************************/

/* Set the filename pattern to use for generating the log filename. */

void set_logfile(const char *pattern)
{
    if (pattern) {  /* Just to be safe */
        strscpy(logfile_pattern, pattern, sizeof(logfile_pattern));
    }
}

/*************************************************************************/

/* Open the log file.  Return zero if the log file could not be opened,
 * else return nonzero (success).
 */

int open_log(void)
{
    if (logfile)
        return 1;
    strbcpy(current_filename, gen_log_filename());
    logfile = fopen(current_filename, "a");
    if (logfile) {
        setbuf(logfile, NULL);
        if (logmemptr) {
            int res, errno_save;
            if (logmemptr > logmem) {
                res = fwrite(logmem, logmemptr-logmem, 1, logfile);
                errno_save = errno;
            } else {
                res = 1;  /* i.e. no error */
#if CLEAN_COMPILE
                errno_save = 0;  /* warning killer for dumb compilers */
#endif
            }
            logmemptr = NULL;
            if (res != 1) {
                /* make sure this is AFTER the memory log has been closed! */
                errno = errno_save;
                log_perror("open_log(): error writing memory log");
            }
        }
    }
    return logfile!=NULL ? 1 : 0;
}

/*************************************************************************/

/* Open a virtual log file in memory.  The contents of the memory log file
 * will be written to a physical log file when open_log() is called and
 * executes successfully.  If a physical log file is already open, does
 * nothing.  Always succeeds (and returns nonzero).
 */

int open_memory_log(void)
{
    if (logfile || logmemptr)
        return 1;
    logmemptr = logmem;
    return 1;
}

/*************************************************************************/

/* Close the log file. */

void close_log(void)
{
    if (logmemptr) {
        logmemptr = NULL;
    }
    if (logfile) {
        fclose(logfile);
        logfile = NULL;
    }
}

/*************************************************************************/

/* Reopen the log file with the current value of LogFilename (for use when
 * rehashing configuration files or rotating the log file).  Return value
 * is like open_log().
 */

int reopen_log(void)
{
    char *newname;
    FILE *f;

    newname = gen_log_filename();
    /* Make sure it will fit in current_filename later */
    if (strlen(newname) > sizeof(current_filename)-1)
        newname[sizeof(current_filename)-1] = 0;
    f = fopen(newname, "a");
    if (!f)
        return 0;
    setbuf(f, NULL);
    if (logfile)
        fclose(logfile);
    logfile = f;
    strcpy(current_filename, newname);  /* safe b/c of length check above */
    return 1;
}

/*************************************************************************/

/* Return nonzero if the log file is currently open, zero if closed. */

int log_is_open(void)
{
    return logfile != NULL;
}

/*************************************************************************/
/*************************************************************************/

/* Log stuff to the log file with a datestamp when debug >= debuglevel.
 * errno is preserved.  If debuglevel is greater than zero, "debug: " is
 * prefixed to the message.  If do_perror is nonzero, the message is
 * followed by a ": " and system error message, a la perror() (if errno<0,
 * it is treated as an herror value from hostname resolution).  If
 * modulename is not NULL, it is used as a prefix to the error message.
 *
 * [module_]log[_perror][_debug]() are implemented as macros which call
 * this function.
 */

void do_log(int debuglevel, int do_perror, const char *modulename,
            const char *fmt, ...)
{
    if (debug >= debuglevel) {
        va_list args;
        int errno_save = errno;

        va_start(args, fmt);
        if (logfile)
            check_log_rotate();
        write_time();
        if (debuglevel > 0)
            logputs("debug: ");
        if (modulename)
            logprintf("(%s) ", modulename);
        vlogprintf(fmt, args);
        if (do_perror) {
            logprintf(": %s",
                      (errno_save<0) ? hstrerror(-errno_save)
                                     : strerror(errno_save));
        }
        logputs("\n");
        va_end(args);
        errno = errno_save;
    }
}

/*************************************************************************/
/*************************************************************************/

/* We've hit something we can't recover from.  Let people know what
 * happened, then go down.
 */

void fatal(const char *fmt, ...)
{
    va_list args;
    char buf[4096];

    if (in_fatal)
        exit(2);
    in_fatal++;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (logfile)
        check_log_rotate();
    write_time();
    logprintf("FATAL: %s\n", buf);
    if (servsock && sock_isconn(servsock))
        wallops(NULL, "FATAL ERROR!  %s", buf);
    exit(1);
}

/*************************************************************************/

/* Same thing, but do it like perror(). */

void fatal_perror(const char *fmt, ...)
{
    va_list args;
    char buf[4096];
    const char *errstr;

    if (in_fatal)
        exit(2);
    in_fatal++;
    errstr = (errno<0) ? hstrerror(-errno) : strerror(errno);
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    if (logfile)
        check_log_rotate();
    write_time();
    logprintf("FATAL: %s: %s\n", buf, errstr);
    if (servsock && sock_isconn(servsock))
        wallops(NULL, "FATAL ERROR!  %s: %s", buf, errstr);
    exit(1);
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
