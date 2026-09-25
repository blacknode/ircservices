/* httpd/main: building responses and reading requests.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Main thread only.  See modules/httpd/http.h.
 */

#include "modules.h"
#include "services.h"

#include "httpd_int.h"

/*************************************************************************/

/* The descriptive text of an error page, for the codes a browser user
 * might see. */
static const char* status_description(int status)
{
    switch (status) {
        case 400:
            return "Your browser sent a request this server could not"
                   " understand.";
        case 401:
            return "You are not authorized to access this resource.";
        case 403:
            return "You are not permitted to access this resource.";
        case 404:
            return "The requested resource could not be found.";
        case 405:
            return "Your browser sent an invalid method for this resource.";
        case 500:
            return "The server encountered an internal error.";
        case 503:
            return "The server is too busy to answer; try again later.";
        case 504:
            return "The server did not find the answer in time.";
        default:
            return NULL;
    }
}

/*************************************************************************/
/*************************************************************************/

/* Responses. */

void httpd_response_init(struct HttpResponse* res)
{
    memset(res, 0, sizeof(*res));
}

void httpd_response_free(struct HttpResponse* res)
{
    free(res->hres_body);
    res->hres_body = NULL;
    res->hres_bodylen = res->hres_bodymax = 0;
}

/*************************************************************************/

void http_response_write(struct HttpResponse* res, const char* data,
                         size_t len)
{
    if (res->hres_overflow || !len)
        return;
    if (res->hres_bodylen + len > HTTP_REPLY_MAX) {
        res->hres_overflow = 1;
        return;
    }
    if (res->hres_bodylen + len + 1 > res->hres_bodymax) {
        size_t size = res->hres_bodymax ? res->hres_bodymax : 4096;
        while (size < res->hres_bodylen + len + 1)
            size *= 2;
        res->hres_body = srealloc(res->hres_body, size);
        res->hres_bodymax = size;
    }
    memcpy(res->hres_body + res->hres_bodylen, data, len);
    res->hres_bodylen += len;
    res->hres_body[res->hres_bodylen] = 0;
}

void http_response_vprintf(struct HttpResponse* res, const char* fmt,
                           va_list args)
{
    char buf[4096];
    va_list copy;
    int len;

    va_copy(copy, args);
    len = vsnprintf(buf, sizeof(buf), fmt, copy);
    va_end(copy);
    if (len < 0)
        return;
    if ((size_t)len < sizeof(buf)) {
        http_response_write(res, buf, len);
    }
    else {
        char* big = smalloc(len + 1);
        vsnprintf(big, len + 1, fmt, args);
        http_response_write(res, big, len);
        free(big);
    }
}

void http_response_printf(struct HttpResponse* res, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    http_response_vprintf(res, fmt, args);
    va_end(args);
}

/*************************************************************************/

void http_response_set(struct HttpResponse* res, int status, const char* type,
                       const char* body, size_t bodylen)
{
    res->hres_status = status;
    strbcpy(res->hres_type, type ? type : "text/plain");
    res->hres_bodylen = 0;
    res->hres_overflow = 0;
    res->hres_file[0] = 0;
    if (body)
        http_response_write(res, body, bodylen);
}

int http_response_header(struct HttpResponse* res, const char* name,
                         const char* value)
{
    struct HttpHeader* h;

    if (!name || !*name || res->hres_nheaders >= HTTP_HEADERS_MAX)
        return 0;
    h = &res->hres_headers[res->hres_nheaders++];
    strbcpy(h->hh_name, name);
    strbcpy(h->hh_value, value ? value : "");
    return 1;
}

void http_response_error(struct HttpResponse* res, int status, const char* fmt,
                         ...)
{
    http_response_set(res, status, "text/html; charset=utf-8", NULL, 0);
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        http_response_vprintf(res, fmt, args);
        va_end(args);
    }
    else {
        const char* desc = status_description(status);
        http_response_printf(res,
                             "<html><head><title>%d %s</title></head><body>"
                             "<h1 align=center>%s</h1>%s</body></html>\n",
                             status, http_status_text(status),
                             http_status_text(status), desc ? desc : "");
    }
}

void http_response_redirect(struct HttpResponse* res, int status,
                            const char* location)
{
    http_response_set(res, status, "text/plain", NULL, 0);
    http_response_header(res, "Location", location);
}

int http_response_file(struct HttpResponse* res, const struct HttpRequest* req,
                       const char* path, const char* type)
{
    const char* value;

    if (!path || !*path || strlen(path) > HTTP_PATH_MAX)
        return 0;
    strbcpy(res->hres_file, path);
    /* Empty: the server thread guesses from the name. */
    strbcpy(res->hres_type, type && *type ? type : "");
    if (!res->hres_status)
        res->hres_status = HTTP_S_OK;
    /* The body is not used for this answer. */
    res->hres_bodylen = 0;
    if (req) {
        if ((value = http_request_header(req, "Range")))
            strbcpy(res->hres_range, value);
        if ((value = http_request_header(req, "If-None-Match")))
            strbcpy(res->hres_inm, value);
    }
    return 1;
}

/*************************************************************************/
/*************************************************************************/

/* Requests. */

const char* http_request_header(const struct HttpRequest* req,
                                const char* name)
{
    unsigned int i;

    if (!req || !name)
        return NULL;
    for (i = 0; i < req->hreq_nheaders; i++) {
        if (stricmp(req->hreq_headers[i].hh_name, name) == 0)
            return req->hreq_headers[i].hh_value;
    }
    return NULL;
}

/* Look for `name' in the urlencoded `data' of `len' bytes. */
static char* find_var(const char* data, size_t len, const char* name,
                      char* buf, size_t size)
{
    size_t namelen = strlen(name);
    const char* end = data + len;

    while (data < end) {
        const char* amp = memchr(data, '&', end - data);
        const char* stop = amp ? amp : end;
        const char* eq = memchr(data, '=', stop - data);
        const char* key_end = eq ? eq : stop;
        char key[256];
        size_t n = key_end - data;

        if (n < sizeof(key)) {
            memcpy(key, data, n);
            key[n] = 0;
            http_unquote_url(key);
            if (strlen(key) == namelen && stricmp(key, name) == 0) {
                n = eq ? (size_t)(stop - eq - 1) : 0;
                if (n >= size)
                    n = size - 1;
                if (n)
                    memcpy(buf, eq + 1, n);
                buf[n] = 0;
                return http_unquote_url(buf);
            }
        }
        data = stop + 1;
    }
    return NULL;
}

char* http_request_var(const struct HttpRequest* req, const char* name,
                       char* buf, size_t size)
{
    const char* type;
    char* ret;

    if (!req || !name || !buf || !size)
        return NULL;
    if (req->hreq_query &&
        (ret = find_var(req->hreq_query, strlen(req->hreq_query), name, buf,
                        size)))
        return ret;
    type = http_request_header(req, "Content-Type");
    if (req->hreq_bodylen && type &&
        strnicmp(type, "application/x-www-form-urlencoded", 33) == 0)
        return find_var(req->hreq_body, req->hreq_bodylen, name, buf, size);
    return NULL;
}

/*************************************************************************/
/*************************************************************************/

/* Utilities. */

char* http_quote_html(const char* str, char* outbuf, size_t outsize)
{
    char* out = outbuf;
    size_t left = outsize;

    if (!outsize)
        return outbuf;
    while (str && *str && left > 1) {
        const char* ent = NULL;
        switch (*str) {
            case '&':
                ent = "&amp;";
                break;
            case '<':
                ent = "&lt;";
                break;
            case '>':
                ent = "&gt;";
                break;
            case '"':
                ent = "&quot;";
                break;
        }
        if (ent) {
            size_t n = strlen(ent);
            if (left <= n)
                break; /* never a truncated entity */
            memcpy(out, ent, n);
            out += n;
            left -= n;
        }
        else {
            *out++ = *str;
            left--;
        }
        str++;
    }
    *out = 0;
    return outbuf;
}

char* http_quote_url(const char* str, char* outbuf, size_t outsize)
{
    static const char hex[] = "0123456789ABCDEF";
    char* out = outbuf;
    size_t left = outsize;

    if (!outsize)
        return outbuf;
    while (str && *str && left > 1) {
        unsigned char c = (unsigned char)*str;
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '.' || c == '_') {
            *out++ = c;
            left--;
        }
        else {
            if (left <= 3)
                break; /* never a truncated escape */
            *out++ = '%';
            *out++ = hex[c >> 4];
            *out++ = hex[c & 15];
            left -= 3;
        }
        str++;
    }
    *out = 0;
    return outbuf;
}

static int hexval(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

char* http_unquote_url(char* buf)
{
    char *in = buf, *out = buf;

    if (!buf)
        return NULL;
    while (*in) {
        if (*in == '%') {
            int hi, lo;
            if (!in[1] || !in[2])
                break;
            hi = hexval(in[1]);
            lo = hexval(in[2]);
            if (hi >= 0 && lo >= 0 && (hi || lo))
                *out++ = (char)(hi << 4 | lo);
            /* else discard it (a %00 included) */
            in += 3;
        }
        else if (*in == '+') {
            *out++ = ' ';
            in++;
        }
        else {
            *out++ = *in++;
        }
    }
    *out = 0;
    return buf;
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
