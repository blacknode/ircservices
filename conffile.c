/* Configuration file handling.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "conffile.h"

/*************************************************************************/

static void do_all_directives(int action, ConfigDirective *directives);

static int read_config_file(const char *modulename,
                            ConfigDirective *directives);

static int do_read_config_file(const char *modulename,
                               ConfigDirective *directives, FILE *f,
                               const char *filename, int recursion_level);

static int parse_config_line(const char *filename, int linenum, char *buf,
                             ConfigDirective *directives);


/* Actions for do_all_directives(): */

#define ACTION_COPYNEW      0   /* Copy `new' parameters to config variables */
#define ACTION_RESTORESAVED 1   /* Restore saved values of config variables */

/*************************************************************************/
/*************************************************************************/

/* Set configuration options for the given module (if `modulename' is NULL,
 * set core configuration options).  Returns nonzero on success, 0 on error
 * (an error message is logged, and printed to the terminal if applicable,
 * in this case).  Returns successfully without doing anything if
 * `directives' is NULL.
 *
 * `action' is a bitmask of CONFIGURE_* values (services.h), specifying
 * what this function should do, as follows:
 *     - CONFIGURE_READ: read new values from the configuration file
 *     - CONFIGURE_SET: copy new values to configuration variables
 * If both CONFIGURE_READ and CONFIGURE_SET are specified, new values are
 * copied to the configuration variables only if all values are read in
 * successfully (i.e. if a configure(...,CONFIGURE_READ) call would have
 * returned success).  CONFIGURE_SET alone will never fail.
 */

int configure(const char *modulename, ConfigDirective *directives,
              int action)
{
    /* If no directives were given, return success */
    if (!directives)
        return 1;

    if (action & CONFIGURE_READ) {
        if (!read_config_file(modulename, directives))
            return 0;
    }

    if (action & CONFIGURE_SET)
        do_all_directives(ACTION_COPYNEW, directives);

    return 1;
}

/*************************************************************************/

/* Deconfigure given directive array (free any allocated storage and
 * restore original values).  A no-op if `directives' is NULL.
 */

void deconfigure(ConfigDirective *directives)
{
    if (directives)
        do_all_directives(ACTION_RESTORESAVED, directives);
}

/*************************************************************************/

/* Print a warning or error message to the log (and the console, if open). */

void config_error(const char *filename, int linenum, const char *message, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, message);
    vsnprintf(buf, sizeof(buf), message, args);
    va_end(args);
    if (linenum)
        log("%s:%d: %s", filename, linenum, buf);
    else
        log("%s: %s", filename, buf);
    if (!nofork && isatty(2)) {
        if (linenum)
            fprintf(stderr, "%s:%d: %s\n", filename, linenum, buf);
        else
            fprintf(stderr, "%s: %s\n", filename, buf);
    }
}

/*************************************************************************/
/*************************************************************************/

/* Perform an action for all directives in an array; the action is given
 * by ACTION_*, defined above.
 */

static void do_all_directives(int action, ConfigDirective *directives)
{
    int n, i;

    for (n = 0; directives[n].name; n++) {
        ConfigDirective *d = &directives[n];
        for (i = 0; i < CONFIG_MAXPARAMS && d->params[i].type != CD_NONE; i++){
            CDValue val;

            /* Select the appropriate value to copy */
            if (action == ACTION_COPYNEW)
                val = d->params[i].new;
            else
                val = d->params[i].prev;

            /* In any case, we'll be rewriting the config variable, so free
             * the previous value if it was one we allocated */
            if (d->params[i].flags & CF_ALLOCED) {
                free(*(void **)d->params[i].ptr);
                d->params[i].flags &= ~CF_ALLOCED;
            }

            /* Don't do anything if we're copying new values and this
             * directive/parameter wasn't seen, or if we're restoring saved
             * values and this parameter hasn't had its value saved (except
             * for function parameters) */
            if (action == ACTION_COPYNEW
             && (!d->was_seen || !(d->params[i].flags & CF_WASSET)))
                continue;
            if (action == ACTION_RESTORESAVED
             && d->params[i].type != CD_FUNC
             && !(d->params[i].flags & CF_SAVED))
                continue;

            /* Copy new value to configuration variable */
            switch (d->params[i].type) {
              case CD_SET:
                if (action == ACTION_COPYNEW)
                    *(int *)d->params[i].ptr = (int)val.intval;
                break;
              case CD_TIME:
                *(time_t *)d->params[i].ptr = val.timeval;
                break;
              case CD_STRING:
                *(char **)d->params[i].ptr = val.ptrval;
                break;
              case CD_INT:
              case CD_POSINT:
              case CD_PORT:
              case CD_TIMEMSEC:
                *(int32 *)d->params[i].ptr = val.intval;
                break;
              case CD_FUNC: {
                int (*func)(const char *, int, char *)
                    = (int (*)(const char *,int,char *))(d->params[i].ptr);
                if (action == ACTION_COPYNEW)
                    func(NULL, CDFUNC_SET, NULL);
                else
                    func(NULL, CDFUNC_DECONFIG, NULL);
                break;
              } /* case CD_FUNC */
              case CD_DEPRECATED:
                /* Nothing to do */
                break;
              default:
                log("conffile: do_all_directives BUG: don't know how to "
                    " copy type %d (%s/%d)", d->params[i].type, d->name, i);
                break;
            } /* switch */

            /* Fix up flags */
            if (action == ACTION_COPYNEW) {
                if (d->params[i].flags & CF_ALLOCED_NEW) {
                    d->params[i].flags |= CF_ALLOCED;
                    /* The value is still allocated, but it's now stored in
                     * the configuration variable, so we don't want to free
                     * it when clearing `new' */
                    d->params[i].flags &= ~CF_ALLOCED_NEW;
                }
            } else {
                d->params[i].flags &= ~CF_SAVED;
            }
        } /* for each parameter */
    } /* for each directive */
}

/*************************************************************************/

/* Read in configuration options, and return nonzero for success, zero for
 * failure.  Performs the actions needed by configure(...,CONFIGURE_READ).
 */

static int read_config_file(const char *modulename,
                            ConfigDirective *directives)
{
    const char *filename;
    FILE *f;
    int retval, i, n;

    /* Open default file */
    filename = modulename==NULL ? IRCSERVICES_CONF : MODULES_CONF;
    f = fopen(filename, "r");
    if (!f) {
        log_perror("Unable to open %s", filename);
        if (!nofork && isatty(2))
            fprintf(stderr, "Unable to open %s: %s\n", filename,
                    strerror(errno));
        return 0;
    }

    /* Clear `was_set' flag and `new' value for all directives */
    for (n = 0; directives[n].name != NULL; n++) {
        directives[n].was_seen = 0;
        for (i = 0; i < CONFIG_MAXPARAMS; i++) {
            if (directives[n].params[i].flags & CF_ALLOCED_NEW)
                free(directives[n].params[i].new.ptrval);
            directives[n].params[i].flags &= ~(CF_WASSET | CF_ALLOCED_NEW);
            memset(&directives[n].params[i].new, 0,
                   sizeof(directives[n].params[i].new));
            if (directives[n].params[i].type == CD_FUNC) {
                int (*func)(const char *, int, char *)
                    = (int (*)(const char *, int, char *))
                        (directives[n].params[i].ptr);
                func(NULL, CDFUNC_INIT, NULL);
            }
        }
    }

    /* Actually do the work */
    retval = do_read_config_file(modulename, directives, f, filename, 0);

    fclose(f);

    /* Make sure all required directives were seen */
    for (n = 0; directives[n].name != NULL; n++) {
        if (!directives[n].was_seen
         && (directives[n].params[0].flags & CF_DIRREQ)
        ) {
            config_error(filename, 0, "Required directive `%s' missing",
                         directives[n].name);
            retval = 0;
        }
    }

    return retval;
}

/*************************************************************************/

/* Read in a single configuration file, recursively processing IncludeFile
 * directives.
 */

static int do_read_config_file(const char *modulename,
                               ConfigDirective *directives, FILE *f,
                               const char *filename, int recursion_level)
{
    char *current_module = NULL;  /* Current module in modules.conf */
    int retval = 1;               /* Return value */
    int linenum = 0;
    char buf[4096], tmpbuf[4096], *s;

    while (fgets(buf, sizeof(buf), f)) {
        /* Check for pathologically long files */
        if (linenum+1 < linenum) {
            config_error(filename, linenum, "File too long");
            retval = 0;
            break;
        }
        linenum++;
        /* Check for pathologically long lines */
        if (strlen(buf) == sizeof(buf)-1 && buf[sizeof(buf)-1] != '\n') {
            /* Report the maximum size as sizeof(buf)-3 to allow \r\n as
             * well as \n to fit */
            config_error(filename, linenum, "Line too long (%d bytes maximum)",
                         sizeof(buf)-3);
            /* Skip everything else until an EOL (or EOF) is seen */
            while (fgets(buf, sizeof(buf), f) && buf[strlen(buf)-1] != '\n')
                /*nothing*/;
            retval = 0;
        }
        /* Strip out comments (but don't touch # inside of quotes) */
        s = buf;
        while (*s) {
            if (*s == '"') {
                if (!(s = strchr(s+1, '"')))
                    break;
            } else if (*s == '#') {
                *s = 0;
                break;
            }
            s++;
        }
        /* Check for IncludeFile directives (use tmpbuf to avoid damaging
         * the original copy of the line) */
        strbcpy(tmpbuf, buf);
        s = strtok(tmpbuf, " \t\r\n");
        if (s && stricmp(s, "IncludeFile") == 0) {
            FILE *f2;
            /* Check recursion level for infinite loops */
            if (recursion_level > 100) {
                config_error(filename, linenum,
                             "IncludeFile recursion depth limit exceeded");
                retval = 0;
                continue;
            }
            /* Find the filename */
            s = strtok_remaining();
            if (s && *s == '"') {
                char *t = strchr(s+1, '"');
                if (!t) {
                    config_error(filename, linenum,
                                 "Missing closing double quote");
                    retval = 0;
                    continue;
                }
                *t = 0;
                s++;
            } else {
                s = strtok(s, " \t\r\n");
            }
            if (!s || !*s) {
                config_error(filename, linenum,
                             "Missing filename for IncludeFile");
                retval = 0;
                continue;
            }
            /* Valid filename string; try to open it */
            f2 = fopen(s, "r");
            if (!f2) {
                config_error(filename, linenum,
                             "Unable to open %s: %s", s, strerror(errno));
                retval = 0;
                continue;
            }
            if (!do_read_config_file(modulename, directives, f2, s,
                                     recursion_level+1))
                retval = 0;
            fclose(f2);
            continue;
        }
        /* Handle Module/EndModule lines specially, and don't parse lines
         * belonging to other modules */
        if (modulename) {
            if (current_module) {
                /* Inside a Module/EndModule pair: discard lines belonging
                 * to other modules, and handle EndModule directives.  If
                 * we reach EndModule for the module we're supposed to be
                 * processing, exit the loop to avoid unneeded processing. */
                strbcpy(tmpbuf, buf);
                s = strtok(tmpbuf, " \t\r\n");
                if (s && stricmp(s, "EndModule") == 0) {
                    int strcmp_result = strcmp(current_module, modulename);
                    free(current_module);
                    if (strcmp_result == 0)
                        break;  /* stop processing file, we're finished */
                    else
                        current_module = NULL;
                    continue;
                } else if (strcmp(current_module, modulename) != 0) {
                    continue;
                }
            } else {  /* !current_module */
                /* Outside a Module/EndModule pair: handle Module
                 * directives, and report errors for anything else */
                s = strtok(buf, " \t\r\n");
                if (!s)
                    continue;
                if (stricmp(s, "Module") != 0) {
                    config_error(filename, linenum,
                                 "Expected `Module' directive");
                    retval = 0;
                } else {
                    current_module = strtok(NULL, " \t\r\n");
                    if (!current_module) {
                        config_error(filename, linenum, "Module name missing");
                        retval = 0;
                    }
                    current_module = strdup(current_module);
                    if (!current_module) {
                        config_error(filename, linenum, "Out of memory");
                        retval = 0;
                        break;
                    }
                }
                continue;
            } /* if (current_module) */
        } /* if (modulename) */
        /* Valid line--parse it */
        if (!parse_config_line(filename, linenum, buf, directives))
            retval = 0;
    }

    return retval;
}

/*************************************************************************/

/* Parse a configuration line.  Return 1 on success; otherwise, print (and
 * log, if applicable) appropriate error message and return 0.  Destroys
 * the buffer by side effect.
 */

static int parse_config_line(const char *filename, int linenum, char *buf,
                             ConfigDirective *directives)
{
    char *s, *t, *directive;
    int i, n, optind;
    long longval;
    unsigned long ulongval;
    int retval = 1;
    int ac = 0;
    char *av[CONFIG_MAXPARAMS];

    directive = strtok(buf, " \t\r\n");
    s = strtok(NULL, "");
    if (s) {
        while (isspace(*s))
            s++;
        while (*s) {
            if (ac >= CONFIG_MAXPARAMS) {
                config_error(filename, linenum,
                             "Warning: too many parameters (%d max)",
                             CONFIG_MAXPARAMS);
                break;
            }
            t = s;
            if (*s == '"') {
                t++;
                s++;
                while (*s && *s != '"') {
                    if (*s == '\\' && s[1] != 0)
                        strmove(s, s+1);
                    s++;
                }
                if (!*s)
                    config_error(filename, linenum,
                                 "Warning: unterminated double-quoted string");
                else
                    *s++ = 0;
            } else {
                s += strcspn(s, " \t\r\n");
                if (*s)
                    *s++ = 0;
            }
            av[ac++] = t;
            while (isspace(*s))
                s++;
        }
    }

    if (!directive)
        return 1;

    for (n = 0; directives[n].name; n++) {
        ConfigDirective *d = &directives[n];
        if (stricmp(directive, d->name) != 0)
            continue;
        d->was_seen = 1;
        optind = 0;
        for (i = 0; i < CONFIG_MAXPARAMS && d->params[i].type != CD_NONE; i++){
            if (d->params[i].type == CD_SET) {
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int *)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                d->params[i].new.intval = 1;
                d->params[i].flags |= CF_WASSET;
                continue;
            }
            if (d->params[i].type == CD_DEPRECATED) {
                config_error(filename, linenum,
                             "Deprecated directive `%s' used", d->name);
                d->params[i].flags |= CF_WASSET;
                continue;
            }
            if (optind >= ac) {
                if (!(d->params[i].flags & CF_OPTIONAL)) {
                    config_error(filename, linenum,
                                 "Not enough parameters for `%s'", d->name);
                    retval = 0;
                }
                break;
            }
            switch (d->params[i].type) {
              case CD_INT:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32 *)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                longval = strtol(av[optind++], &s, 0);
                if (*s) {
                    config_error(filename, linenum,
                                 "%s: Expected an integer for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#if SIZEOF_LONG > 4
                if (longval < -0x80000000L || longval > 0x7FFFFFFFL) {
                    config_error(filename, linenum,
                                 "%s: Value out of range for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#endif
                d->params[i].new.intval = (int32)longval;
                break;
              case CD_POSINT:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32 *)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                ulongval = strtoul(av[optind++], &s, 0);
                if (*s || ulongval <= 0) {
                    config_error(filename, linenum,
                                 "%s: Expected a positive integer for"
                                 " parameter %d", d->name, optind);
                    retval = 0;
                    break;
                }
#if SIZEOF_LONG > 4
                if (ulongval > 0xFFFFFFFFL) {
                    config_error(filename, linenum,
                                 "%s: Value out of range for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
#endif
                d->params[i].new.intval = (int32)ulongval;
                break;
              case CD_PORT:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32 *)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                longval = strtol(av[optind++], &s, 0);
                if (*s) {
                    config_error(filename, linenum,
                                 "%s: Expected a port number for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
                if (longval < 1 || longval > 65535) {
                    config_error(filename, linenum,
                                 "Port numbers must be in the range 1..65535");
                    retval = 0;
                    break;
                }
                d->params[i].new.intval = (int32)longval;
                break;
              case CD_STRING:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.ptrval = *(char **)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                d->params[i].new.ptrval = strdup(av[optind++]);
                if (!d->params[i].new.ptrval) {
                    config_error(filename, linenum, "%s: Out of memory",
                                 d->name);
                    return 0;
                }
                d->params[i].flags |= CF_ALLOCED_NEW;
                break;
              case CD_TIME:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.timeval = *(time_t *)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                d->params[i].new.timeval = dotime(av[optind++]);
                if (d->params[i].new.timeval < 0) {
                    config_error(filename, linenum,
                                 "%s: Expected a time value for parameter %d",
                                 d->name, optind);
                    retval = 0;
                    break;
                }
                break;
              case CD_TIMEMSEC:
                if (!(d->params[i].flags & CF_SAVED)) {
                    d->params[i].prev.intval = *(int32 *)d->params[i].ptr;
                    d->params[i].flags |= CF_SAVED;
                }
                longval = strtol(av[optind++], &s, 10);
                if (longval < 0) {
                    config_error(filename, linenum,
                                 "%s: Expected a positive value for"
                                 " parameter %d", d->name, optind);
                    retval = 0;
                    break;
                } else if (longval > 1000000) {
                    config_error(filename, linenum,
                                 "%s: Value too large (maximum 1000000)",
                                 d->name);
                }
                longval *= 1000;
                if (*s == '.') {
                    int decimal = 0;
                    int count = 0;
                    s++;
                    while (count < 3 && isdigit(*s)) {
                        decimal = decimal*10 + (*s++ - '0');
                        count++;
                    }
                    while (count++ < 3)
                        decimal *= 10;
                    longval += decimal;
                    while (isdigit(*s))
                        s++;
                }
                if (*s) {
                    config_error(filename, linenum,
                                 "%s: Expected a decimal number for"
                                 " parameter %d", d->name, optind);
                    retval = 0;
                    break;
                }
                d->params[i].new.intval = (int32)longval;
                break;
              case CD_FUNC: {
                int (*func)(const char *, int, char *)
                    = (int (*)(const char *, int, char *))(d->params[i].ptr);
                if (!func(filename, linenum, av[optind++]))
                    retval = 0;
                break;
              }
              default:
                config_error(filename, linenum, "%s: Unknown type %d for"
                             " param %d", d->name, d->params[i].type, i+1);
                return 0;  /* don't bother continuing--something's bizarre */
            } /* switch (d->params[i].type) */
            d->params[i].flags |= CF_WASSET;
        } /* for all parameters */
        break;  /* because we found a match */
    } /* for all directives in array */

    if (!directives[n].name) {
        config_error(filename, linenum, "Unknown directive `%s'", directive);
        return 1;  /* don't cause abort */
    }

    return retval;
} /* parse_config_line() */

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
