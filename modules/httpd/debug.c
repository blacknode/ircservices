/* Debug page module for HTTP server.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "conffile.h"
#include "http.h"

/*************************************************************************/

static Module *module_httpd;

static char *DebugURL;

/*************************************************************************/
/*************************** Request callback ****************************/
/*************************************************************************/

static int do_request(Client *c, int *close_ptr)
{
    /* Check whether this URL belongs to us.  If not, pass control on to
     * the next callback function.
     */
    if (strcmp(c->url, DebugURL) != 0)
        return 0;


    /* Send initial "200 OK" line and Date: header. */
    http_send_response(c, HTTP_S_OK);

    /* If not a HEAD request, indicate that we will close the connection
     * after this request (because we don't send a Content-Length:
     * header).  If the request is a HEAD request, the blank line after
     * the headers will signal the end of the response, so we can leave
     * the connection open for further requests (keepalive).
     */
    if (c->method != METHOD_HEAD)
        sockprintf(c->socket, "Connection: close\r\n");

    /* Send Content-Type: header and end header portion of response */
    sockprintf(c->socket, "Content-Type: text/plain\r\n\r\n");


    /* Now send the body part of the response for non-HEAD requests.
     *
     * RFC2616 9.4: Server MUST NOT return a message-body in the response
     *              to a HEAD request.
     */
    if (c->method != METHOD_HEAD) {
        int i;

        /* Write data to socket. */
        sockprintf(c->socket, "address: %s\n", c->address);
        sockprintf(c->socket, "request_len: %d\n", c->request_len);
        sockprintf(c->socket, "version_major: %d\n", c->version_major);
        sockprintf(c->socket, "version_minor: %d\n", c->version_minor);
        sockprintf(c->socket, "method: %d\n", c->method);
        sockprintf(c->socket, "url: %s\n", c->url);
        sockprintf(c->socket, "data_len: %d\n", c->data_len);
        sockprintf(c->socket, "headers_count: %d\n", c->headers_count);
        ARRAY_FOREACH (i, c->headers)
            sockprintf(c->socket, "headers[%d]: %s: %s\n", i, c->headers[i],
                       c->headers[i] + strlen(c->headers[i]) + 1);
        sockprintf(c->socket, "variables_count: %d\n", c->variables_count);
        ARRAY_FOREACH (i, c->variables)
            sockprintf(c->socket, "variables[%d]: %s: %s\n", i,c->variables[i],
                       c->variables[i] + strlen(c->variables[i]) + 1);

        /* We did not specify a Content-Length: header, so we must close
         * the connection to signal end-of-data to the client.  However,
         * we MUST NOT call disconn() directly as that could lead to
         * use of invalid pointers in the main HTTP server module.
         * Instead, we set `close_ptr' nonzero, which tells the server to
         * close the connection when control returns to it.
         */
        *close_ptr = 1;
    }
    /* Note that we MUST NOT explicitly set `close_ptr' to zero for HEAD
     * requests, or any other request for which we can keep the connection
     * alive; the client may be an old (HTTP/1.0) client that can't handle
     * keepalive, or it may have explicitly requested the connection be
     * closed (e.g. with a "Connection: close" header).
     */

    /* URL was handled by this module; terminate callback chain. */
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

int init_module(void)
{
    module_httpd = find_module("httpd/main");
    if (!module_httpd) {
        module_log("Main httpd module not loaded");
        exit_module(0);
        return 0;
    }
    use_module(module_httpd);

    if (!add_callback(module_httpd, "request", do_request)) {
        module_log("Unable to add callback");
        exit_module(0);
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    if (module_httpd) {
        remove_callback(module_httpd, "request", do_request);
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
