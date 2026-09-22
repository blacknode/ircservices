/* HTTP server common utility routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "http.h"

/*************************************************************************/
/*************************************************************************/

/* List of response texts for each response code, and inline functions to
 * retrieve them.
 */

static struct {
    int code;
    const char *text;
    const char *desc;
} http_response_text[] = {

    { 100, "Continue" },
    { 101, "Switching Protocols" },

    { 200, "OK" },
    { 201, "Created" },
    { 202, "Accepted" },
    { 203, "Non-Authoritative Information" },
    { 204, "No Content" },
    { 205, "Reset Content" },
    { 206, "Partial Content" },

    { 300, "Multiple Choices" },
    { 301, "Moved Permanently" },
    { 302, "Found" },
    { 303, "See Other" },
    { 304, "Not Modified" },
    { 305, "Use Proxy" },
    { 307, "Temporary Redirect" },

    { 400, "Bad Request",
           "Your browser sent a request this server could not understand." },
    { 401, "Unauthorized",
           "You are not authorized to access this resource." },
    { 402, "Payment Required" },
    { 403, "Forbidden",
           "You are not permitted to access this resource." },
    { 404, "Not Found",
           "The requested resource could not be found." },
    { 405, "Method Not Allowed",
           "Your browser sent an invalid method for this resource." },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Timeout" },
    { 409, "Conflict" },
    { 410, "Gone" },
    { 411, "Length Required",
           "Your browser sent an invalid request to the server." },
    { 412, "Precondition Failed" },
    { 413, "Request Entity Too Large" },
    { 414, "Request-URI Too Large" },
    { 415, "Unsupported Media Type" },
    { 416, "Requested Range Not Satisfiable" },
    { 417, "Expectation Failed" },

    { 500, "Internal Server Error",
           "An internal server error has occurred." },
    { 501, "Not Implemented",
           "The requested method is not implemented by this server." },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable"
           "Your request cannot currently be processed.  Please try again"
           " later." },
    { 504, "Gateway Timeout" },
    { 505, "HTTP Version Not Supported" },

    { -1 }
};

static inline const char *http_lookup_response(int code)
{
    int i;
    for (i = 0; http_response_text[i].code > 0; i++) {
        if (http_response_text[i].code == code)
            return http_response_text[i].text;
    }
    return NULL;
}

static inline const char *http_lookup_description(int code)
{
    int i;
    for (i = 0; http_response_text[i].code > 0; i++) {
        if (http_response_text[i].code == code)
            return http_response_text[i].desc;
    }
    return NULL;
}

/*************************************************************************/
/*************************************************************************/

/* Return the value of the named header from the client request.  If the
 * request did not include the named header, return NULL.  If `header' is
 * NULL, return the next header with the same name as the previous call
 * which included a non-NULL `header' or NULL if none, a la strtok().
 */

char *http_get_header(Client *c, const char *header)
{
    int i;
    static const char *last_header = NULL;
    static int last_return;

    if (!c) {
        module_log("BUG: http_get_header(): client is NULL!");
        return NULL;
    }
    if (!header) {
        if (!last_header)
            return NULL;
        header = last_header;
        i = (last_return>=c->headers_count) ? c->headers_count : last_return+1;
    } else {
        i = 0;
    }
    last_header = header;
    while (i < c->headers_count) {
        if (stricmp(c->headers[i], header) == 0) {
            last_return = i;
            return c->headers[i] + strlen(c->headers[i]) + 1;
        }
        i++;
    }
    last_return = i;
    return NULL;
}

/*************************************************************************/

/* Return the value of the named variable from the client request.  If the
 * request did not include the named variable, return NULL.  If `variable'
 * is NULL, return the next variable with the same name as the previous
 * call which included a non-NULL `variable' or NULL if none, a la strtok().
 * Variable names are assumed to be case-insensitive.
 */

char *http_get_variable(Client *c, const char *variable)
{
    int i;
    static const char *last_variable = NULL;
    static int last_return;

    if (!c) {
        module_log("BUG: http_get_variable(): client is NULL!");
        return NULL;
    }
    if (!variable) {
        if (!last_variable)
            return NULL;
        variable = last_variable;
        i = (last_return>=c->variables_count) ? c->variables_count
                                              : last_return+1;
    } else {
        i = 0;
    }
    last_variable = variable;
    while (i < c->variables_count) {
        if (stricmp(c->variables[i], variable) == 0) {
            last_return = i;
            return c->variables[i] + strlen(c->variables[i]) + 1;
        }
        i++;
    }
    last_return = i;
    return NULL;
}

/*************************************************************************/

/* HTML-quote (&...;) any HTML-special characters (<,>,&) in `str', and
 * place the result in `outbuf', truncating to `outsize' bytes (including
 * trailing null).  &...; entities inserted by this routine will never be
 * truncated.  Returns `outbuf' on success, NULL on error (invalid
 * parameter).
 */

char *http_quote_html(const char *str, char *outbuf, int32 outsize)
{
    char *retval = outbuf;

    if (!str || !outbuf || outsize <= 0) {
        if (outsize <= 0)
            module_log("BUG: http_quote_html(): bad outsize (%d)!", outsize);
        else
            module_log("BUG: http_quote_html(): %s is NULL!",
                       !str ? "str" : "outbuf");
        errno = EINVAL;
        return NULL;
    }
    while (*str && outsize > 1) {
        switch (*str) {
          case '&':
            if (outsize < 6) {
                outsize = 0;
                break;
            }
            memcpy(outbuf, "&amp;", 5);
            outbuf += 5;
            outsize -= 5;
            break;
          case '<':
            if (outsize < 5) {
                outsize = 0;
                break;
            }
            memcpy(outbuf, "&lt;", 5);
            outbuf += 4;
            outsize -= 4;
            break;
          case '>':
            if (outsize < 5) {
                outsize = 0;
                break;
            }
            memcpy(outbuf, "&gt;", 5);
            outbuf += 4;
            outsize -= 4;
            break;
          default:
            *outbuf++ = *str;
            outsize--;
            break;
        }
        str++;
    }
    *outbuf = 0;
    return retval;
}

/*************************************************************************/

/* URL-quote (%nn) any special characters (anything except A-Z a-z 0-9 - . _)
 * and place the result in `outbuf', truncating to `outsize' bytes (including
 * trailing null).  %nn tokens inserted by this routine will never be
 * truncated.  Returns `outbuf' on success, NULL on error (invalid parameter).
 */

char *http_quote_url(const char *str, char *outbuf, int32 outsize)
{
    char *retval = outbuf;

    if (!str || !outbuf || outsize <= 0) {
        if (outsize <= 0)
            module_log("BUG: http_quote_url(): bad outsize (%d)!", outsize);
        else
            module_log("BUG: http_quote_url(): %s is NULL!",
                       !str ? "str" : "outbuf");
        errno = EINVAL;
        return NULL;
    }
    while (*str && outsize > 1) {
        if ((*str < 'A' || *str > 'Z')
         && (*str < 'a' || *str > 'z')
         && (*str < '0' || *str > '9')
         && *str != '-'
         && *str != '.'
         && *str != '_'
        ) {
            if (*str == ' ') {
                *outbuf++ = '+';
                outsize--;
            } else {
                if (outsize < 4) {
                    outsize = 0;
                    break;
                }
                sprintf(outbuf, "%%%.02X", (unsigned char)*str);
                outbuf += 3;
                outsize -= 3;
            }
        } else {
            *outbuf++ = *str;
            outsize--;
        }
        str++;
    }
    *outbuf = 0;
    return retval;
}

/*************************************************************************/

/* Remove URL-quoting (%nn) from the string in the given buffer, and return
 * the buffer, or NULL on failure (invalid parameter).  If the string ends
 * with an incomplete %nn token, it is discarded; invalid %nn tokens (i.e.
 * "nn" is not a pair of hex digits) are also discarded.  Overwrites the
 * buffer.
 */

char *http_unquote_url(char *buf)
{
    char *retval = buf, *out = buf, *s;
    char hexbuf[3] = {0,0,0};

    if (!buf) {
        module_log("BUG: http_unquote_url(): buf is NULL!");
        errno = EINVAL;
        return NULL;
    }
    while (*buf) {
        if (*buf == '%') {
            if (!buf[1] || !buf[2])
                break;
            hexbuf[0] = buf[1];
            hexbuf[1] = buf[2];
            buf += 3;
            *out = strtol(hexbuf, &s, 16);
            if (!*s)  /* i.e. both digits were valid */
                out++;
            /* else discard character */
        } else if (*buf++ == '+') {
            *out++ = ' ';
        } else {
            *out++ = buf[-1];
        }
    }
    *out = 0;
    return retval;
}

/*************************************************************************/

/* Send an HTTP response line with the given code, plus the Date header. */

void http_send_response(Client *c, int code)
{
    const char *text;
    time_t t;
    char datebuf[64];

    if (!c) {
        module_log("BUG: http_send_response(): client is NULL!");
        return;
    } else if (code < 0 || code > 999) {
        module_log("BUG: http_send_response(): code is invalid! (%d)", code);
        return;
    }
    text = http_lookup_response(code);
    if (text)
        sockprintf(c->socket, "HTTP/1.1 %03d %s\r\n", code, text);
    else
        sockprintf(c->socket, "HTTP/1.1 %03d Code %03d\r\n", code, code);
    time(&t);
    if (strftime(datebuf, sizeof(datebuf), "%a, %d %b %Y %H:%M:%S GMT",
                 gmtime(&t)) > 0)
        sockprintf(c->socket, "Date: %s\r\n", datebuf);
    else
        module_log("http_send_response(): strftime() failed");
}

/*************************************************************************/

/* Send an error message with the given code and body text, and close the
 * client connection.  If `format' is NULL, the response text for the given
 * code ("Error NNN" if the code is unknown) and descriptive text, if any,
 * are used as the body text.  Note that the client data structure will be
 * unusable after calling this routine.
 */

void http_error(Client *c, int code, const char *format, ...)
{
    if (!c) {
        module_log("BUG: http_error(): client is NULL!");
        return;
    } else if (code < 0 || code > 999) {
        module_log("BUG: http_error(): code is invalid! (%d)", code);
        http_error(c, HTTP_F_INTERNAL_SERVER_ERROR, NULL);
        return;
    }
    http_send_response(c, code);
    sockprintf(c->socket,
               "Content-Type: text/html\r\nConnection: close\r\n\r\n");
    if (c->method != METHOD_HEAD) {
        if (format) {
            va_list args;
            va_start(args, format);
            vsockprintf(c->socket, format, args);
            va_end(args);
        } else {
            const char *text, *desc;
            text = http_lookup_response(code);
            if (text) {
                desc = http_lookup_description(code);
                sockprintf(c->socket, "<h1 align=center>%s</h1>", text);
                if (desc)
                    sockprintf(c->socket, "%s", desc);
            } else {
                sockprintf(c->socket, "<h1 align=center>Error %d</h1>", code);
            }
        }
    }
    if (c->in_request)
        c->in_request = -1;  /* signal handle_request() to close later */
    else
        disconn(c->socket);
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
