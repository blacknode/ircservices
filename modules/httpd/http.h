/* HTTP: served by httpd/main, routed to whichever module asked.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/http.h).  See docs/readme.http.
 *
 * The server is Mongoose (vendor/mongoose/), running on a thread of its
 * own inside httpd/main; this header is what every other module sees of
 * it.  A module claims a ROUTE and is handed requests that are already
 * parsed, in the main thread, and never touches a socket:
 *
 *     http_add_route(THIS_MODULE, "GET", "/nick/", handle_nick, NULL);
 *
 * A path ending in `/' matches everything under it; anything else matches
 * exactly.  An exact match beats a prefix, and a longer prefix beats a
 * shorter one.  That is the whole routing language, on purpose.  A route
 * for GET also answers HEAD (the body is not sent).
 *
 * ANSWERING LATER.  A handler runs in the main thread and does not have to
 * answer there.  A handler that needs a record that is not in memory
 * fetches it in the background (store_prefetch()) and answers when it
 * arrives: it keeps the request's identifier, returns zero, and fills in
 * http_response(id) and calls http_respond(id, NULL) later.  Every request
 * has a deadline (RequestTimeout in the module block); one that passes is
 * answered 504 and a later answer is dropped.
 *
 * WHICH THREAD IS WHICH.  The socket is the server thread's and the
 * routes are the main thread's; nothing in this header may be called from
 * a worker thread, and nothing a handler is given outlives the call (copy
 * what an answer given later needs).
 *
 * A module that claims routes loads after httpd/main, calls use_module()
 * on it, and gives its routes up from exit_module() with
 * http_del_routes(THIS_MODULE) (httpd/main also drops them, and the
 * requests waiting on them, when the module is unloaded).
 *
 * This file does not include services.h, so that the server thread's own
 * source can include it.
 */

#ifndef HTTP_H
#define HTTP_H

#include <stdarg.h>
#include <stddef.h>

struct Module_;

/*************************************************************************/

/* Handle for one request being answered.  Zero is never a valid one. */
typedef unsigned long http_req_t;

/* Longest method name, path, query string, header name and header value
 * carried, without the NUL. */
#define HTTP_METHOD_MAX 15
#define HTTP_PATH_MAX   255
#define HTTP_QUERY_MAX  1024
#define HTTP_HNAME_MAX  63
#define HTTP_HVALUE_MAX 1023

/* Most headers carried in either direction. */
#define HTTP_HEADERS_MAX 32

/* Largest request body accepted.  A bigger one is refused (413) before any
 * of it has been read: nothing Services serve takes uploads. */
#define HTTP_BODY_MAX 65536

/* Largest response body a handler may build: what crosses from the main
 * thread to the server thread.  A bigger answer is a file
 * (http_response_file()), which the server thread streams. */
#define HTTP_REPLY_MAX (4 * 1024 * 1024)

/* Most routes one module may claim. */
#define HTTP_ROUTES_MAX 64

/* How long a handler has to answer, by default and at most (seconds). */
#define HTTP_TIMEOUT_DEFAULT 5
#define HTTP_TIMEOUT_MAX     30

/* Return codes of the "auth" callback of httpd/main (see below). */
#define HTTP_AUTH_UNDECIDED 0
#define HTTP_AUTH_ALLOW     1
#define HTTP_AUTH_DENY      2

/*************************************************************************/

/* HTTP reply codes.  Prefix letters are:
 *     I - 1xx Informational
 *     S - 2xx Successful
 *     R - 3xx Redirection
 *     E - 4xx Client Error
 *     F - 5xx Server Error (think "Failure")
 */

#define HTTP_S_OK                       200
#define HTTP_S_NO_CONTENT               204
#define HTTP_R_MOVED_PERMANENTLY        301
#define HTTP_R_FOUND                    302
#define HTTP_R_SEE_OTHER                303
#define HTTP_R_NOT_MODIFIED             304
#define HTTP_R_TEMPORARY_REDIRECT       307
#define HTTP_E_BAD_REQUEST              400
#define HTTP_E_UNAUTHORIZED             401
#define HTTP_E_FORBIDDEN                403
#define HTTP_E_NOT_FOUND                404
#define HTTP_E_METHOD_NOT_ALLOWED       405
#define HTTP_E_REQUEST_TIMEOUT          408
#define HTTP_E_LENGTH_REQUIRED          411
#define HTTP_E_REQUEST_ENTITY_TOO_LARGE 413
#define HTTP_F_INTERNAL_SERVER_ERROR    500
#define HTTP_F_NOT_IMPLEMENTED          501
#define HTTP_F_SERVICE_UNAVAILABLE      503
#define HTTP_F_GATEWAY_TIMEOUT          504

/*************************************************************************/

/* One header, in either direction. */
struct HttpHeader {
    char hh_name[HTTP_HNAME_MAX + 1];   /* Name, as it was written */
    char hh_value[HTTP_HVALUE_MAX + 1]; /* Value, trimmed */
};

/* A request, already parsed, as a handler sees it.  Valid until the
 * handler returns. */
struct HttpRequest {
    const char* hreq_method;   /* "GET", "POST"; uppercase */
    const char* hreq_path;     /* Path, decoded, without the query */
    const char* hreq_query;    /* After the '?', raw, or "" */
    const char* hreq_remote;   /* Who asked, as text */
    unsigned char hreq_ip[16]; /* Who asked: 4 bytes, or 16 for IPv6 */
    int hreq_ip6;              /* Nonzero for an IPv6 address */
    int hreq_tls;              /* Nonzero if it arrived over TLS */
    unsigned int hreq_nheaders;
    const struct HttpHeader* hreq_headers;
    const char* hreq_body; /* The body, NUL-terminated for convenience;
                            * may hold NULs */
    size_t hreq_bodylen;
};

/* What a handler answers with.  httpd/main owns the storage: a handler
 * fills it in with the http_response_*() functions below. */
struct HttpResponse {
    int hres_status;                     /* 200, 404, ... */
    char hres_type[HTTP_HVALUE_MAX + 1]; /* Content-Type */
    unsigned int hres_nheaders;          /* Extra headers */
    struct HttpHeader hres_headers[HTTP_HEADERS_MAX];
    char* hres_body;     /* Body (grows as it is written) */
    size_t hres_bodylen; /* Bytes in it */
    size_t hres_bodymax; /* Bytes allocated */
    int hres_overflow;   /* More than HTTP_REPLY_MAX was written */
    /* A file to send instead of the body, set by http_response_file(),
     * with the conditional headers of the request it answers. */
    char hres_file[HTTP_PATH_MAX + 1];
    char hres_range[HTTP_HVALUE_MAX + 1];
    char hres_inm[HTTP_HVALUE_MAX + 1]; /* If-None-Match */
};

/* Handles one request.  Runs in the main thread.  `id' is the handle to
 * answer with later; `res' is where to write the answer now.  Returns
 * nonzero if `res' has been filled in and should be sent, zero if the
 * handler will call http_respond(id, NULL) itself. */
typedef int (*HttpHandlerFn)(http_req_t id, const struct HttpRequest* req,
                             struct HttpResponse* res, void* user);

/*************************************************************************/

/* The "auth" callback of httpd/main: called for every request, before it
 * is routed, as
 *     int callback(const struct HttpRequest *req, struct HttpResponse *res)
 * and returns HTTP_AUTH_UNDECIDED to let the next callback (and then the
 * route) decide, HTTP_AUTH_ALLOW to let the request through without
 * asking the rest, or HTTP_AUTH_DENY after filling in `res' with the
 * refusal (a response left with status 0 is sent as 403). */

/*************************************************************************/

/* Routes. */

/* Nonzero when httpd/main has somewhere to listen (a ListenTo); whether
 * the listener is up this instant is not a module's business. */
extern int http_available(void);

/* Claim a route: `method' ("GET", "POST"; any case) and `path' (starting
 * with '/'; ending with '/' for everything under it), answered by `fn'
 * with `user'.  Returns nonzero on success; zero if the path is
 * malformed, already claimed for that method, or `mod' has too many. */
extern int http_add_route(struct Module_* mod, const char* method,
                          const char* path, HttpHandlerFn fn, void* user);

/* Give up one route.  Returns nonzero if it was found. */
extern int http_del_route(struct Module_* mod, const char* method,
                          const char* path);

/* Give up every route of `mod', and let go of the requests waiting on
 * them. */
extern void http_del_routes(struct Module_* mod);

/*************************************************************************/

/* Answering. */

/* The response of a request still waiting for its answer, to fill in;
 * NULL if it has been answered or has timed out (or never existed). */
extern struct HttpResponse* http_response(http_req_t id);

/* Send the answer of a request whose handler returned zero: `res', or the
 * request's own response (http_response(id)) if `res' is NULL.  Does
 * nothing for a request that has already been answered or has timed out:
 * a module that answers late has been slow, not wrong. */
extern void http_respond(http_req_t id, const struct HttpResponse* res);

/* Set the status, the content type (NULL: text/plain) and the body
 * (replacing whatever was written; NULL for none). */
extern void http_response_set(struct HttpResponse* res, int status,
                              const char* type, const char* body,
                              size_t bodylen);

/* Add one header.  Returns nonzero on success, zero if there is no room.
 * A header whose name or value holds a newline is dropped when sent. */
extern int http_response_header(struct HttpResponse* res, const char* name,
                                const char* value);

/* Append to the body. */
extern void http_response_write(struct HttpResponse* res, const char* data,
                                size_t len);
extern void http_response_printf(struct HttpResponse* res, const char* fmt,
                                 ...)
#ifdef __GNUC__
    __attribute__((format(printf, 2, 3)))
#endif
    ;
extern void http_response_vprintf(struct HttpResponse* res, const char* fmt,
                                  va_list args);

/* An HTML error page: `status', and the body `fmt' (HTML), or the
 * standard text for the status if `fmt' is NULL. */
extern void http_response_error(struct HttpResponse* res, int status,
                                const char* fmt, ...)
#ifdef __GNUC__
    __attribute__((format(printf, 3, 4)))
#endif
    ;

/* A redirect (`status' is 301, 302, 303 or 307) to `location'. */
extern void http_response_redirect(struct HttpResponse* res, int status,
                                   const char* location);

/* Answer with the contents of the file `path', streamed by the server
 * thread (it must still be there when the answer is sent), as `type' (or
 * NULL to guess from the name).  The Range and If-None-Match headers of
 * `req' travel with it.  Returns nonzero on success. */
extern int http_response_file(struct HttpResponse* res,
                              const struct HttpRequest* req, const char* path,
                              const char* type);

/*************************************************************************/

/* Reading a request. */

/* One header of a request, by name, case-insensitively, or NULL. */
extern const char* http_request_header(const struct HttpRequest* req,
                                       const char* name);

/* The variable `name' of a request -- from the query string, or from an
 * application/x-www-form-urlencoded body -- decoded into `buf' (of
 * `size' bytes).  Returns `buf', or NULL if there is no such variable. */
extern char* http_request_var(const struct HttpRequest* req, const char* name,
                              char* buf, size_t size);

/*************************************************************************/

/* Utilities. */

/* HTML-quote (&...;) the HTML-special characters of `str' (<, >, &, ")
 * into `outbuf' of `outsize' bytes; entities are never truncated.
 * Returns `outbuf'. */
extern char* http_quote_html(const char* str, char* outbuf, size_t outsize);

/* URL-quote (%nn) everything but A-Z a-z 0-9 - . _ of `str' into `outbuf'
 * of `outsize' bytes; escapes are never truncated.  Returns `outbuf'. */
extern char* http_quote_url(const char* str, char* outbuf, size_t outsize);

/* Remove URL-quoting (%nn, and + for a space) from `buf' in place.  An
 * incomplete or invalid %nn is discarded.  Returns `buf'. */
extern char* http_unquote_url(char* buf);

/* The reason phrase of a status code ("Not Found"), or "Unknown". */
extern const char* http_status_text(int status);

/*************************************************************************/

#endif /* HTTP_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
