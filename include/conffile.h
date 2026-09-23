/* Structures, constants, and external declarations for configuration file
 * handling.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef CONFFILE_H
#define CONFFILE_H

/*************************************************************************/

/* Configuration directives.  Note that all numeric parameter types except
 * CD_TIME and CD_SET take an int32 value pointer (including CD_TIMEMSEC).
 */

/* Information about a configuration parameter's value: */
typedef union {
    void *ptrval;
    int32 intval;
    time_t timeval;
} CDValue;

/* Information about a configuration directive: */
typedef struct {
    const char *name;
    struct {
        int type;       /* Parameter type (CD_* below) */
        int flags;      /* Parameter flags (CF_* below) */
        void *ptr;      /* Pointer to where to store the value */
        /* The following data is internal-use-only: */
        CDValue prev;   /* Previous value (to restore when deconfigured) */
        CDValue new;    /* New value (to set if conf-file successfully read) */
    } params[CONFIG_MAXPARAMS];
    /* Also internal use only: */
    int was_seen;       /* Non-zero if directive was seen this time around */
} ConfigDirective;

#define CD_NONE         0
#define CD_INT          1
#define CD_POSINT       2       /* Positive integer only */
#define CD_PORT         3       /* 1..65535 only */
#define CD_STRING       4
#define CD_TIME         5       /* Type of `ptr' is `time_t *' */
#define CD_TIMEMSEC     6       /* Variable is in milliseconds, parameter
                                 *    in seconds (decimal allowed) */
#define CD_FUNC         7       /* `ptr' is a function to call; see
                                 *    init.c for examples */
#define CD_SET          -1      /* Not a real parameter; just set the
                                 *    given `int' variable to 1 */
#define CD_DEPRECATED   -2      /* Set for deprecated directives; causes
                                 *    a warning to be printed */

/* Flags: */
#define CF_OPTIONAL     0x01    /* Parameter is optional (defaults to 0) */
#define CF_DIRREQ       0x02    /* Directive is required (set on first param)*/
/* Internal-use-only flags: */
#define CF_SAVED        0x80    /* Original value saved in `prev' */
#define CF_WASSET       0x40    /* Parameter set this time around */
#define CF_ALLOCED      0x20    /* Current value is alloc'd by parser */
#define CF_ALLOCED_NEW  0x10    /* Value of `new' is alloc'd by parser */

/* Values for `action' parameter to configure() (can be or'ed together): */
#define CONFIGURE_READ  1       /* Read settings from configuration file */
#define CONFIGURE_SET   2       /* Set configuration variables to new values */

/* Values passed in `linenum' parameter to configuration directive handlers
 * (CD_FUNC parameters) when `filename' is NULL: */
#define CDFUNC_INIT     0       /* Prepare for reading data */
#define CDFUNC_SET      1       /* Copy new data to real config variables */
#define CDFUNC_DECONFIG 2       /* Delete any data in config variables */

/*************************************************************************/

/* Global functions: */

/* Set configuration options for the given module (if `modulename' is NULL,
 * set core configuration options).  Returns nonzero on success, 0 on error
 * (an error message is logged, and printed to the terminal if applicable,
 * in this case).  Returns successfully without doing anything if
 * `directives' is NULL. */
extern int configure(const char *modulename, ConfigDirective *directives,
                     int action);

/* Deconfigure given directive array (free any allocated storage and
 * restore original values).  A no-op if `directives' is NULL. */
extern void deconfigure(ConfigDirective *directives);

/* Print a warning or error message to the log (and the console, if open). */
extern void config_error(const char *filename, int linenum,
                         const char *message,...);

/*************************************************************************/

#endif  /* CONFFILE_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
