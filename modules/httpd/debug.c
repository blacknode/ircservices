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

static ConfigDirective debug_config[] = {
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

static void debug_rehash(Module *module)
{
    if (!claimed || strcmp(claimed, DebugURL) != 0)
        claim();
}

/*************************************************************************/

static int debug_init(Module *module)
{
    if (!claim())
        return 0;
    if (!http_available())
        module_log("httpd/main has no ListenTo: %s will not be reached",
                   DebugURL);

    return 1;
}

/*************************************************************************/

static int debug_fini(Module *module, int shutdown)
{
    unclaim();
    http_del_routes(module);
    return 1;
}

/*************************************************************************/

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HTTP: a page that echoes the request, for debugging",
    .requires = MODULE_REQUIRES("httpd/main"),
    .config = debug_config,
    .init = debug_init,
    .fini = debug_fini,
    .rehash = debug_rehash,
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
