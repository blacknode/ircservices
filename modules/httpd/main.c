/* Main HTTP server module.
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
#include "timeout.h"
#include "http.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*************************************************************************/

static int cb_auth    = -1;
static int cb_request = -1;


static int32  ListenBacklog;
static int32  RequestBufferSize;
static int32  MaxConnections;
static int32  MaxRequests;
static time_t IdleTimeout;
static int    LogConnections;

/* List of ports to listen to (set by ListenTo configuration directive) */
static struct listento_ {
    char ip[16];  /* aaa.bbb.ccc.ddd\0 */
    uint16 port;
} *ListenTo;
static int ListenTo_count;
#define MAX_LISTENTO    32767


/* Array of listen sockets (corresponding to ListenTo[] entries) */
static Socket **listen_sockets;

/* Array of clients */
static Client *clients;
int clients_count;

/*************************************************************************/

static void do_accept(Socket *listener, void *param);
static void do_disconnect(Socket *socket, void *param_unused);
static void do_readline(Socket *s, void *param_unused);
static void do_readdata(Socket *s, void *param);
static void do_timeout(Timeout *t);

static Client *find_client(Socket *s);
static void set_timeout(Client *c);
static void clear_timeout(Client *c);
static void parse_header(Client *c, char *linestart);
static void handle_request(Client *c);

/*************************************************************************/
/*************************** HTTP server core ****************************/
/*************************************************************************/

/* Accept a connection and perform IP address checks on it; if it doesn't
 * pass, disconnect it.
 */

static void do_accept(Socket *listener, void *param)
{
    Socket *new = param;
    struct sockaddr_in sin;
    int sin_len = sizeof(sin);

    if (sock_remote(new, (struct sockaddr *)&sin, &sin_len) < 0) {
        module_log_perror("sock_remote() failed");
    } else if (sin_len > sizeof(sin)) {
        module_log("sock_remote() returned oversize address (%d)", sin_len);
    } else if (sin.sin_family != AF_INET) {
        module_log("sock_remote() returned bad address family (%d)",
                   sin.sin_family);
    } else {
        int i = clients_count;
        ARRAY_EXTEND(clients);
        snprintf(clients[i].address, sizeof(clients[i].address), "%s:%u",
                 unpack_ip((uint8 *)&sin.sin_addr), ntohs(sin.sin_port));
        clients[i].socket          = new;
        clients[i].ip              = sin.sin_addr.s_addr;
        clients[i].port            = sin.sin_port;
        clients[i].timeout         = NULL;
        clients[i].request_count   = 0;
        clients[i].in_request      = 0;
        clients[i].request_buf     = smalloc(RequestBufferSize);
        clients[i].request_len     = 0;
        clients[i].version_major   = 0;
        clients[i].version_minor   = 0;
        clients[i].method          = -1;
        clients[i].url             = NULL;
        clients[i].data            = NULL;
        clients[i].data_len        = 0;
        clients[i].headers         = NULL;
        clients[i].headers_count   = 0;
        clients[i].variables       = NULL;
        clients[i].variables_count = 0;
        if (clients_count >= MaxConnections) {
            module_log("Dropping connection (exceeded MaxConnections: %d)"
                       " from %s", MaxConnections, clients[i].address);
            http_error(&clients[i], HTTP_F_SERVICE_UNAVAILABLE, NULL);
            /* http_error() closes socket for us, so don't try to
             * disconnect it below */
        } else {
            set_timeout(&clients[i]);
            sock_setcb(new, SCB_READLINE, do_readline);
            sock_setcb(new, SCB_DISCONNECT, do_disconnect);
            sock_set_blocking(new, 1);
            if (LogConnections)
                module_log("Accepted connection from %s",
                           clients[i].address);
        }
        return;
    }
    disconn(new);
}

/*************************************************************************/

static void do_disconnect(Socket *socket, void *param_unused)
{
    Client *c = find_client(socket);
    int index = c - clients;

    if (!c) {
        module_log("BUG: unexpected disconnect callback for socket %p",socket);
        return;
    }
    clear_timeout(c);
    free(c->headers);
    free(c->variables);
    free(c->request_buf);
    ARRAY_REMOVE(clients, index);
}

/*************************************************************************/

/* Read a line from a client socket.  If the end of the request data is
 * reached, the request is passed to handle_request(); if the request data
 * length exceeds the buffer size, an error is sent to the client and the
 * connection is closed.
 */

static void do_readline(Socket *socket, void *param_unused)
{
    Client *c = find_client(socket);
    char line[HTTP_LINEMAX], *linestart, *s;
    int32 i;

    if (!c) {
        module_log("BUG: unexpected readline callback for socket %p", socket);
        disconn(socket);
        return;
    }

    if (!sgets(line, sizeof(line), socket) || *line == 0) {
        module_log("BUG: sgets() failed in readline callback for socket %p",
                   socket);
        return;
    }
    i = strlen(line);
    if (line[i-1] != '\n') {
        module_log("%s: Request/header line too long, closing connection",
                   c->address);
        http_error(c, HTTP_E_REQUEST_ENTITY_TOO_LARGE, NULL);
        return;
    }
    line[--i] = 0;
    if (i > 0 && line[i-1] == '\r')
        line[--i] = 0;
    i++;  /* include trailing \0 */
    if (c->request_len + i > RequestBufferSize) {
        module_log("%s: Request too large, closing connection", c->address);
        http_error(c, HTTP_E_REQUEST_ENTITY_TOO_LARGE, NULL);
        return;
    }
    linestart = c->request_buf + c->request_len;
    memcpy(linestart, line, i);
    c->request_len += i;

    if (!c->url) {
        char *method, *url, *version;
        if (!*linestart) {
            /* RFC2616 4.2: servers SHOULD ignore initial empty lines (OK) */
            c->request_len = 0;
            set_timeout(c);
            return;
        }
        method  = strtok(linestart, " ");
        url     = strtok(NULL,      " ");
        version = strtok(NULL,      " ");
        if (!method || !url || !version) {
            /* Note that we don't support REALLY old clients (HTTP 0.9)
             * which don't send version strings */
            /* RFC2616 10.4: SHOULD ensure client has received error before
             * closing connection (NG) -- in this case the client is so
             * broken that it doesn't deserve to be worried about */
            module_log("%s: Invalid HTTP request", c->address);
            http_error(c, HTTP_E_BAD_REQUEST, NULL);
            return;
        }
        if (strcmp(method, "GET") == 0) {
            c->method = METHOD_GET;
        } else if (strcmp(method, "HEAD") == 0) {
            c->method = METHOD_HEAD;
        } else if (strcmp(method, "POST") == 0) {
            c->method = METHOD_POST;
        } else {
            module_log("%s: Unimplemented/unsupported method `%s' requested",
                       c->address, method);
            http_error(c, HTTP_F_NOT_IMPLEMENTED, NULL);
            return;
        }
        if (strncmp(version, "HTTP/", 5) != 0 || !(s = strchr(version+5,'.'))){
            module_log("%s: Bad HTTP version string: %s", c->address, version);
            http_error(c, HTTP_E_BAD_REQUEST, NULL);
            return;
        }
        *s++ = 0;
        c->version_major = (int)atolsafe(version+5, 0, INT_MAX);
        c->version_minor = (int)atolsafe(s, 0, INT_MAX);
        if (c->version_major < 0 || c->version_minor < 0) {
            module_log("%s: Bad HTTP version string: %s.%s",
                       c->address, version, s);
            http_error(c, HTTP_E_BAD_REQUEST, NULL);
            return;
        }
        if (c->version_major != 1) {
            module_log("%s: Unsupported HTTP version: %d.%d",
                       c->address, c->version_major, c->version_minor);
            http_error(c, HTTP_F_HTTP_VER_NOT_SUPPORTED, NULL);
            return;
        }
        if (strnicmp(url, "http://", 7) == 0) {
            /* RFC2616 5.1.2: MUST accept absolute URIs (OK, but we ignore
             * the hostname and just pretend it's us) */
            s = strchr(url+7, '/');
            if (s) {
                strmove(url, s);
            } else {
                url[0] = '/';
                url[1] = 0;
            }
        }
        c->url = url;
        set_timeout(c);
        return;
    } /* if (!url) */

    /* We already have the URL, so this must be a header or blank line. */
    if (*linestart) {
        /* Header line: process it */
        parse_header(c, linestart);
        set_timeout(c);
        return;
    }

    /* End of headers.  For GET/HEAD, handle any query string present in
     * the URL and process the request immediately.  For POST, handle
     * Expect: 100-continue, then deal with the body. */

    if (c->method == METHOD_GET || c->method == METHOD_HEAD) {
        char *s = strchr(c->url, '?');
        if (s) {
            *s++ = 0;
            c->data = s;
            c->data_len = strlen(s);
        }
        handle_request(c);
    } else if (c->method == METHOD_POST) {
        long length;
        s = http_get_header(c, "Content-Length");
        if (!s) {
            module_log("%s: Missing Content-Length header for POST",
                       c->address);
            http_error(c, HTTP_E_LENGTH_REQUIRED, NULL);
            return;
        }
        errno = 0;
        length = atolsafe(s, 0, LONG_MAX);
        if (length < 0) {
            module_log("%s: Invalid Content-Length header: %s", c->address, s);
            http_error(c, HTTP_E_BAD_REQUEST, NULL);
            return;
        } else if (c->request_len+length>RequestBufferSize) {
            module_log("%s: Request too large, closing connection",c->address);
            http_error(c, HTTP_E_REQUEST_ENTITY_TOO_LARGE, NULL);
            return;
        }
        c->data = c->request_buf + c->request_len;
        c->data_len = (int32)length;
        if (length > 0) {
            s = http_get_header(c, "Expect");
            for (s = strtok(s, ", \t"); s; s = strtok(NULL, ", \t")) {
                if (strcmp(s, "100-continue") == 0) {
                    sockprintf(socket, "HTTP/1.1 100 Continue\r\n\r\n");
                    break;
                }
            }
            /* Set up to read POST data */
            sock_setcb(socket, SCB_READ, do_readdata);
            sock_setcb(socket, SCB_READLINE, NULL);
            set_timeout(c);
        } else {
            /* length == 0: do it just like GET */
            handle_request(c);
        }
    } else {
        module_log("BUG: do_readline(): unsupported method %d", c->method);
        http_error(c, HTTP_F_INTERNAL_SERVER_ERROR, NULL);
    }

}

/*************************************************************************/

/* Read data from a client socket.  When the end of the data is reached,
 * reached, the request is passed to handle_request().  The request is
 * assumed to have already been checked for exceeding the buffer size.
 */

static void do_readdata(Socket *socket, void *param)
{
    Client *c = find_client(socket);
    int32 available = (int32)(long)param, needed, nread;

    if (!c) {
        module_log("BUG: unexpected readdata callback for socket %p", socket);
        disconn(socket);
        return;
    }

    needed = c->data_len - (c->request_len - (c->data - c->request_buf));
    if (available > needed)
        available = needed;
    if (c->request_len + available > RequestBufferSize) {
        module_log("BUG: do_readdata(%s[%s]): data size exceeded buffer limit",
                   c->address, c->url);
        http_error(c, HTTP_F_INTERNAL_SERVER_ERROR, NULL);
        return;
    }
    nread = sread(socket, c->request_buf + c->request_len, available);
    if (nread != available) {
        module_log("BUG: do_readdata(%s[%s]): nread (%d) != available (%d)",
                   c->address, c->url, nread, available);
    }
    c->request_len += nread;
    needed -= nread;
    if (needed <= 0) {
        /* Prepare for next request, if any */
        sock_setcb(socket, SCB_READ, NULL);
        sock_setcb(socket, SCB_READLINE, do_readline);
        /* Process this request */
        handle_request(c);
    }
}

/*************************************************************************/

/* Handle an idle timeout on a client. */

static void do_timeout(Timeout *t)
{
    Client *c = find_client(t->data);
    if (!c) {
        module_log("BUG: do_timeout(): client not found for timeout %p!", t);
        return;
    }
    c->timeout = NULL;
    disconn(c->socket);
}

/*************************************************************************/
/*************************************************************************/

/* Return the client structure corresponding to the given socket, or NULL
 * if not found.
 */

static Client *find_client(Socket *s)
{
    int i;

    ARRAY_FOREACH (i, clients) {
        if (clients[i].socket == s)
            return &clients[i];
    }
    return NULL;
}

/*************************************************************************/

/* Start an idle timer running on the given client.  If a timer was already
 * running, clear it and start a new one.
 */

static void set_timeout(Client *c)
{
    if (!c->socket) {
        module_log("BUG: attempt to set timeout for client %d with no"
                   " socket!", (int)(c-clients));
        return;
    }
    if (IdleTimeout) {
        clear_timeout(c);
        c->timeout = add_timeout(IdleTimeout, do_timeout, 0);
        c->timeout->data = c->socket;
    }
}

/*************************************************************************/

/* Cancel the idle timer for the given client. */

static void clear_timeout(Client *c)
{
    if (c->timeout) {
        del_timeout(c->timeout);
        c->timeout = NULL;
    }
}

/*************************************************************************/

/* Do appropriate things with the given header line. */

static void parse_header(Client *c, char *linestart)
{
    char *s;

    /* Check for whitespace-started line and no headers yet */
    if ((*linestart == ' ' || *linestart == '\t') && !c->headers_count) {
        http_error(c, HTTP_E_BAD_REQUEST, NULL);
        return;
    }

    /* Remove all trailing whitespace */
    s = linestart + strlen(linestart) - 1;
    while (s > linestart && (*s == ' ' || *s == '\t')) {
        *s-- = 0;
        c->request_len--;
    }

    if (*linestart == ' ' || *linestart == '\t') {
        /* If it starts with whitespace, just tack it onto the end of the
         * previous line (convert all leading whitespace to a single space) */
        linestart[-1] = ' ';
        s = linestart;
        while (*s == ' ' || *s == '\t')
            s++;
        strmove(linestart, s);
        c->request_len -= s-linestart;
    } else {
        /* New header: split into name and value, and create a new
         * headers[] entry */
        int i = c->headers_count;
        ARRAY_EXTEND(c->headers);
        c->headers[i] = linestart;
        s = strchr(linestart, ':');
        if (!s) {
            http_error(c, HTTP_E_BAD_REQUEST, NULL);
            return;
        }
        *s++ = 0;
        linestart = s;
        while (*s == ' ' || *s == '\t')
            s++;
        strmove(linestart, s);
        c->request_len -= s - linestart;
    }
}

/*************************************************************************/

/* Parse the data given by `buf' (null-terminated) into variables and
 * create an array of them in c->variables.  Assume standard
 * x-www-form-urlencoding encoding (for multipart data, use
 * parse_data_multipart() below).
 */

static void parse_data(Client *c, char *buf)
{
    char *start;
    int found_equals = 0;
    char hexbuf[3];

    hexbuf[2] = 0;
    free(c->variables);
    c->variables = NULL;
    c->variables_count = 0;
    start = buf;

    ARRAY_EXTEND(c->variables);
    c->variables[0] = start;
    while (*buf) {
        switch (*buf) {
          case '=':
            if (!found_equals) {
                *buf = 0;
                http_unquote_url(start);
                found_equals = 1;
                start = buf+1;
            }
            break;
          case '&':
            *buf = 0;
            http_unquote_url(start);
            found_equals = 0;
            start = buf+1;
            ARRAY_EXTEND(c->variables);
            c->variables[c->variables_count-1] = start;
            break;
        }
        buf++;
    }
}

/*************************************************************************/

/* Parse the data given by `buf' (null-terminated) into variables and
 * create an array of them in c->variables, using the boundary string
 * given by `boundary'.
 */

static void parse_data_multipart(Client *c, char *buf, const char *boundary)
{
    char *dest = buf;
    int boundarylen = strlen(boundary);
    char *varname = NULL;

    free(c->variables);
    c->variables = NULL;
    c->variables_count = 0;

    buf = strstr(buf, boundary);
    if (!buf)
        return;  /* boundary string not found */

    while (*buf && (buf[boundarylen+2] != '-' || buf[boundarylen+3] != '-')) {
        char *s;

        /* Read in header for this part */
        s = buf + strcspn(buf, "\r\n");
        if (!*s)
            return;
        buf = s + strspn(s, "\r") + 1;
        while (*buf != '\r' && *buf != '\n') {
            s = buf + strcspn(buf, "\r\n");
            if (!*s)
                return;
            if (*s == '\r')
                *s++ = 0;
            *s++ = 0;
            if (strnicmp(buf,"Content-Disposition:",20) == 0) {
                buf += 20;
                while (*buf && isspace(*buf))
                    buf++;
                if (*buf && strnicmp(buf,"form-data;",10) == 0) {
                    buf += 10;
                    while (*buf && isspace(*buf))
                        buf++;
                    if (*buf && strnicmp(buf,"name=",5) == 0) {
                        buf += 5;
                        if (*buf == '"') {
                            char *t = strchr(++buf, '"');
                            if (t)
                                *t = 0;
                        } else {
                            char *t = strchr(buf, ';');
                            if (t)
                                *t = 0;
                        }
                        varname = dest;
                        strmove(dest, buf);
                        dest += strlen(buf)+1;
                    }
                }
            }
            buf = s;
        } /* while (*buf != '\r' && *buf != '\n') */
        if (*buf == '\r')
            buf++;
        buf++;

        /* Read in data (variable contents) */
        if (varname) {
            ARRAY_EXTEND(c->variables);
            c->variables[c->variables_count-1] = varname;
            varname = NULL;
        }
        s = buf + strcspn(buf, "\r\n");
        if (s > buf) {
            memmove(dest, buf, s-buf);
            dest += s-buf;
        }
        if (*s == '\r')
            s++;
        buf = s;
        /* *buf is always pointing to a \n or \0 at the top of this loop */
        while (*buf && (buf[1] != '-' || buf[2] != '-'
               || strncmp(buf+3, boundary, boundarylen) != 0)) {
            s = buf+1 + strcspn(buf+1, "\r\n");
            if (!s)
                s = buf + strlen(buf);
            memmove(dest, buf, s-buf);
            dest += s-buf;
            if (*s == '\r')
                s++;
            buf = s;
        }
        /* Null-terminate variable contents */
        *dest++ = 0;
        /* Skip over newline */
        if (*buf)
            buf++;
    } /* while not final boundary line */
}

/*************************************************************************/

static void handle_request(Client *c)
{
    int res;
    int close = 0;

    /* Parse GET query string or POST data into variables */
    if (c->data && c->data_len) {
        char *s;
        if (c->method == METHOD_POST) {
            /* There were at least two newlines before the beginning of the
             * data, so it's safe to move it back a byte (to add a trailing
             * null) */
            memmove(c->data-1, c->data, c->data_len);
            c->data--;
            c->data[c->data_len] = 0;
        }
        /* Check the content type, and extract the boundary string if it's
         * multipart data */
        s = http_get_header(c, "Content-Type");
        if (s && strnicmp(s, "multipart/form-data;", 20) == 0) {
            s += 20;
            while (isspace(*s))
                s++;
            if (strnicmp(s, "boundary=", 9) == 0) {
                s += 9;
                if (*s == '"') {
                    char *t = strchr(++s, '"');
                    if (t)
                        *t = 0;
                }
            } else {
                s = NULL;
            }
        } else {
            s = NULL;
        }
        /* Parse data into variables */
        if (s)
            parse_data_multipart(c, c->data, s);
        else
            parse_data(c, c->data);
    }

    c->request_count++;
    c->in_request = 1;

    if (c->version_major == 1 && c->version_minor == 0) {
        close = 1;
    } else {
        const char *s = http_get_header(c, "Connection");
        if (s && strstr(s, "close")) {
            /* This might accidentally trigger on something like "abcloseyz",
             * but that's okay; all it means is the client has to reconnect */
            close = 1;
        }
    }

    res = call_callback_2(cb_auth, c, &close);
    if (res < 0) {
        module_log("handle_request(): call_callback(cb_request) failed");
        http_error(c, HTTP_F_INTERNAL_SERVER_ERROR, NULL);
        close = 1;
    } else if (res != HTTP_AUTH_DENY) {
        res = call_callback_2(cb_request, c, &close);
        if (res < 0) {
            module_log("handle_request(): call_callback(cb_request) failed");
            http_error(c, HTTP_F_INTERNAL_SERVER_ERROR, NULL);
            close = 1;
        } else if (res == 0) {
            http_error(c, HTTP_E_NOT_FOUND, NULL);
        }
    }

    if (close || (MaxRequests && c->request_count >= MaxRequests)
     || c->in_request < 0  /* flag from http_error */
    ) {
        disconn(c->socket);
    } else {
        free(c->headers);
        free(c->variables);
        c->in_request      = 0;
        c->request_len     = 0;
        c->version_major   = 0;
        c->version_minor   = 0;
        c->method          = -1;
        c->url             = NULL;
        c->data            = NULL;
        c->data_len        = 0;
        c->headers         = NULL;
        c->headers_count   = 0;
        c->variables       = NULL;
        c->variables_count = 0;
        set_timeout(c);
    }
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

static int do_ListenTo(const char *filename, int linenum, char *param);

ConfigDirective module_config[] = {
    { "IdleTimeout",      { { CD_TIME, 0, &IdleTimeout } } },
    { "ListenBacklog",    { { CD_POSINT, CF_DIRREQ, &ListenBacklog } } },
    { "ListenTo",         { { CD_FUNC, CF_DIRREQ, do_ListenTo } } },
    { "LogConnections",   { { CD_SET, 0, &LogConnections } } },
    { "MaxConnections",   { { CD_POSINT, 0, &MaxConnections } } },
    { "MaxRequests",      { { CD_POSINT, 0, &MaxRequests } } },
    { "RequestBufferSize",{ { CD_POSINT, 0, &RequestBufferSize } } },
    { NULL }
};

/*************************************************************************/

static int do_ListenTo(const char *filename, int linenum, char *param)
{
    char *s;
    int port;
    uint8 *ip;
    char *ipstr;
    char ipbuf[15+1];  /* aaa.bbb.ccc.ddd\0 */
    int recursing = 0, i;
    static struct listento_ *new_ListenTo;
    static int new_ListenTo_count;

    if (!filename) {
        /* filename == NULL, perform special operations */
        switch (linenum) {
          case CDFUNC_INIT:     /* prepare to read new data */
            free(new_ListenTo);
            new_ListenTo = NULL;
            new_ListenTo_count = 0;
            break;
          case CDFUNC_SET:      /* store new data in config variable */
            free(ListenTo);
            ListenTo = new_ListenTo;
            ListenTo_count = new_ListenTo_count;
            new_ListenTo = NULL;
            new_ListenTo_count = 0;
            break;
          case CDFUNC_DECONFIG: /* clear any stored data */
            free(ListenTo);
            ListenTo = NULL;
            ListenTo_count = 0;
            break;
        } /* switch (linenum) */
        return 1;
    } /* if (!filename) */

    /* filename != NULL, process directive */

    if (linenum < 0) {
        recursing = 1;
        linenum = -linenum;
    }

    if (ListenTo_count >= MAX_LISTENTO) {
        config_error(filename, linenum,
                     "Too many ListenTo addresses (maximum %d)",
                     MAX_LISTENTO);
        return 0;
    }

    s = strchr(param, ':');
    if (!s) {
        config_error(filename, linenum,
                     "ListenTo address requires both address and port");
        return 0;
    }

    *s++ = 0;
    port = atolsafe(s, 1, 65535);
    if (port < 1) {
        config_error(filename, linenum, "Invalid port number `%s'", s);
        return 0;
    }

    if (strcmp(param, "*") == 0) {
        /* "*" -> all addresses (NULL string) */
        ipstr = NULL;
    } else if ((ip = pack_ip(param)) != NULL) {
        /* IP address -> normalize (no leading zeros, etc.) */
        snprintf(ipbuf, sizeof(ipbuf), "%u.%u.%u.%u",
                 ip[0], ip[1], ip[2], ip[3]);
        if (strlen(ipbuf) > 15) {
            config_error(filename, linenum, "BUG: strlen(ipbuf) > 15 [%s]",
                         ipbuf);
            return 0;
        }
        ipstr = ipbuf;
    } else {
        /* hostname -> check for double recursion, then look up and
         *             recursively add addresses */
#ifdef HAVE_GETHOSTBYNAME
        struct hostent *hp;
#endif
        if (recursing) {
            config_error(filename, linenum, "BUG: double recursion (param=%s)",
                         param);
            return 0;
        }
#ifdef HAVE_GETHOSTBYNAME
        if ((hp = gethostbyname(param)) != NULL) {
            if (hp->h_addrtype == AF_INET) {
                for (i = 0; hp->h_addr_list[i]; i++) {
                    ip = (uint8 *)hp->h_addr_list[i];
                    snprintf(ipbuf, sizeof(ipbuf), "%u.%u.%u.%u",
                             ip[0], ip[1], ip[2], ip[3]);
                    if (strlen(ipbuf) > 15) {
                        config_error(filename, linenum,
                                     "BUG: strlen(ipbuf) > 15 [%s]", ipbuf);
                        return 0;
                    }
                    if (!do_ListenTo(filename, -linenum, ipbuf))
                        return 0;
                }
                return 1;  /* Success */
            } else {                
                config_error(filename, linenum, "%s: no IPv4 addresses found",
                             param);
            }
        } else {
            config_error(filename, linenum, "%s: %s", param,
                         hstrerror(h_errno));
        }
#else
        config_error(filename, linenum,
                     "gethostbyname() not available, hostnames may not be"
                     " used");
#endif
        return 0;
    }

    i = new_ListenTo_count;
    ARRAY_EXTEND(new_ListenTo);
    if (ipstr)
        strcpy(new_ListenTo[i].ip, ipstr);/*safe: strlen(ip)<16 checked above*/
    else
        memset(new_ListenTo[i].ip, 0, sizeof(new_ListenTo[i].ip));
    new_ListenTo[i].port = port;
    return 1;
}

/*************************************************************************/

int init_module()
{
    int i, opencount;

    cb_auth    = register_callback("auth");
    cb_request = register_callback("request");
    if (cb_auth < 0 || cb_request < 0) {
        module_log("Unable to register callbacks");
        exit_module(0);
        return 0;
    }

    listen_sockets = smalloc(sizeof(*listen_sockets) * ListenTo_count);
    opencount = 0;
    ARRAY_FOREACH (i, ListenTo) {
        listen_sockets[i] = sock_new();
        if (listen_sockets[i]) {
            if (open_listener(listen_sockets[i],
                              *ListenTo[i].ip ? ListenTo[i].ip : NULL,
                              ListenTo[i].port, ListenBacklog) == 0) {
                sock_setcb(listen_sockets[i], SCB_ACCEPT, do_accept);
                module_log("Listening on %s:%u",
                           ListenTo[i].ip, ListenTo[i].port);
                opencount++;
            } else {
                module_log_perror("Failed to open listen socket for %s:%u",
                                  ListenTo[i].ip, ListenTo[i].port);
            }
        } else {
            module_log("Failed to create listen socket for %s:%u",
                       *ListenTo[i].ip ? ListenTo[i].ip : "*",
                       ListenTo[i].port);
        }
    }
    if (!opencount) {
        module_log("No ports could be opened, aborting");
        return 0;
    }

    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    int i;

    ARRAY_FOREACH (i, ListenTo) {
        if (listen_sockets[i]) {
            close_listener(listen_sockets[i]);
            sock_free(listen_sockets[i]);
        }
    }
    free(ListenTo);
    ListenTo = NULL;
    ListenTo_count = 0;
    free(listen_sockets);
    listen_sockets = NULL;

    unregister_callback(cb_request);
    unregister_callback(cb_auth);

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
