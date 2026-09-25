/* httpd/main: what crosses between the server thread and the main thread.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The module is four files:
 *
 *     server.c   the server thread: Mongoose, the only file that knows it
 *                exists.  Does not include services.h (see worker.h).
 *     routes.c   the routes, the requests waiting for an answer and their
 *                deadline, the "httpd.auth" event.  Main thread.
 *     util.c     building responses, reading requests.  Main thread.
 *     main.c     configuration, starting and stopping.  Main thread.
 *
 * Across the line travels an HttpXfer -- bytes and a connection id, never
 * a pointer into anything either side might free -- allocated with
 * worker_alloc() by whichever side makes it and freed by the other.
 */

#ifndef HTTPD_INT_H
#define HTTPD_INT_H

#include "modules/httpd/http.h"
#include "worker.h"

/* Most ListenTo addresses. */
#define HTTPD_LISTEN_MAX 16

/* Longest listener URL ("https://[ffff:...]:65535") and file path. */
#define HTTPD_URL_MAX  128
#define HTTPD_PATH_MAX 255

/* A request on its way up, or an answer on its way down. */
struct HttpXfer {
    struct HttpXfer* hx_next; /* On the server thread's outbox */
    unsigned long hx_conn;    /* mg_connection::id */

    /* Going up. */
    char hx_method[HTTP_METHOD_MAX + 1];
    char hx_path[HTTP_PATH_MAX + 1];
    char hx_query[HTTP_QUERY_MAX + 1];
    char hx_remote[64];
    unsigned char hx_ip[16];
    int hx_ip6;
    int hx_tls;
    unsigned int hx_nheaders;
    struct HttpHeader hx_headers[HTTP_HEADERS_MAX];

    /* Coming down.  A status of 0 means nobody will answer: close. */
    int hx_status;
    char hx_type[HTTP_HVALUE_MAX + 1];
    unsigned int hx_nreply;
    struct HttpHeader hx_reply[HTTP_HEADERS_MAX];
    char hx_sendfile[HTTPD_PATH_MAX + 1];
    char hx_range[HTTP_HVALUE_MAX + 1];
    char hx_inm[HTTP_HVALUE_MAX + 1];

    /* The body, either way: `hx_bodylen' bytes and a NUL. */
    size_t hx_bodylen;
    char hx_body[];
};

/* A new transfer with room for `bodylen' bytes of body (and a NUL):
 * worker_alloc()ed, zeroed.  Either thread. */
extern struct HttpXfer* httpd_xfer_new(size_t bodylen);

/* What the server thread is started with: read from the configuration in
 * the main thread and handed over, because a worker never reads it. */
struct HttpdArgs {
    char ha_url[HTTPD_LISTEN_MAX][HTTPD_URL_MAX]; /* "http://1.2.3.4:80" */
    int ha_nurl;
    char ha_cert[HTTPD_PATH_MAX + 1]; /* PEM files, or "" */
    char ha_key[HTTPD_PATH_MAX + 1];
    int ha_max; /* Connections at once; 0 = any */
    /* Run in the main thread (as a task's wt_done) with a request (an
     * HttpXfer as wt_in), and with the result of the start (an
     * HttpdReady as wt_in). */
    WorkDoneFn ha_deliver;
    WorkDoneFn ha_ready;
};

/* How the start went: one line per listener. */
struct HttpdReady {
    int hr_ok[HTTPD_LISTEN_MAX]; /* Listening */
    char hr_error[256];          /* What failed before any listener */
    int hr_nurl;
    char hr_url[HTTPD_LISTEN_MAX][HTTPD_URL_MAX];
};

/* server.c, called from the main thread. */

/* Start the server thread, owned by `mod'.  Returns nonzero if the thread
 * is running; how the listening went arrives later (ha_ready). */
extern int httpd_server_start(struct Module_* mod,
                              const struct HttpdArgs* args);

/* Stop it and wait for it.  Answers not sent yet are dropped. */
extern void httpd_server_stop(void);

/* Nonzero while it runs (started, and not stopped). */
extern int httpd_server_running(void);

/* Hand an answer to the server thread, which frees it.  Returns nonzero
 * on success; zero (the transfer is freed) if the server is not
 * running. */
extern int httpd_server_send(struct HttpXfer* xfer);

/* routes.c */
extern void httpd_deliver(struct WorkTask* task);
extern void httpd_routes_shutdown(void);
extern void httpd_routes_cleanup(void);
extern void httpd_routes_drop_module(struct Module_* mod);
extern void httpd_routes_expire(void);
extern int httpd_routes_init(void);
extern int httpd_timeout_seconds; /* RequestTimeout */

/* util.c */
extern void httpd_response_init(struct HttpResponse* res);
extern void httpd_response_free(struct HttpResponse* res);

/* main.c */
extern int httpd_have_listeners(void);

#endif /* HTTPD_INT_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
