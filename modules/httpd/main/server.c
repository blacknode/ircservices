/* httpd/main: the server thread.  Mongoose, on a dedicated worker.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/http_server.c).  See docs/readme.http.
 *
 * The only file in the tree that knows Mongoose exists.  Everything else
 * sees modules/httpd/http.h -- routes, a request, a response -- and could
 * not tell what is underneath.
 *
 * WHY A LIBRARY.  This replaced a hand-written parser that worked.  The
 * argument is not that it was broken; it is that HTTP is a protocol where
 * being nearly right is a security bug, and the list of ways to get
 * framing wrong -- chunked encoding, folded headers, pipelining,
 * Content-Length against Transfer-Encoding -- is a list somebody else has
 * already walked, with fuzzers, for twenty years.  TLS came with it.
 * vendor/mongoose/ is upstream's and is never edited.
 *
 * WHICH THREAD IS WHICH.  Mongoose has an event loop of its own and
 * Services have one already, so it runs on a thread of its own:
 *
 *   - up: MG_EV_HTTP_MSG copies the request into an HttpXfer and
 *     worker_post()s it; the task's wt_done (httpd_deliver() in routes.c)
 *     runs in the main thread.
 *   - down: the main thread puts the answer on an outbox under a mutex and
 *     rings mg_wakeup(), which wakes the poll; the thread empties the
 *     outbox after every poll.  (ircu2 passes the answer through
 *     mg_wakeup() itself; but on POSIX that is a datagram sent without
 *     waiting, dropped silently when the socket buffer is full -- and with
 *     it the answer, and the memory.  Here a lost wake-up costs at most
 *     one poll interval.)
 *
 * Across that line travels a struct of bytes and a connection id --
 * mg_connection::id, an integer Mongoose guarantees unique -- never a
 * pointer into either side.  A connection that went away while its answer
 * was being worked out is simply not found, which is the ordinary case for
 * a client that pressed stop.
 *
 * Nothing here touches Services state, and this file does not include
 * services.h: no User, no log(), no smalloc().  Mongoose's own logging is
 * compiled out for the same reason; what it has to say about a failed
 * listen comes back in the HttpdReady the thread posts.
 */

#include "httpd_int.h"

#include "mongoose.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* How long a poll waits, in milliseconds: the longest an answer waits if
 * its wake-up was lost, and how soon a stop is noticed. */
#define HTTPD_POLL_MS 200

/* What a connection remembers about the request it is answering, in
 * mg_connection::data. */
#define CONN_HEAD  0 /* The request was HEAD: no body in the answer */
#define CONN_CLOSE 1 /* Close once the answer has gone */

/*************************************************************************/

/* Main thread's. */
static struct Worker* httpd_worker; /* The thread, or NULL */

/* The manager.  Initialised by the thread before it reports, freed by the
 * thread after the main thread has stopped sending (httpd_server_stop()
 * clears httpd_worker first and then waits). */
static struct mg_mgr httpd_mgr;

/* Answers on their way down, and the connection whose id rings the bell
 * (a listener: the wake-up is only there to end the poll). */
static pthread_mutex_t outbox_lock = PTHREAD_MUTEX_INITIALIZER;
static struct HttpXfer *outbox_head, *outbox_tail;
static unsigned long doorbell;

/* Server thread's. */
static struct Worker* httpd_self;
static WorkDoneFn httpd_deliver_fn;
static int httpd_conns, httpd_conn_max;
static struct mg_str httpd_cert, httpd_key;

static void httpd_event(struct mg_connection* c, int ev, void* ev_data);

/*************************************************************************/
/*************************************************************************/

/* Either thread. */

struct HttpXfer* httpd_xfer_new(size_t bodylen)
{
    return worker_alloc(sizeof(struct HttpXfer) + bodylen + 1);
}

const char* http_status_text(int status)
{
    switch (status) {
        case 100:
            return "Continue";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 204:
            return "No Content";
        case 206:
            return "Partial Content";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Found";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 416:
            return "Range Not Satisfiable";
        case 429:
            return "Too Many Requests";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        default:
            return "Unknown";
    }
}

/*************************************************************************/
/*************************************************************************/

/* The server thread. */

/* Copy an mg_str into a fixed buffer, truncating and NUL-terminating. */
static void httpd_copy(char* dst, size_t dstlen, struct mg_str s)
{
    size_t n = s.len < dstlen - 1 ? s.len : dstlen - 1;

    if (n && s.buf)
        memcpy(dst, s.buf, n);
    dst[n] = '\0';
}

/* Decode the request path into `dst', or refuse it.
 *
 * Mongoose does the framing; what it deliberately does not do is decide
 * what a path means, and this is that decision.  An encoded separator
 * (%2F, %5C) is refused rather than decoded: routing happens on the
 * decoded path, so it would turn what the client wrote as data into a
 * separator and with it reach a prefix route it is not under.  So is an
 * encoded NUL (%00), which shortens the path for whoever reads it with
 * str*() next.  And after decoding, a path holding ".." or a control byte
 * is refused: nothing here opens a file, but the path is handed to modules
 * and one of them may, and the check belongs where the path is first
 * believed.  Returns nonzero if `dst' holds a path to route on. */
static int httpd_path(char* dst, size_t dstlen, struct mg_str uri)
{
    const char* p;
    size_t i;
    int n;

    for (i = 0; uri.buf && i + 2 < uri.len; i++) {
        if (uri.buf[i] != '%')
            continue;
        if (!strncasecmp(uri.buf + i + 1, "2f", 2) ||
            !strncasecmp(uri.buf + i + 1, "5c", 2) ||
            !strncmp(uri.buf + i + 1, "00", 2))
            return 0;
    }

    n = mg_url_decode(uri.buf, uri.len, dst, dstlen, 0);
    if (n < 0)
        return 0;
    dst[n] = '\0';
    if (dst[0] != '/')
        return 0;
    for (p = dst; *p; p++) {
        if ((unsigned char)*p < 0x20 || *p == 0x7f)
            return 0;
        if (p[0] == '.' && p[1] == '.')
            return 0;
    }
    return 1;
}

/* A status-only answer, from this thread. */
static void httpd_status(struct mg_connection* c, int status)
{
    mg_http_reply(c, status, "Content-Type: text/plain\r\n", "%d %s\n", status,
                  http_status_text(status));
}

/* Refuse a request whose body is still on its way.  Answering is not
 * enough: Mongoose would read the body as a request of its own and have
 * it answered a second time.  is_resp stops the parsing, and is_draining
 * closes the connection once the answer has gone. */
static void httpd_refuse(struct mg_connection* c, int status)
{
    httpd_status(c, status);
    c->is_resp = 1;
    c->is_draining = 1;
}

/* The headers are in and the body is not: the only moment a body that is
 * too big can be refused before it is read. */
static void httpd_headers(struct mg_connection* c, struct mg_http_message* hm)
{
    struct mg_str* cl = mg_http_get_header(hm, "Content-Length");

    if (cl) {
        char buf[32];
        httpd_copy(buf, sizeof(buf), *cl);
        if (strtoul(buf, NULL, 10) > HTTP_BODY_MAX)
            httpd_refuse(c, 413);
    }
    else if (mg_http_get_header(hm, "Transfer-Encoding")) {
        /* Chunked: no length until it ends, and Mongoose keeps what has
         * arrived in memory meanwhile (this event comes again as more
         * does).  Past the limit, with room for the chunk framing, it is
         * refused. */
        if (c->recv.len > hm->head.len + HTTP_BODY_MAX + HTTP_BODY_MAX / 8)
            httpd_refuse(c, 413);
    }
}

/* Turn a parsed request into a transfer and post it to the main thread.
 * Returns nonzero if it went up, or was answered here. */
static int httpd_request(struct mg_connection* c, struct mg_http_message* hm)
{
    struct HttpXfer* xfer;
    struct WorkTask* task;
    struct mg_str* conn;
    size_t i;

    if (hm->body.len > HTTP_BODY_MAX) {
        httpd_refuse(c, 413);
        return 1;
    }
    if (!(xfer = httpd_xfer_new(hm->body.len)))
        return 0;
    xfer->hx_conn = c->id;
    xfer->hx_tls = c->is_tls;
    httpd_copy(xfer->hx_method, sizeof(xfer->hx_method), hm->method);
    /* The query is carried raw: "+" means a space there and nowhere else,
     * and a server that decided would be deciding for every route. */
    httpd_copy(xfer->hx_query, sizeof(xfer->hx_query), hm->query);
    if (!httpd_path(xfer->hx_path, sizeof(xfer->hx_path), hm->uri)) {
        worker_free(xfer);
        httpd_status(c, 400);
        return 1;
    }
    mg_snprintf(xfer->hx_remote, sizeof(xfer->hx_remote), "%M", mg_print_ip,
                &c->rem);
    memcpy(xfer->hx_ip, c->rem.addr.ip, 16);
    xfer->hx_ip6 = c->rem.is_ip6;
    for (i = 0; i < MG_MAX_HTTP_HEADERS && hm->headers[i].name.len; i++) {
        struct HttpHeader* h;
        if (xfer->hx_nheaders >= HTTP_HEADERS_MAX)
            break;
        h = &xfer->hx_headers[xfer->hx_nheaders++];
        httpd_copy(h->hh_name, sizeof(h->hh_name), hm->headers[i].name);
        httpd_copy(h->hh_value, sizeof(h->hh_value), hm->headers[i].value);
    }
    if (hm->body.len)
        memcpy(xfer->hx_body, hm->body.buf, hm->body.len);
    xfer->hx_bodylen = hm->body.len;

    /* What the answer needs to know about the request. */
    c->data[CONN_HEAD] = mg_strcasecmp(hm->method, mg_str("HEAD")) == 0;
    conn = mg_http_get_header(hm, "Connection");
    if (mg_strcasecmp(hm->proto, mg_str("HTTP/1.0")) == 0)
        c->data[CONN_CLOSE] =
            !(conn && mg_strcasecmp(*conn, mg_str("keep-alive")) == 0);
    else
        c->data[CONN_CLOSE] =
            conn && mg_strcasecmp(*conn, mg_str("close")) == 0;

    if (!(task = worker_task_new(NULL, httpd_deliver_fn))) {
        worker_free(xfer);
        return 0;
    }
    task->wt_in = xfer;
    if (!worker_post(httpd_self, task)) {
        /* The main thread is not draining.  Answering 503 here beats
         * blocking this thread, which is the one thing it may not do. */
        worker_task_free(task); /* frees xfer */
        return 0;
    }
    /* Mongoose would otherwise consider the request finished and, on a
     * keep-alive connection, start on the next one while this one is
     * still being answered. */
    c->is_resp = 1;
    return 1;
}

/* The headers of an answer, as one string: Content-Type and whatever the
 * module added.  A header holding a newline is how one response becomes
 * two, and the second one is whatever the module was handed: it is
 * dropped, and the answer still goes. */
static void httpd_reply_headers(const struct HttpXfer* xfer, char* buf,
                                size_t size)
{
    size_t n, wrote;
    unsigned int i;

    /* A file's type goes to Mongoose separately (mime_types), which
     * writes the header itself. */
    if (!xfer->hx_type[0] || xfer->hx_sendfile[0])
        wrote = 0;
    else if (strpbrk(xfer->hx_type, "\r\n"))
        wrote = mg_snprintf(buf, size, "Content-Type: text/plain\r\n");
    else
        wrote = mg_snprintf(buf, size, "Content-Type: %s\r\n", xfer->hx_type);
    buf[0] = wrote ? buf[0] : '\0';
    n = wrote < size ? wrote : size - 1;
    for (i = 0; i < xfer->hx_nreply; i++) {
        const struct HttpHeader* h = &xfer->hx_reply[i];
        if (!h->hh_name[0] || strpbrk(h->hh_name, "\r\n: ") ||
            strpbrk(h->hh_value, "\r\n"))
            continue;
        wrote = mg_snprintf(buf + n, size - n, "%s: %s\r\n", h->hh_name,
                            h->hh_value);
        if (wrote >= size - n) {
            buf[n] = '\0'; /* no room: this one and the rest go */
            break;
        }
        n += wrote;
    }
}

/* Send an answer that came down from the main thread. */
static void httpd_answer(struct mg_connection* c, const struct HttpXfer* xfer)
{
    char headers[HTTP_HEADERS_MAX * (HTTP_HNAME_MAX + HTTP_HVALUE_MAX + 4) +
                 HTTP_HVALUE_MAX + 32];

    if (!xfer->hx_status) {
        c->is_draining = 1; /* nobody will answer; let it go */
        return;
    }
    httpd_reply_headers(xfer, headers, sizeof(headers));

    if (xfer->hx_sendfile[0]) {
        /* The request is long gone -- it went up, was answered, and came
         * back as bytes -- so the message mg_http_serve_file() wants is
         * rebuilt from what travelled with the answer: the method, Range
         * and If-None-Match.  Mongoose does the rest, including the
         * partial-content arithmetic and the ETag. */
        struct mg_http_message hm;
        struct mg_http_serve_opts opts;
        char mime[HTTP_HVALUE_MAX + 3];
        int n = 0;

        memset(&hm, 0, sizeof(hm));
        memset(&opts, 0, sizeof(opts));
        hm.method = mg_str(c->data[CONN_HEAD] ? "HEAD" : "GET");
        if (xfer->hx_range[0]) {
            hm.headers[n].name = mg_str("Range");
            hm.headers[n].value = mg_str(xfer->hx_range);
            n++;
        }
        if (xfer->hx_inm[0]) {
            hm.headers[n].name = mg_str("If-None-Match");
            hm.headers[n].value = mg_str(xfer->hx_inm);
            n++;
        }
        /* Mongoose always writes a Content-Type of its own for a file: a
         * type the module chose is given to it as the type of every name
         * ("*=type"), and without one it guesses from the name. */
        if (xfer->hx_type[0] && !strpbrk(xfer->hx_type, "\r\n,")) {
            mg_snprintf(mime, sizeof(mime), "*=%s", xfer->hx_type);
            opts.mime_types = mime;
        }
        opts.extra_headers = headers;
        mg_http_serve_file(c, &hm, xfer->hx_sendfile, &opts);
    }
    else {
        mg_printf(c, "HTTP/1.1 %d %s\r\n%sContent-Length: %lu\r\n\r\n",
                  xfer->hx_status, http_status_text(xfer->hx_status), headers,
                  (unsigned long)xfer->hx_bodylen);
        if (!c->data[CONN_HEAD] && xfer->hx_bodylen)
            mg_send(c, xfer->hx_body, xfer->hx_bodylen);
    }
    c->is_resp = 0;
    if (c->data[CONN_CLOSE])
        c->is_draining = 1;
}

/* Send whatever the main thread has put on the outbox. */
static void httpd_drain_outbox(void)
{
    struct HttpXfer* list;

    pthread_mutex_lock(&outbox_lock);
    list = outbox_head;
    outbox_head = outbox_tail = NULL;
    pthread_mutex_unlock(&outbox_lock);

    while (list) {
        struct HttpXfer* xfer = list;
        struct mg_connection* c;

        list = xfer->hx_next;
        for (c = httpd_mgr.conns; c; c = c->next) {
            if (c->id == xfer->hx_conn)
                break;
        }
        /* Not found: the client went away while its answer was being
         * worked out.  Not an error. */
        if (c && !c->is_listening && !c->is_closing)
            httpd_answer(c, xfer);
        worker_free(xfer);
    }
}

/* Mongoose's event handler, for every connection. */
static void httpd_event(struct mg_connection* c, int ev, void* ev_data)
{
    switch (ev) {
        case MG_EV_ACCEPT:
            httpd_conns++;
            /* Over the limit the connection is closed rather than queued:
             * a queue of connections nobody is reading is a polite way of
             * running out of descriptors. */
            if (httpd_conn_max > 0 && httpd_conns > httpd_conn_max) {
                c->is_closing = 1;
                break;
            }
            if (c->is_tls) {
                struct mg_tls_opts opts;
                memset(&opts, 0, sizeof(opts));
                opts.cert = httpd_cert;
                opts.key = httpd_key;
                mg_tls_init(c, &opts);
            }
            break;

        case MG_EV_CLOSE:
            if (c->is_accepted)
                httpd_conns--;
            break;

        case MG_EV_HTTP_HDRS:
            httpd_headers(c, (struct mg_http_message*)ev_data);
            break;

        case MG_EV_HTTP_MSG:
            if (!httpd_request(c, (struct mg_http_message*)ev_data))
                httpd_status(c, 503);
            break;

        default:
            /* MG_EV_WAKEUP is only the doorbell: the outbox is emptied
             * after the poll it ended. */
            break;
    }
}

/* Tell the main thread how the start went. */
static void httpd_report(struct HttpdReady* ready, WorkDoneFn fn)
{
    struct WorkTask* task = worker_task_new(NULL, fn);

    if (!task) {
        worker_free(ready);
        return;
    }
    task->wt_in = ready;
    if (!worker_post(httpd_self, task))
        worker_task_free(task);
}

static void httpd_main(struct Worker* worker, void* arg)
{
    struct HttpdArgs* args = arg;
    struct HttpdReady* ready = worker_alloc(sizeof(*ready));
    WorkDoneFn ready_fn = args->ha_ready;
    int i, listening = 0;

    httpd_self = worker;
    httpd_deliver_fn = args->ha_deliver;
    httpd_conns = 0;
    httpd_conn_max = args->ha_max;
    memset(&httpd_cert, 0, sizeof(httpd_cert));
    memset(&httpd_key, 0, sizeof(httpd_key));

    if (!ready) {
        worker_free(args);
        return;
    }
    ready->hr_nurl = args->ha_nurl;
    for (i = 0; i < args->ha_nurl; i++)
        memcpy(ready->hr_url[i], args->ha_url[i], HTTPD_URL_MAX);

    mg_mgr_init(&httpd_mgr);
    if (!mg_wakeup_init(&httpd_mgr)) {
        strcpy(ready->hr_error, "could not create the wake-up socket");
        goto fail;
    }
    if (args->ha_cert[0] && args->ha_key[0]) {
        httpd_cert = mg_file_read(&mg_fs_posix, args->ha_cert);
        httpd_key = mg_file_read(&mg_fs_posix, args->ha_key);
        if (!httpd_cert.buf || !httpd_key.buf) {
            mg_snprintf(ready->hr_error, sizeof(ready->hr_error),
                        "could not read %s or %s", args->ha_cert,
                        args->ha_key);
            goto fail;
        }
    }
    for (i = 0; i < args->ha_nurl; i++) {
        struct mg_connection* c =
            mg_http_listen(&httpd_mgr, args->ha_url[i], httpd_event, NULL);
        ready->hr_ok[i] = c != NULL;
        if (c) {
            listening++;
            pthread_mutex_lock(&outbox_lock);
            if (!doorbell)
                doorbell = c->id;
            pthread_mutex_unlock(&outbox_lock);
        }
    }
    worker_free(args);
    args = NULL;
    if (!listening)
        goto fail;
    httpd_report(ready, ready_fn);

    while (!worker_stopping(worker)) {
        mg_mgr_poll(&httpd_mgr, HTTPD_POLL_MS);
        httpd_drain_outbox();
    }
    /* The main thread stopped sending before it asked this thread to
     * stop, and waits for it here. */
    mg_mgr_free(&httpd_mgr);
    goto done;

fail:
    mg_mgr_free(&httpd_mgr);
    httpd_report(ready, ready_fn);
    if (args)
        worker_free(args);
done:
    mg_free((void*)httpd_cert.buf);
    mg_free((void*)httpd_key.buf);
    memset(&httpd_cert, 0, sizeof(httpd_cert));
    memset(&httpd_key, 0, sizeof(httpd_key));
    pthread_mutex_lock(&outbox_lock);
    doorbell = 0;
    pthread_mutex_unlock(&outbox_lock);
    httpd_self = NULL;
}

/*************************************************************************/
/*************************************************************************/

/* The main thread's side. */

int httpd_server_start(struct Module_* mod, const struct HttpdArgs* args)
{
    struct HttpdArgs* copy;

    if (httpd_worker)
        return 1;
    if (!(copy = worker_alloc(sizeof(*copy))))
        return 0;
    *copy = *args;
    httpd_worker = worker_spawn_owned(mod, "httpd", httpd_main, copy);
    if (!httpd_worker) {
        worker_free(copy);
        return 0;
    }
    return 1;
}

void httpd_server_stop(void)
{
    struct Worker* worker = httpd_worker;
    struct HttpXfer* list;
    unsigned long bell;

    if (!worker)
        return;
    /* In this order, and the order is the whole of the thread safety
     * here: nothing is sent once httpd_worker is NULL, and worker_stop()
     * then waits for the thread, which frees the manager as the last
     * thing it does. */
    httpd_worker = NULL;
    pthread_mutex_lock(&outbox_lock);
    bell = doorbell;
    pthread_mutex_unlock(&outbox_lock);
    if (bell)
        mg_wakeup(&httpd_mgr, bell, "", 1); /* end the poll now */
    worker_stop(worker);

    pthread_mutex_lock(&outbox_lock);
    list = outbox_head;
    outbox_head = outbox_tail = NULL;
    doorbell = 0;
    pthread_mutex_unlock(&outbox_lock);
    while (list) {
        struct HttpXfer* next = list->hx_next;
        worker_free(list);
        list = next;
    }
}

int httpd_server_running(void)
{
    return httpd_worker != NULL;
}

int httpd_server_send(struct HttpXfer* xfer)
{
    unsigned long bell;

    if (!httpd_worker) {
        worker_free(xfer);
        return 0;
    }
    xfer->hx_next = NULL;
    pthread_mutex_lock(&outbox_lock);
    if (outbox_tail)
        outbox_tail->hx_next = xfer;
    else
        outbox_head = xfer;
    outbox_tail = xfer;
    bell = doorbell;
    pthread_mutex_unlock(&outbox_lock);
    /* mg_wakeup() is documented as safe from any thread.  If the bell is
     * not heard (its socket buffer full), the answer goes after the next
     * poll anyway. */
    if (bell)
        mg_wakeup(&httpd_mgr, bell, "", 1);
    return 1;
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
