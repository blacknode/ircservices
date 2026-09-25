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
#include "modules/httpd/http.h"
#include <netinet/in.h>
#include <netdb.h>

/*************************************************************************/


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

static int do_auth(const struct HttpRequest *req, struct HttpResponse *res)
{
    static const unsigned char v4mapped[12] =
        {0,0,0,0, 0,0,0,0, 0,0,0xFF,0xFF};
    uint32 ip = 0;
    int have_v4 = 0;
    int i;

    /* The rules are IPv4 addresses.  An IPv4 client arrives as such, or
     * over IPv6 as ::ffff:a.b.c.d; any other IPv6 client matches only a
     * rule for every address ("*"). */
    if (!req->hreq_ip6) {
        memcpy(&ip, req->hreq_ip, 4);
        have_v4 = 1;
    } else if (memcmp(req->hreq_ip, v4mapped, sizeof(v4mapped)) == 0) {
        memcpy(&ip, req->hreq_ip + 12, 4);
        have_v4 = 1;
    }

    ARRAY_FOREACH (i, protected) {
        if (strncmp(req->hreq_path, protected[i].path,
                    protected[i].pathlen) != 0)
            continue;
        if (protected[i].mask
            && (!have_v4 || (ip & protected[i].mask) != protected[i].ip))
            continue;
        if (protected[i].allow) {
            return HTTP_AUTH_UNDECIDED;
        } else {
            module_log("Denying request for %s from %s", req->hreq_path,
                       req->hreq_remote);
            http_response_error(res, HTTP_E_FORBIDDEN, NULL);
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
static ConfigDirective auth_ip_config[] = {
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
        struct hostent *hp;
        if (recursing) {
            config_error(filename, linenum, "BUG: double recursion (param=%s)",
                         param);
            free(di.path);
            return 0;
        }
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

static int auth_ip_init(Module *module)
{
    if (!event_attach(module, HTTPD_EVENT_AUTH, do_auth)) {
        module_log("Unable to attach to " HTTPD_EVENT_AUTH);
        return 0;
    }
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HTTP: allow or deny requests by client address",
    .requires = MODULE_REQUIRES("httpd/main"),
    .config = auth_ip_config,
    .init = auth_ip_init,
};

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
