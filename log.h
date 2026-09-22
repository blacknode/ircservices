/* Definitions and prototypes related to logging.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */


#ifndef LOG_H
#define LOG_H

extern void set_logfile(const char *pattern);
extern int open_log(void);
extern int open_memory_log(void);
extern void close_log(void);
extern int reopen_log(void);
extern int log_is_open(void);
extern void do_log(int debuglevel, int do_perror, const char *modulename,
                   const char *fmt, ...)
        FORMAT(printf,4,5);
extern void fatal(const char *fmt, ...)
        FORMAT(printf,1,2);
extern void fatal_perror(const char *fmt, ...)
        FORMAT(printf,1,2);

#undef log  /* in case system includes #define it */
#define log(fmt,...) \
    log_debug(0, fmt , ##__VA_ARGS__)
#define log_perror(fmt,...) \
    log_perror_debug(0, fmt , ##__VA_ARGS__)
#define log_debug(debuglevel,fmt,...) \
    do_log(debuglevel, 0, NULL, fmt, ##__VA_ARGS__)
#define log_perror_debug(debuglevel,fmt,...) \
    do_log(debuglevel, 1, NULL, fmt, ##__VA_ARGS__)
#define module_log(fmt,...) \
    module_log_debug(0, fmt , ##__VA_ARGS__)
#define module_log_perror(fmt,...) \
    module_log_perror_debug(0, fmt , ##__VA_ARGS__)
#define module_log_debug(debuglevel,fmt,...) \
    do_log(debuglevel, 0, MODULE_NAME, fmt, ##__VA_ARGS__)
#define module_log_perror_debug(debuglevel,fmt,...) \
    do_log(debuglevel, 1, MODULE_NAME, fmt, ##__VA_ARGS__)

#endif  /* LOG_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
