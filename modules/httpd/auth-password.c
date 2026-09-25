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
#include "modules/httpd/http.h"

/*************************************************************************/


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

/* Compare two strings without saying where they differ. */
static int same_secret(const char *a, const char *b)
{
    size_t la = strlen(a), lb = strlen(b), i;
    unsigned char diff = la != lb;

    for (i = 0; i < la && i < lb; i++)
        diff |= (unsigned char)(a[i] ^ b[i]);
    return diff == 0;
}

static int do_auth(const struct HttpRequest *req, struct HttpResponse *res)
{
    int i;
    const char *authinfo;
    char realm[256];

    /* Search for a matching path prefix. */
    ARRAY_FOREACH (i, protected) {
        if (strncmp(req->hreq_path, protected[i].path,
                    protected[i].pathlen) == 0)
            break;
    }
    if (i >= protected_count) {
        /* No matching path prefix: return "undecided". */
        return HTTP_AUTH_UNDECIDED;
    }

    /* Check for an Authorization: header with basic credentials. */
    authinfo = http_request_header(req, "Authorization");
    if (authinfo && strnicmp(authinfo, "Basic", 5) == 0
        && (authinfo[5] == ' ' || authinfo[5] == '\t')) {
        authinfo += 5;
        /* Skip past any extra whitespace... */
        while (*authinfo == ' ' || *authinfo == '\t')
            authinfo++;
        /* ... then compare against the configured username/password. */
        if (same_secret(authinfo, protected[i].userpass)) {
            /* Allow the next authorization callback to check this
             * request.  In general, it is not a good idea to
             * explicitly allow requests unless the requesting user
             * has (for example) been authorized as the Services root
             * or otherwise should clearly have access despite any
             * other authorization checks.
             */
            return HTTP_AUTH_UNDECIDED;
        }
    }

    /* If the username or password are incorrect (or no Authorization:
     * header was supplied), deny the request.
     */
    http_response_error(res, HTTP_E_UNAUTHORIZED, NULL);
    snprintf(realm, sizeof(realm), "Basic realm=\"%s\"", AuthName);
    http_response_header(res, "WWW-Authenticate", realm);
    return HTTP_AUTH_DENY;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_Protect1(const char *filename, int linenum, char *param);
static int do_Protect2(const char *filename, int linenum, char *param);
static ConfigDirective auth_password_config[] = {
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

static int auth_password_init(Module *module)
{
    if (!event_attach(module, HTTPD_EVENT_AUTH, do_auth)) {
        module_log("Unable to attach to " HTTPD_EVENT_AUTH);
        return 0;
    }
    return 1;
}

/*************************************************************************/

static int auth_password_fini(Module *module, int shutdown)
{
    int i;

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

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HTTP: password-protected paths",
    .requires = MODULE_REQUIRES("httpd/main"),
    .config = auth_password_config,
    .init = auth_password_init,
    .fini = auth_password_fini,
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
