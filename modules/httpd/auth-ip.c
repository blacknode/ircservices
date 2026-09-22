/* IP address authorization module for HTTP server.
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
#include "http.h"
#include <netinet/in.h>
#include <netdb.h>

/*************************************************************************/

static Module *module_httpd;

/* List of hosts to allow/deny */
typedef struct {
    char *path;
    int pathlen;        /* for convenience */
    uint32 ip, mask;    /* network byte order */
    int allow;          /* 1 = allow, 0 = deny */
} DirInfo;
static DirInfo *protected = NULL;
static int protected_count = 0;

/*************************************************************************/
/************************ Authorization callback *************************/
/*************************************************************************/

static int do_auth(Client *c, int *close_ptr)
{
    int i;

    ARRAY_FOREACH (i, protected) {
        if (strncmp(c->url, protected[i].path, protected[i].pathlen) != 0)
            continue;
        if ((c->ip & protected[i].mask) != protected[i].ip)
            continue;
        if (protected[i].allow) {
            return HTTP_AUTH_UNDECIDED;
        } else {
            module_log("Denying request for %s from %s", c->url, c->address);
            return HTTP_AUTH_DENY;
        }
    }
    return HTTP_AUTH_UNDECIDED;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_prefix(const char *filename, int linenum, char *param);
static int do_AllowHost(const char *filename, int linenum, char *param);
static int do_DenyHost(const char *filename, int linenum, char *param);
static int do_AllowDenyHost(const char *filename, int linenum, char *param,
                            int allow);
ConfigDirective module_config[] = {
    { "AllowHost",        { { CD_FUNC, 0, do_prefix },
                            { CD_FUNC, 0, do_AllowHost } } },
    { "DenyHost",         { { CD_FUNC, 0, do_prefix },
                            { CD_FUNC, 0, do_DenyHost } } },
    { NULL }
};

static char *prefix = NULL;

/*************************************************************************/

static int do_prefix(const char *filename, int linenum, char *param)
{
    if (filename) {
        free(prefix);
        prefix = strdup(param);
        if (!prefix) {
            config_error(filename, linenum, "Out of memory");
            return 0;
        }
    }
    return 1;
}

static int do_AllowHost(const char *filename, int linenum, char *param)
{
    return do_AllowDenyHost(filename, linenum, param, 1);
}

static int do_DenyHost(const char *filename, int linenum, char *param)
{
    return do_AllowDenyHost(filename, linenum, param, 0);
}

/*************************************************************************/

static int do_AllowDenyHost(const char *filename, int linenum, char *param,
                            int allow)
{
    char *s;
    int mask = 32;
    const uint8 *ip;
    int recursing = 0, i;
    DirInfo di;
    static DirInfo *new_protected = NULL;
    static int new_protected_count = 0;

    if (!filename) {
        /* filename == NULL, special actions */
        switch (linenum) {
          case CDFUNC_INIT:     /* prepare for reading */
            free(new_protected);
            new_protected = NULL;
            new_protected_count = 0;
            break;
          case CDFUNC_SET:      /* store new values in config variables */
            if (new_protected_count >= 0) {
                ARRAY_FOREACH (i, protected)
                    free(protected[i].path);
                free(protected);
                protected = new_protected;
                protected_count = new_protected_count;
                new_protected = NULL;
                new_protected_count = -1;  /* flag to say "don't copy again" */
            }
            break;
          case CDFUNC_DECONFIG: /* clear out config variables */
            ARRAY_FOREACH (i, protected)
                free(protected[i].path);
            free(protected);
            protected = NULL;
            protected_count = 0;
            break;
        } /* switch (linenum) */
        return 1;
    } /* if (!filename) */

    /* filename != NULL, process directive */

    if (linenum < 0) {
        recursing = 1;
        linenum = -linenum;
    }
    di.path = prefix;
    di.pathlen = strlen(prefix);
    prefix = NULL;

    s = strchr(param, '/');
    if (s) {
        *s++ = 0;
        mask = (int)atolsafe(s, 1, 31);
        if (mask < 1) {
            config_error(filename, linenum, "Invalid mask length `%s'", s);
            free(di.path);
            return 0;
        }
    }

    if (strcmp(param, "*") == 0) {
        /* All-hosts wildcard -> equivalent to 0.0.0.0/0 */
        ip = (const uint8 *)"\0\0\0\0";
        mask = 0;
    } else if ((ip = pack_ip(param)) != NULL) {
        /* IP address -> okay as is */
    } else {
        /* hostname -> check for double recursion, then look up and
         *             recursively add addresses */
#ifdef HAVE_GETHOSTBYNAME
        struct hostent *hp;
#endif
        if (recursing) {
            config_error(filename, linenum, "BUG: double recursion (param=%s)",
                         param);
            free(di.path);
            return 0;
        }
#ifdef HAVE_GETHOSTBYNAME
        if ((hp = gethostbyname(param)) != NULL) {
            if (hp->h_addrtype == AF_INET) {
                for (i = 0; hp->h_addr_list[i]; i++) {
                    char ipbuf[16];
                    ip = (const uint8 *)hp->h_addr_list[i];
                    snprintf(ipbuf, sizeof(ipbuf), "%u.%u.%u.%u",
                             ip[0], ip[1], ip[2], ip[3]);
                    if (strlen(ipbuf) > 15) {
                        config_error(filename, linenum,
                                     "BUG: strlen(ipbuf) > 15 [%s]", ipbuf);
                        free(di.path);
                        return 0;
                    }
                    prefix = strdup(di.path);
                    if (!prefix) {
                        config_error(filename, linenum, "Out of memory");
                        free(di.path);
                        return 0;
                    }
                    if (!do_AllowDenyHost(filename, -linenum, ipbuf, allow)) {
                        free(di.path);
                        return 0;
                    }
                }
                free(di.path);
                return 1;  /* Success */
            } else {                
                config_error(filename, linenum, "%s: no IPv4 addresses found",
                             param);
            }
        } else {
            config_error(filename, linenum, "%s: %s", param,
                         hstrerror(h_errno));
        }
#else
        config_error(filename, linenum,
                     "gethostbyname() not available, hostnames may not be"
                     " used");
#endif
        free(di.path);
        return 0;
    }

    di.ip = *((uint32 *)ip);
    di.mask = mask ? htonl(0xFFFFFFFFUL << (32-mask)) : 0;
    di.ip &= di.mask;
    di.allow = allow;
    ARRAY_EXTEND(new_protected);
    new_protected[new_protected_count-1] = di;
    return 1;
}

/*************************************************************************/
/*************************************************************************/

int init_module(void)
{
    module_httpd = find_module("httpd/main");
    if (!module_httpd) {
        module_log("Main httpd module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_httpd);

    if (!add_callback(module_httpd, "auth", do_auth)) {
        module_log("Unable to add callback");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (module_httpd) {
        remove_callback(module_httpd, "auth", do_auth);
        unuse_module(module_httpd);
        module_httpd = NULL;
    }

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
