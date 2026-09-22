/* Top-page (http://services.example.net/) handler.
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

static const char *Filename = NULL;
static const char *ContentType = "text/html";
static const char *Redirect = NULL;

/*************************************************************************/
/*************************** Request callback ****************************/
/*************************************************************************/

static int do_request(Client *c, int *close_ptr)
{
    if (*c->url && strcmp(c->url, "/") != 0) {
        return 0;
    }
    if (Redirect) {
        http_send_response(c, HTTP_R_FOUND);
        sockprintf(c->socket, "Location: %s\r\n", Redirect);
        sockprintf(c->socket, "Content-Length: 0\r\n\r\n");
    } else if (Filename) {
        FILE *f = fopen(Filename, "rb");
        if (f) {
            char buf[4096];
            int i;
            *close_ptr = 1;
            http_send_response(c, HTTP_S_OK);
            sockprintf(c->socket,
                       "Content-Type: %s\r\nConnection: close\r\n\r\n",
                       ContentType);
            while ((i = fread(buf, 1, sizeof(buf), f)) > 0)
                swrite(c->socket, buf, i);
            fclose(f);
        } else if (errno == EACCES) {
            http_error(c, HTTP_E_FORBIDDEN, NULL);
        } else {
            http_error(c, HTTP_E_NOT_FOUND, NULL);
        }
    } else {
        http_error(c, HTTP_E_NOT_FOUND, NULL);
    }
    return 1;
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

ConfigDirective module_config[] = {
    { "Filename",         { { CD_STRING, 0, &Filename },
                            { CD_STRING, CF_OPTIONAL, &ContentType } } },
    { "Redirect",         { { CD_STRING, 0, &Redirect } } },
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
