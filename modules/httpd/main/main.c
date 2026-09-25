/* Main HTTP server module.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The configuration, and starting and stopping the server thread.  The
 * server is server.c (Mongoose, on a thread of its own), the routes are
 * routes.c; see httpd_int.h and docs/readme.http.
 */

#include "conffile.h"
#include "modules.h"
#include "services.h"
#include "timeout.h"

#include "httpd_int.h"

#include <arpa/inet.h>

/*************************************************************************/

/* Connections at once, by default. */
#define DEF_MAX_CONNECTIONS 64

/* Configuration. */

static struct listento_ {
    char url[HTTPD_URL_MAX]; /* "http://1.2.3.4:80", "https://[::1]:443" */
} *ListenTo, *new_ListenTo;
static int ListenTo_count, new_ListenTo_count;

static int32 MaxConnections;
static time_t RequestTimeout;
static char* TLSCertificate;
static char* TLSKey;

/* What the server thread was started with, so that a REHASH that changed
 * none of it leaves it alone: restarting it drops every connection. */
static struct HttpdArgs current_args;

static Timeout* expire_timeout;

/*************************************************************************/
/*************************************************************************/

int http_available(void)
{
    return ListenTo_count > 0;
}

int httpd_have_listeners(void)
{
    return ListenTo_count > 0;
}

/*************************************************************************/

/* How the start went (from the server thread). */

static void httpd_ready(struct WorkTask* task)
{
    struct HttpdReady* ready = task->wt_in;
    int i, ok = 0;

    if (!ready)
        return;
    if (ready->hr_error[0])
        module_log("HTTP server: %s", ready->hr_error);
    for (i = 0; i < ready->hr_nurl; i++) {
        if (ready->hr_ok[i]) {
            module_log("Listening on %s", ready->hr_url[i]);
            ok++;
        }
        else if (!ready->hr_error[0]) {
            module_log("Could not listen on %s (address in use, or not"
                       " permitted)",
                       ready->hr_url[i]);
        }
    }
    if (!ok) {
        /* The thread has already returned; forget it, so that a REHASH
         * tries again rather than believing a listener is up. */
        module_log("No HTTP port could be opened");
        httpd_server_stop();
        httpd_routes_shutdown();
        memset(&current_args, 0, sizeof(current_args));
    }
}

/*************************************************************************/

/* Bring the server thread in line with the configuration. */

static void httpd_apply(void)
{
    struct HttpdArgs want;
    int i, tls = 0;

    memset(&want, 0, sizeof(want));
    want.ha_max = MaxConnections > 0 ? MaxConnections : DEF_MAX_CONNECTIONS;
    want.ha_deliver = httpd_deliver;
    want.ha_ready = httpd_ready;
    if (TLSCertificate && *TLSCertificate && TLSKey && *TLSKey) {
        strbcpy(want.ha_cert, TLSCertificate);
        strbcpy(want.ha_key, TLSKey);
        tls = 1;
    }
    else if ((TLSCertificate && *TLSCertificate) || (TLSKey && *TLSKey)) {
        module_log("TLSCertificate and TLSKey go together; neither is used");
    }
    for (i = 0; i < ListenTo_count; i++) {
        if (strncmp(ListenTo[i].url, "https:", 6) == 0 && !tls) {
            module_log("Not listening on %s: no TLSCertificate/TLSKey",
                       ListenTo[i].url);
            continue;
        }
        strbcpy(want.ha_url[want.ha_nurl], ListenTo[i].url);
        want.ha_nurl++;
    }

    httpd_timeout_seconds =
        RequestTimeout > 0 ? (int)RequestTimeout : HTTP_TIMEOUT_DEFAULT;
    if (httpd_timeout_seconds > HTTP_TIMEOUT_MAX)
        httpd_timeout_seconds = HTTP_TIMEOUT_MAX;

    if (httpd_server_running() &&
        memcmp(&want, &current_args, sizeof(want)) == 0)
        return;
    if (httpd_server_running()) {
        module_log("HTTP settings changed; restarting the listener");
        httpd_server_stop();
        httpd_routes_shutdown();
    }
    memset(&current_args, 0, sizeof(current_args));
    if (!want.ha_nurl)
        return;
    if (!httpd_server_start(THIS_MODULE, &want)) {
        module_log("Could not start the HTTP server thread (is the workers"
                   " block giving it any?)");
        return;
    }
    current_args = want;
}

/*************************************************************************/

static void do_expire(Timeout* t)
{
    httpd_routes_expire();
}

/* A module going drops its routes, whether or not it gave them up. */
static int do_module_unloaded(Module* module)
{
    httpd_routes_drop_module(module);
    return 0;
}

static void httpd_rehash(Module* module)
{
    httpd_apply();
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_ListenTo(const char* filename, int linenum, char* param);
static int do_ListenTo_tls(const char* filename, int linenum, char* param);

static ConfigDirective httpd_config[] = {
    {"ListenTo",
     {{CD_FUNC, 0, do_ListenTo}, {CD_FUNC, CF_OPTIONAL, do_ListenTo_tls}}},
    {"MaxConnections", {{CD_POSINT, 0, &MaxConnections}}},
    {"RequestTimeout", {{CD_TIME, 0, &RequestTimeout}}},
    {"TLSCertificate", {{CD_STRING, 0, &TLSCertificate}}},
    {"TLSKey", {{CD_STRING, 0, &TLSKey}}},
    {NULL}};

/*************************************************************************/

/* ListenTo = <address>:<port> [, tls];  <address> is an IPv4 address, an
 * IPv6 address in brackets, or "*" for every IPv4 address. */

static int do_ListenTo(const char* filename, int linenum, char* param)
{
    char host[64], *s;
    unsigned char buf[16];
    long port;
    int i;

    if (!filename) {
        switch (linenum) {
            case CDFUNC_INIT:
                free(new_ListenTo);
                new_ListenTo = NULL;
                new_ListenTo_count = 0;
                break;
            case CDFUNC_SET:
                free(ListenTo);
                ListenTo = new_ListenTo;
                ListenTo_count = new_ListenTo_count;
                new_ListenTo = NULL;
                new_ListenTo_count = 0;
                break;
            case CDFUNC_DECONFIG:
                free(ListenTo);
                ListenTo = NULL;
                ListenTo_count = 0;
                break;
        }
        return 1;
    }

    if (new_ListenTo_count >= HTTPD_LISTEN_MAX) {
        config_error(filename, linenum,
                     "Too many ListenTo addresses (maximum %d)",
                     HTTPD_LISTEN_MAX);
        return 0;
    }
    s = strrchr(param, ':');
    if (!s || s == param) {
        config_error(filename, linenum,
                     "ListenTo requires both an address and a port"
                     " (<address>:<port>)");
        return 0;
    }
    *s++ = 0;
    port = atolsafe(s, 1, 65535);
    if (port < 1) {
        config_error(filename, linenum, "Invalid port number `%s'", s);
        return 0;
    }
    if (strcmp(param, "*") == 0) {
        strbcpy(host, "0.0.0.0");
    }
    else if (param[0] == '[' && param[strlen(param) - 1] == ']') {
        param[strlen(param) - 1] = 0;
        if (inet_pton(AF_INET6, param + 1, buf) != 1) {
            config_error(filename, linenum, "Invalid IPv6 address `%s'",
                         param + 1);
            return 0;
        }
        snprintf(host, sizeof(host), "[%s]", param + 1);
    }
    else if (inet_pton(AF_INET, param, buf) == 1) {
        strbcpy(host, param);
    }
    else {
        config_error(filename, linenum,
                     "`%s' is not an IP address (give an IPv4 address, an"
                     " IPv6 address in brackets, or *)",
                     param);
        return 0;
    }

    i = new_ListenTo_count;
    ARRAY_EXTEND(new_ListenTo);
    snprintf(new_ListenTo[i].url, sizeof(new_ListenTo[i].url), "http://%s:%ld",
             host, port);
    return 1;
}

/* The optional second value: "tls" makes the address above an HTTPS
 * listener. */

static int do_ListenTo_tls(const char* filename, int linenum, char* param)
{
    struct listento_* l;

    if (!filename)
        return 1;
    if (stricmp(param, "tls") != 0) {
        config_error(filename, linenum,
                     "The second value of ListenTo can only be `tls'");
        return 0;
    }
    if (!new_ListenTo_count)
        return 0; /* the address was refused; that error is enough */
    l = &new_ListenTo[new_ListenTo_count - 1];
    if (strncmp(l->url, "http://", 7) == 0) {
        char url[HTTPD_URL_MAX + 8];
        snprintf(url, sizeof(url), "https://%s", l->url + 7);
        strbcpy(l->url, url); /* the address is short: it fits */
    }
    return 1;
}

/*************************************************************************/

static int httpd_init(Module* module)
{
    if (!httpd_routes_init()) {
        module_log("Unable to declare " HTTPD_EVENT_AUTH);
        return 0;
    }
    if (!event_attach(module, EVENT_MODULE_UNLOADED, do_module_unloaded)) {
        module_log("Unable to attach to " EVENT_MODULE_UNLOADED);
        return 0;
    }
    expire_timeout = add_timeout(1, do_expire, 1);
    if (!ListenTo_count)
        module_log("No ListenTo address: not listening");
    httpd_apply();
    return 1;
}

/*************************************************************************/

static int httpd_fini(Module* module, int shutdown)
{
    httpd_server_stop();
    httpd_routes_cleanup();
    if (expire_timeout) {
        del_timeout(expire_timeout);
        expire_timeout = NULL;
    }
    memset(&current_args, 0, sizeof(current_args));
    free(ListenTo);
    ListenTo = NULL;
    ListenTo_count = 0;
    return 1;
}

/*************************************************************************/

/* A module without a pseudo-client: the HTTP server the httpd/ modules
 * claim routes on (see http.h). */
ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HTTP server (routes for the httpd/ modules)",
    .config = httpd_config,
    .init = httpd_init,
    .fini = httpd_fini,
    .rehash = httpd_rehash,
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
