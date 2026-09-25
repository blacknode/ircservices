/* Top-page (http://services.example.net/) handler.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Claims "/" exactly.  A file is not read here: the server thread streams
 * it (http_response_file()), outside the main loop.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"

#include "modules/httpd/http.h"

/*************************************************************************/

static Module *module_httpd;

static const char *Filename = NULL;
static const char *ContentType = "text/html";
static const char *Redirect = NULL;

/*************************************************************************/
/**************************** Request handler ****************************/
/*************************************************************************/

static int do_request(http_req_t id, const struct HttpRequest *req,
                      struct HttpResponse *res, void *user)
{
    /* A route for "/" is a prefix, and so receives every path no other
     * module claimed: only the top page itself is ours. */
    if (strcmp(req->hreq_path, "/") != 0) {
        http_response_error(res, HTTP_E_NOT_FOUND, NULL);
    } else if (Redirect) {
        http_response_redirect(res, HTTP_R_FOUND, Redirect);
    } else if (Filename) {
        if (!http_response_file(res, req, Filename,
                                ContentType ? ContentType : "text/html"))
            http_response_error(res, HTTP_F_INTERNAL_SERVER_ERROR, NULL);
    } else {
        http_response_error(res, HTTP_E_NOT_FOUND, NULL);
    }
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "Filename",         { { CD_STRING, 0, &Filename },
                            { CD_STRING, CF_OPTIONAL, &ContentType } } },
    { "Redirect",         { { CD_STRING, 0, &Redirect } } },
    { NULL }
};

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

    if (!http_add_route(THIS_MODULE, "GET", "/", do_request, NULL)) {
        module_log("Unable to claim / (already claimed)");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (module_httpd) {
        http_del_routes(THIS_MODULE);
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
