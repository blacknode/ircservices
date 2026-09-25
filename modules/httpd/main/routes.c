/* httpd/main: the routes, and the requests waiting for an answer.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/http.c).  No socket here and no parsing -- those
 * are server.c's, and Mongoose's under it.  What is here is a list of
 * routes, the requests waiting for an answer, the deadline that bounds
 * them, and the "auth" callback every request goes through first.  Main
 * thread only.
 */

#include "modules.h"
#include "services.h"
#include "timeout.h"

#include "httpd_int.h"

/*************************************************************************/

/* One route a module has claimed. */
typedef struct httproute_ HttpRoute;
struct httproute_ {
    HttpRoute *next, *prev;
    Module* mod;
    char method[HTTP_METHOD_MAX + 1]; /* Uppercase */
    char path[HTTP_PATH_MAX + 1];
    int prefix; /* It ends in '/': everything under it */
    HttpHandlerFn fn;
    void* user;
};

/* One request whose answer is being waited for. */
typedef struct httpcall_ HttpCall;
struct httpcall_ {
    HttpCall *next, *prev;
    http_req_t id;      /* The handle modules see */
    unsigned long conn; /* The server's connection id */
    Module* mod;        /* Whose route it went to */
    time_t deadline;    /* When it is answered 504 */
    struct HttpResponse res;
};

static HttpRoute* routes;
static HttpCall* calls;
static unsigned long last_id;

/* The "auth" callback. */
static int cb_auth = -1;

/* How long a handler has (RequestTimeout). */
int httpd_timeout_seconds = HTTP_TIMEOUT_DEFAULT;

/*************************************************************************/
/*************************************************************************/

/* Sending. */

/* Hand `res' to the server thread for connection `conn'. */
static void send_response(unsigned long conn, const struct HttpResponse* res)
{
    struct HttpXfer* xfer;
    size_t len = res->hres_bodylen;
    unsigned int i;
    int status = res->hres_status ? res->hres_status : HTTP_S_OK;

    if (res->hres_overflow) {
        /* A page cut short is worse than no page. */
        module_log("A response was larger than %d bytes; sending 500",
                   HTTP_REPLY_MAX);
        len = 0;
        status = HTTP_F_INTERNAL_SERVER_ERROR;
    }
    if (res->hres_file[0])
        len = 0;
    if (!(xfer = httpd_xfer_new(len)))
        return;
    xfer->hx_conn = conn;
    xfer->hx_status = status;
    if (res->hres_type[0])
        strbcpy(xfer->hx_type, res->hres_type);
    else if (!res->hres_file[0])
        strbcpy(xfer->hx_type, "text/plain");
    if (res->hres_overflow)
        strbcpy(xfer->hx_type, "text/plain");
    for (i = 0; i < res->hres_nheaders && i < HTTP_HEADERS_MAX; i++)
        xfer->hx_reply[xfer->hx_nreply++] = res->hres_headers[i];
    if (res->hres_file[0]) {
        strbcpy(xfer->hx_sendfile, res->hres_file);
        strbcpy(xfer->hx_range, res->hres_range);
        strbcpy(xfer->hx_inm, res->hres_inm);
    }
    if (len)
        memcpy(xfer->hx_body, res->hres_body, len);
    xfer->hx_bodylen = len;
    httpd_server_send(xfer);
}

/* Tell the server thread to let a connection go without an answer. */
static void send_cancel(unsigned long conn)
{
    struct HttpXfer* xfer = httpd_xfer_new(0);

    if (xfer) {
        xfer->hx_conn = conn;
        xfer->hx_status = 0;
        httpd_server_send(xfer);
    }
}

/* Send a status and a short page, for a request no module owns. */
static void send_status(unsigned long conn, int status)
{
    struct HttpResponse res;

    httpd_response_init(&res);
    http_response_error(&res, status, NULL);
    send_response(conn, &res);
    httpd_response_free(&res);
}

/*************************************************************************/

static void free_call(HttpCall* call)
{
    LIST_REMOVE(call, calls);
    httpd_response_free(&call->res);
    free(call);
}

static HttpCall* find_call(http_req_t id)
{
    HttpCall* call;

    LIST_FOREACH(call, calls)
    {
        if (call->id == id)
            return call;
    }
    return NULL;
}

/*************************************************************************/
/*************************************************************************/

/* Routes. */

static HttpRoute* route_exact(const char* method, const char* path)
{
    HttpRoute* r;

    LIST_FOREACH(r, routes)
    {
        if (stricmp(r->method, method) == 0 && strcmp(r->path, path) == 0)
            return r;
    }
    return NULL;
}

/* The route that should answer, or NULL.  An exact match wins over a
 * prefix, and a longer prefix over a shorter one. */
static HttpRoute* route_find(const char* method, const char* path)
{
    HttpRoute *r, *best = NULL;
    size_t bestlen = 0;

    LIST_FOREACH(r, routes)
    {
        size_t len;
        if (stricmp(r->method, method) != 0)
            continue;
        if (strcmp(r->path, path) == 0)
            return r;
        if (!r->prefix)
            continue;
        len = strlen(r->path);
        if (strncmp(r->path, path, len) == 0 && len > bestlen) {
            best = r;
            bestlen = len;
        }
    }
    return best;
}

EXPORT_FUNC(http_add_route)
int http_add_route(Module* mod, const char* method, const char* path,
                   HttpHandlerFn fn, void* user)
{
    HttpRoute* r;
    int count = 0;
    char* s;

    if (!method || !*method || !path || path[0] != '/' || !fn)
        return 0;
    if (strlen(method) > HTTP_METHOD_MAX || strlen(path) > HTTP_PATH_MAX)
        return 0;
    if (route_exact(method, path))
        return 0;
    LIST_FOREACH(r, routes)
    {
        if (r->mod == mod)
            count++;
    }
    if (count >= HTTP_ROUTES_MAX)
        return 0;

    r = scalloc(1, sizeof(*r));
    r->mod = mod;
    strbcpy(r->method, method);
    for (s = r->method; *s; s++)
        *s = toupper((unsigned char)*s);
    strbcpy(r->path, path);
    r->prefix = path[strlen(path) - 1] == '/';
    r->fn = fn;
    r->user = user;
    LIST_INSERT(r, routes);
    return 1;
}

EXPORT_FUNC(http_del_route)
int http_del_route(Module* mod, const char* method, const char* path)
{
    HttpRoute* r;

    LIST_FOREACH(r, routes)
    {
        if (r->mod == mod && stricmp(r->method, method) == 0 &&
            strcmp(r->path, path) == 0) {
            LIST_REMOVE(r, routes);
            free(r);
            return 1;
        }
    }
    return 0;
}

EXPORT_FUNC(http_del_routes)
void http_del_routes(Module* mod)
{
    HttpRoute *r, *r2;
    HttpCall *call, *call2;

    LIST_FOREACH_SAFE(r, routes, r2)
    {
        if (r->mod == mod) {
            LIST_REMOVE(r, routes);
            free(r);
        }
    }
    /* A request whose handler is going will never be answered: the
     * connection is let go rather than held until the deadline. */
    LIST_FOREACH_SAFE(call, calls, call2)
    {
        if (call->mod == mod) {
            send_cancel(call->conn);
            free_call(call);
        }
    }
}

void httpd_routes_drop_module(Module* mod)
{
    http_del_routes(mod);
}

/*************************************************************************/
/*************************************************************************/

/* Requests. */

/* A request has arrived from the server thread (wt_done of its task). */
void httpd_deliver(struct WorkTask* task)
{
    struct HttpXfer* xfer = task->wt_in;
    struct HttpRequest req;
    struct HttpResponse authres;
    const char* method;
    HttpRoute* route;
    HttpCall* call;
    int res;

    if (!xfer || !httpd_server_running())
        return;

    memset(&req, 0, sizeof(req));
    req.hreq_method = xfer->hx_method;
    req.hreq_path = xfer->hx_path;
    req.hreq_query = xfer->hx_query;
    req.hreq_remote = xfer->hx_remote;
    memcpy(req.hreq_ip, xfer->hx_ip, sizeof(req.hreq_ip));
    req.hreq_ip6 = xfer->hx_ip6;
    req.hreq_tls = xfer->hx_tls;
    req.hreq_nheaders = xfer->hx_nheaders;
    req.hreq_headers = xfer->hx_headers;
    xfer->hx_body[xfer->hx_bodylen] = 0;
    req.hreq_body = xfer->hx_body;
    req.hreq_bodylen = xfer->hx_bodylen;

    /* Access control first, for every request: a 401 must not depend on
     * whether there is anything at that path. */
    httpd_response_init(&authres);
    res = call_callback_2(cb_auth, &req, &authres);
    if (res < 0) {
        module_log("The auth callback failed for %s", req.hreq_path);
        httpd_response_free(&authres);
        send_status(xfer->hx_conn, HTTP_F_INTERNAL_SERVER_ERROR);
        return;
    }
    else if (res == HTTP_AUTH_DENY) {
        if (!authres.hres_status)
            http_response_error(&authres, HTTP_E_FORBIDDEN, NULL);
        send_response(xfer->hx_conn, &authres);
        httpd_response_free(&authres);
        return;
    }
    httpd_response_free(&authres);

    /* A route for GET also answers HEAD; the server thread leaves out the
     * body. */
    method = stricmp(req.hreq_method, "HEAD") == 0 ? "GET" : req.hreq_method;
    route = route_find(method, req.hreq_path);
    if (!route) {
        /* Nothing claimed it: the answer is ours, and nothing is held. */
        send_status(xfer->hx_conn, HTTP_E_NOT_FOUND);
        return;
    }

    call = scalloc(1, sizeof(*call));
    call->id = ++last_id;
    if (!call->id)
        call->id = ++last_id;
    call->conn = xfer->hx_conn;
    call->mod = route->mod;
    call->deadline = time(NULL) + httpd_timeout_seconds;
    httpd_response_init(&call->res);
    call->res.hres_status = HTTP_S_OK;
    LIST_INSERT(call, calls);

    if ((*route->fn)(call->id, &req, &call->res, route->user)) {
        /* The handler may have called http_respond() itself and also
         * returned nonzero; then the call is gone and there is nothing to
         * send twice. */
        if ((call = find_call(call->id)) != NULL) {
            send_response(call->conn, &call->res);
            free_call(call);
        }
    }
}

EXPORT_FUNC(http_response)
struct HttpResponse* http_response(http_req_t id)
{
    HttpCall* call = find_call(id);

    return call ? &call->res : NULL;
}

EXPORT_FUNC(http_respond)
void http_respond(http_req_t id, const struct HttpResponse* res)
{
    HttpCall* call = find_call(id);

    /* Already answered, or timed out: a module that answers late has been
     * slow, not wrong. */
    if (!call)
        return;
    send_response(call->conn, res ? res : &call->res);
    free_call(call);
}

/* Fail every request whose deadline has passed: a gateway timeout is the
 * truthful answer (something behind this server was asked and did not
 * reply), and holding the socket open instead is the failure the
 * deadline exists to bound.  Called every second. */
void httpd_routes_expire(void)
{
    HttpCall *call, *call2;
    time_t now = time(NULL);

    LIST_FOREACH_SAFE(call, calls, call2)
    {
        if (call->deadline <= now) {
            module_log("A handler did not answer %s in time; sending 504",
                       call->mod ? get_module_name(call->mod) : "(core)");
            send_status(call->conn, HTTP_F_GATEWAY_TIMEOUT);
            free_call(call);
        }
    }
}

/*************************************************************************/

int httpd_routes_init(void)
{
    cb_auth = register_callback("auth");
    return cb_auth >= 0;
}

/* The listener is going: every request in flight goes with it, silently,
 * because there is nobody left to answer through.  The routes stay. */
void httpd_routes_shutdown(void)
{
    while (calls)
        free_call(calls);
}

/* The module is going. */
void httpd_routes_cleanup(void)
{
    httpd_routes_shutdown();
    while (routes) {
        HttpRoute* r = routes;
        LIST_REMOVE(r, routes);
        free(r);
    }
    if (cb_auth >= 0) {
        unregister_callback(cb_auth);
        cb_auth = -1;
    }
}

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
