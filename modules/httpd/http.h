/* HTTP-related definitions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef HTTP_H
#define HTTP_H

#ifndef TIMEOUT_H
# include "timeout.h"
#endif

/*************************************************************************/

/* Client data structure. */

typedef struct {
    Socket *socket;
    Timeout *timeout;
    char address[22];   /* aaa.bbb.ccc.ddd:ppppp\0 */
    uint32 ip;
    uint16 port;
    int request_count;  /* Number of requests so far on this connection */
    int in_request;     /* Nonzero if request currently being processed */
    char *request_buf;  /* Pointers below point into this buffer */
    int32 request_len;  /* Amount of data read so far */
    int version_major;
    int version_minor;
    int method;         /* METHOD_* (see below) */
    char *url;
    char *data;         /* For POST */
    int32 data_len;
    char **headers;     /* name + \0 + value */
    int32 headers_count;
    char **variables;   /* name + \0 + value */
    int32 variables_count;
} Client;

/*************************************************************************/

/* Maximum length of a single line in an HTTP request (bytes). */
#define HTTP_LINEMAX    4096

/*************************************************************************/

/* Return codes from authorization callbacks. */
#define HTTP_AUTH_UNDECIDED     0
#define HTTP_AUTH_ALLOW         1
#define HTTP_AUTH_DENY          2

/*************************************************************************/

/* HTTP methods. */
#define METHOD_GET      0
#define METHOD_HEAD     1
#define METHOD_POST     2

/*************************************************************************/

/* HTTP reply codes.  Prefix letters are:
 *     I - 1xx Informational
 *     S - 2xx Successful
 *     R - 3xx Redirection
 *     E - 4xx Client Error
 *     F - 5xx Server Error (think "Failure")
 */

#define HTTP_I_CONTINUE                 100
#define HTTP_I_SWITCHING_PROTOCOLS      101

#define HTTP_S_OK                       200
#define HTTP_S_CREATED                  201
#define HTTP_S_ACCEPTED                 202
#define HTTP_S_NON_AUTHORITATIVE        203
#define HTTP_S_NO_CONTENT               204
#define HTTP_S_RESET_CONTENT            205
#define HTTP_S_PARTIAL_CONTENT          206

#define HTTP_R_MULTIPLE_CHOICES         300
#define HTTP_R_MOVED_PERMANENTLY        301
#define HTTP_R_FOUND                    302
#define HTTP_R_SEE_OTHER                303
#define HTTP_R_NOT_MODIFIED             304
#define HTTP_R_USE_PROXY                305
#define HTTP_R_TEMPORARY_REDIRECT       307

#define HTTP_E_BAD_REQUEST              400
#define HTTP_E_UNAUTHORIZED             401
#define HTTP_E_PAYMENT_REQUIRED         402
#define HTTP_E_FORBIDDEN                403
#define HTTP_E_NOT_FOUND                404
#define HTTP_E_METHOD_NOT_ALLOWED       405
#define HTTP_E_NOT_ACCEPTABLE           406
#define HTTP_E_PROXY_AUTH_REQUIRED      407
#define HTTP_E_REQUEST_TIMEOUT          408
#define HTTP_E_CONFLICT                 409
#define HTTP_E_GONE                     410
#define HTTP_E_LENGTH_REQUIRED          411
#define HTTP_E_PRECONDITION_FAILED      412
#define HTTP_E_REQUEST_ENTITY_TOO_LARGE 413
#define HTTP_E_REQUEST_URI_TOO_LONG     414
#define HTTP_E_UNSUPPORTED_MEDIA_TYPE   415
#define HTTP_E_RANGE_NOT_SATISFIABLE    416
#define HTTP_E_EXPECTATION_FAILED       417

#define HTTP_F_INTERNAL_SERVER_ERROR    500
#define HTTP_F_NOT_IMPLEMENTED          501
#define HTTP_F_BAD_GATEWAY              502
#define HTTP_F_SERVICE_UNAVAILABLE      503
#define HTTP_F_GATEWAY_TIMEOUT          504
#define HTTP_F_HTTP_VER_NOT_SUPPORTED   505

/*************************************************************************/
/*************************************************************************/

/* Utility routines (provided by core module, in util.c): */

extern char *http_get_header(Client *c, const char *header);
extern char *http_get_variable(Client *c, const char *variable);
extern char *http_quote_html(const char *str, char *outbuf, int32 outsize);
extern char *http_quote_url(const char *str, char *outbuf, int32 outsize);
extern char *http_unquote_url(char *buf);
extern void http_send_response(Client *c, int code);
extern void http_error(Client *c, int code, const char *format, ...);

/*************************************************************************/

#endif  /* HTTP_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
