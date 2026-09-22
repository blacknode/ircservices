/* Password authorization module for HTTP server.
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

/*************************************************************************/

static Module *module_httpd;

static char *AuthName;

typedef struct {
    char *path;
    int pathlen;  /* for convenience */
    char *userpass;
} DirInfo;
static DirInfo *protected = NULL;
static int protected_count = 0;

/*************************************************************************/
/************************ Authorization callback *************************/
/*************************************************************************/

static int do_auth(Client *c, int *close_ptr)
{
    int i;
    char *authinfo, *s;

    /* Search for a matching path prefix. */
    ARRAY_FOREACH (i, protected) {
        if (strncmp(c->url, protected[i].path, protected[i].pathlen) == 0)
            break;
    }
    if (i >= protected_count) {
        /* No matching path prefix: return "undecided". */
        return HTTP_AUTH_UNDECIDED;
    }

    /* Check for an Authorization: header. */
    authinfo = http_get_header(c, "Authorization");
    if (authinfo) {
        /* Retrieve the encoded username and password. */
        s = strchr(authinfo, ' ');
        /* Check and make sure they're actually there. */
        if (s) {
            /* Skip past any extra whitespace... */
            while (*s == ' ' || *s == '\t')
                s++;
            /* ... then compare against the configured username/password. */
            if (strcmp(s, protected[i].userpass) == 0) {
                /* Allow the next authorization callback to check this
                 * request.  In general, it is not a good idea to
                 * explicitly allow requests unless the requesting user
                 * has (for example) been authorized as the Services root
                 * or otherwise should clearly have access despite any
                 * other suthorization checks.
                 */
                return HTTP_AUTH_UNDECIDED;
            }
        }
    }

    /* If the username or password are incorrect (or no Authorization:
     * header was supplied, deny the request.
     */
    http_send_response(c, HTTP_E_UNAUTHORIZED);
    sockprintf(c->socket, "WWW-Authenticate: basic realm=%s\r\n", AuthName);
    sockprintf(c->socket, "Content-Type: text/html\r\n");
    sockprintf(c->socket, "Content-Length: 14\r\n\r\n");
    sockprintf(c->socket, "Access denied.");
    return HTTP_AUTH_DENY;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_Protect1(const char *filename, int linenum, char *param);
static int do_Protect2(const char *filename, int linenum, char *param);
ConfigDirective module_config[] = {
    { "AuthName",         { { CD_STRING, CF_DIRREQ, &AuthName } } },
    { "Protect",          { { CD_FUNC, 0, do_Protect1 },
                            { CD_FUNC, 0, do_Protect2 } } },
    { NULL }
};

static char *protect_param1 = NULL;

/*************************************************************************/

static int do_Protect1(const char *filename, int linenum, char *param)
{
    if (filename) {
        free(protect_param1);  /*in case a previous line had only 1 parameter*/
        protect_param1 = strdup(param);
        if (!protect_param1) {
            config_error(filename, linenum, "Out of memory");
            return 0;
        }
    }
    return 1;
}

/*************************************************************************/

static int do_Protect2(const char *filename, int linenum, char *param)
{
    DirInfo di;
    char *s;
    int bufsize = 0, i;
    static DirInfo *new_protected = NULL;
    static int new_protected_count = 0;

    if (!filename) {
        /* filename == NULL, do special handling */
        switch (linenum) {
          case CDFUNC_INIT:     /* prepare for reading */
            ARRAY_FOREACH (i, new_protected) {
                free(new_protected[i].path);
                free(new_protected[i].userpass);
            }
            free(new_protected);
            new_protected = NULL;
            new_protected_count = 0;
            break;
          case CDFUNC_SET:      /* store new values in config variables */
            if (new_protected_count >= 0) {
                ARRAY_FOREACH (i, protected) {
                    free(protected[i].path);
                    free(protected[i].userpass);
                }
                free(protected);
                protected = new_protected;
                protected_count = new_protected_count;
                new_protected = NULL;
                new_protected_count = -1;  /* flag to say "don't copy again" */
            }
            break;
          case CDFUNC_DECONFIG: /* clear out config variables */
            ARRAY_FOREACH (i, protected) {
                free(protected[i].path);
                free(protected[i].userpass);
            }
            free(protected);
            protected = NULL;
            protected_count = 0;
            break;
        } /* switch (linenum) */
        return 1;
    } /* if (!filename) */

    /* filename != NULL, process directive */

    /* Move path parameter to DirInfo and clear temporary path holder */
    if (!protect_param1) {
        module_log("config: BUG: missing first parameter for Protect!");
        config_error(filename, linenum, "Internal error");
        return 0;
    }
    di.path = protect_param1;
    protect_param1 = NULL;
    di.pathlen = strlen(di.path);

    /* Check for colon in username/password string */
    s = strchr(param, ':');
    if (!s) {
        config_error(filename, linenum,
                     "Second parameter to Protect must be in the form"
                     " `username:password'");
        return 0;
    }

    /* base64-encode and store in di.userpass */
    bufsize = encode_base64(param, strlen(param), NULL, 0);
    if (bufsize <= 0) {
        config_error(filename, linenum, "Internal error: base64 encoding"
                     " failed");
        free(di.path);
        return 0;
    }
    di.userpass = malloc(bufsize);
    if (!di.userpass) {
        config_error(filename, linenum, "Out of memory");
        free(di.path);
        return 0;
    }
    if (encode_base64(param, strlen(param), di.userpass, bufsize) != bufsize) {
        config_error(filename, linenum, "Internal error: base64 encoding"
                     " failed");
        free(di.userpass);
        free(di.path);
        return 0;
    }

    /* Store new record in array and return success */
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
    int i;

    if (module_httpd) {
        remove_callback(module_httpd, "auth", do_auth);
        unuse_module(module_httpd);
        module_httpd = NULL;
    }

    ARRAY_FOREACH (i, protected) {
        free(protected[i].path);
        free(protected[i].userpass);
    }
    free(protected);
    protected = NULL;
    protected_count = 0;

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
