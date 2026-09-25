/* Debug page module for HTTP server.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Shows a request as httpd/main hands it to a module: what a module
 * writer sees.  One route (DebugURL), for GET and POST.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "modules/httpd/http.h"

/*************************************************************************/

static Module *module_httpd;

static char *DebugURL;

/* The URL the routes were claimed for (DebugURL may change on REHASH). */
static char *claimed;

/*************************************************************************/
/**************************** Request handler ****************************/
/*************************************************************************/

static int do_request(http_req_t id, const struct HttpRequest *req,
                      struct HttpResponse *res, void *user)
{
    unsigned int i;

    http_response_set(res, HTTP_S_OK, "text/plain; charset=utf-8", NULL, 0);
    http_response_printf(res, "id: %lu\n", id);
    http_response_printf(res, "method: %s\n", req->hreq_method);
    http_response_printf(res, "path: %s\n", req->hreq_path);
    http_response_printf(res, "query: %s\n", req->hreq_query);
    http_response_printf(res, "remote: %s\n", req->hreq_remote);
    http_response_printf(res, "tls: %s\n", req->hreq_tls ? "yes" : "no");
    http_response_printf(res, "headers: %u\n", req->hreq_nheaders);
    for (i = 0; i < req->hreq_nheaders; i++) {
        http_response_printf(res, "headers[%u]: %s: %s\n", i,
                             req->hreq_headers[i].hh_name,
                             req->hreq_headers[i].hh_value);
    }
    http_response_printf(res, "body: %lu bytes\n",
                         (unsigned long)req->hreq_bodylen);
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "DebugURL",         { { CD_STRING, CF_DIRREQ, &DebugURL } } },
    { NULL }
};

/*************************************************************************/

static void unclaim(void)
{
    if (claimed) {
        http_del_route(THIS_MODULE, "GET", claimed);
        http_del_route(THIS_MODULE, "POST", claimed);
        free(claimed);
        claimed = NULL;
    }
}

static int claim(void)
{
    unclaim();
    if (!http_add_route(THIS_MODULE, "GET", DebugURL, do_request, NULL)
     || !http_add_route(THIS_MODULE, "POST", DebugURL, do_request, NULL)
    ) {
        module_log("Unable to claim %s (not a path, or already claimed)",
                   DebugURL);
        http_del_routes(THIS_MODULE);
        return 0;
    }
    claimed = sstrdup(DebugURL);
    return 1;
}

static int do_reconfigure(int after_configure)
{
    if (after_configure && (!claimed || strcmp(claimed, DebugURL) != 0))
        claim();
    return 0;
}

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

    if (!add_callback(NULL, "reconfigure", do_reconfigure) || !claim()) {
        exit_module(0);
        return 0;
    }
    if (!http_available())
        module_log("httpd/main has no ListenTo: %s will not be reached",
                   DebugURL);

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    remove_callback(NULL, "reconfigure", do_reconfigure);
    if (module_httpd) {
        unclaim();
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
