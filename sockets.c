/* Socket routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

/* Define this to trace function calls: */
/* #define TRACE_CALLS */

/* Define this to log warnings on exceeding buffer size: */
/* #define WARN_ON_BUFSIZE */

/* Define this to disable swritemap().  For IRC Services, we disable this
 * to avoid depending on the presence of munmap(), since we don't use
 * swritemap() anyway. */
#define DISABLE_SWRITEMAP

#include "services.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef DISABLE_SWRITEMAP
# include <sys/mman.h>  /* for munmap() */
#endif

/*************************************************************************/

/* For function call tracing: */

#ifdef TRACE_CALLS

static int trace_depth = -1;

#define ENTER(fmt,...)                                                  \
    trace_depth++;                                                      \
    log_debug(1, "sockets: TRACE: %*s%s(" fmt ")",                      \
              trace_depth*2, "", __FUNCTION__ , ## __VA_ARGS__)
#define ENTER_WITH(retfmt,fmt,...)                                      \
    const char *__retfmt = (retfmt);                                    \
    ENTER(fmt , ## __VA_ARGS__)
#define RETURN do {                                                     \
    log_debug(1, "sockets: TRACE: %*s%s() -> void",                     \
              trace_depth*2, "", __FUNCTION__);                         \
    trace_depth--;                                                      \
    return;                                                             \
} while (0)
#define RETURN_WITH(val) do {                                           \
    typeof(val) __tmp = (val);                                          \
    if (debug) {                                                        \
        char buf[64];                                                   \
        snprintf(buf, sizeof(buf), __retfmt, __tmp);                    \
        log_debug(1, "sockets: TRACE: %*s%s() -> %s",                   \
                  trace_depth*2, "", __FUNCTION__, buf);                \
    }                                                                   \
    trace_depth--;                                                      \
    return __tmp;                                                       \
} while (0)

#else  /* !TRACE_CALLS */

#define ENTER(fmt,...)                  /* nothing */
#define ENTER_WITH(retfmt,fmt,...)      /* nothing */
#define RETURN                          return
#define RETURN_WITH(val)                return(val)

#endif  /* TRACE_CALLS */

/*************************************************************************/

/* Socket data structure */

struct socket_ {
    Socket *next, *prev;
    int fd;                     /* Socket's file descriptor */
    struct sockaddr_in remote;  /* Remote address */
    int flags;                  /* Status flags (SF_*) */

    SocketCallback cb_connect;  /* Connect callback */
    SocketCallback cb_disconn;  /* Disconnect callback */
    SocketCallback cb_accept;   /* Accept callback */
    SocketCallback cb_read;     /* Data-available callback */
    SocketCallback cb_readline; /* Line-available callback */
    SocketCallback cb_trigger;  /* Write trigger callback */

    int write_timeout;          /* Write timeout, in seconds (0 = none) */
    time_t last_write_time;     /* Last time data was successfully sent */

    /* Usage of pointers:
     *    - Xbuf is the buffer base pointer
     *    - Xptr is the address of the next character to store
     *    - Xend is the address of the next character to retrieve
     *    - Xtop is the address of the last byte of the buffer + 1
     * Xend-Xptr (mod Xbufsize) gives the number of bytes in the buffer.
     */
    char *rbuf, *rptr, *rend, *rtop;  /* Read buffer and pointers */
    char *wbuf, *wptr, *wend, *wtop;  /* Write buffer and pointers */

    /* List of mapped memory areas to send, earliest first */
    struct wmapinfo {
        struct wmapinfo *next;
        int32 wait;             /* # of bytes to write from wbuf first */
        const char *map;        /* Start of mapped area, or trigger data */
        int32 maplen;           /* Length of mapped area (0 = write trigger) */
        int32 pos;              /* Current position */
    } *writemap, *writemap_tail;

    uint64 total_read;          /* Total number of bytes read */
    uint64 total_written;       /* Total number of bytes written */
};

#define SF_SELFCREATED  0x0001  /* We created this socket ourselves */
#define SF_BLOCKING     0x0002  /* Writes are blocking */
#define SF_LISTENER     0x0004  /* Socket is a listener socket */
#define SF_CONNECTING   0x0008  /* Socket is busy connecting */
#define SF_CONNECTED    0x0010  /* Socket has connected */
#define SF_MUTE         0x0020  /* Socket is muted */
#define SF_UNMUTED      0x0040  /* Socket was just unmuted */
#define SF_CALLBACK     0x0080  /* Currently calling callbacks */
#define SF_WTRIGGER     0x0100  /* A write trigger has been reached */
#define SF_WARNED       0x0200  /* Warned about hitting buffer limit */
#define SF_DISCONNECT   0x0400  /* Disconnect when writebuf empty */
#define SF_DISCONN_REQ  0x0800  /* Disconnect has been requested */
#define SF_DISCONNECTING 0x1000 /* Socket is in the middle of disconnecting */
#define SF_BROKEN       0x2000  /* Connection was broken while in a callback */
#define SF_DELETEME     0x4000  /* Delete socket when convenient */

/* Used when calling do_disconn() due to SF_DISCONNECT */
#define DISCONN_RESUME_FLAG     0x100
#define DISCONN_LOCAL_RESUME    (void *)((int)DISCONN_LOCAL|DISCONN_RESUME_FLAG)
#define DISCONN_REMOTE_RESUME   (void *)((int)DISCONN_REMOTE|DISCONN_RESUME_FLAG)

/* Size of read/write buffers */
#define read_buffer_size(s)  ((s)->rtop - (s)->rbuf)
#define write_buffer_size(s) ((s)->wtop - (s)->wbuf)

/* Does the socket still have more data that needs to be written? */
#define MORE_TO_WRITE(s)     (write_buffer_len(s) > 0 || ((s)->writemap))

/*************************************************************************/

/* List of all sockets (even unopened ones) */
static Socket *allsockets = NULL;

/* Array of all opened sockets (indexed by FD), dynamically allocated */
static Socket **sockets = NULL;

/* Highest FD number in use plus 1; also length of sockets[] array */
static int max_fd;

/* Set of all connected socket FDs */
static fd_set sock_fds;

/* Set of all FDs that need data written (or are connecting) */
static fd_set write_fds;

/* Total memory used by socket buffers */
static uint32 total_bufsize;

/* Per-connection and total buffer size limits */
static uint32 bufsize_limit = 0, total_bufsize_limit = 0;

/* Global read timeout, in milliseconds */
static int read_timeout = -1;

/*************************************************************************/

/* Internal routine declarations (definitions at bottom of file) */

static int do_callback(Socket *s, SocketCallback cb, void *param);
static void do_accept(Socket *s);
static int fill_read_buffer(Socket *s);
static int flush_write_buffer(Socket *s);
static uint32 resize_how_much(const Socket *s, uint32 current_size, int *errp);
static int resize_rbuf(Socket *s, uint32 size);
static int resize_wbuf(Socket *s, uint32 size);
static int resize_buf(char **p_buf, char **p_ptr, char **p_end, char **p_top,
                      uint32 newsize);
static int reclaim_buffer_space_one(Socket *s);
static int reclaim_buffer_space(void);
static void next_wmap(Socket *s);
static int buffered_write(Socket *s, const char *buf, int len);
static int do_disconn(Socket *s, void *code);
static void sock_closefd(Socket *s);

/*************************************************************************/
/*************************** Global routines *****************************/
/*************************************************************************/

/* Set the per-connection and total buffer size limits (zero means no
 * limit).  Always successful; both values are silently rounded down to a
 * multiple of SOCK_MIN_BUFSIZE (if a value is less than SOCK_MIN_BUFSIZE it
 * is rounded up).  Both values default to zero, i.e. unlimited.
 */

void sock_set_buflimits(uint32 per_conn, uint32 total)
{
    if (per_conn > 0) {
        per_conn = per_conn / SOCK_MIN_BUFSIZE * SOCK_MIN_BUFSIZE;
        if (!per_conn)
            per_conn = SOCK_MIN_BUFSIZE;
    }
    if (total > 0) {
        total = total / SOCK_MIN_BUFSIZE * SOCK_MIN_BUFSIZE;
        if (!total)
            total = SOCK_MIN_BUFSIZE;
    }
    bufsize_limit = per_conn;
    total_bufsize_limit = total;
}

/*************************************************************************/

/* Set the global read timeout.  A value of -1 (default) means no timeout. */

void sock_set_rto(int msec)
{
    read_timeout = msec;
}

/*************************************************************************/

/* Create and return a new socket.  Returns NULL if unsuccessful (i.e. no
 * more space for buffers).
 */

Socket *sock_new(void)
{
    Socket *s;

    ENTER_WITH("%p", "");
    if (total_bufsize_limit) {
        while (total_bufsize + SOCK_MIN_BUFSIZE*2 > total_bufsize_limit) {
            if (!reclaim_buffer_space()) {
                log("sockets: sock_new(): out of buffer space! current=%lu,"
                    " limit=%lu", (unsigned long)total_bufsize,
                    (unsigned long)total_bufsize_limit);
                RETURN_WITH(NULL);
            }
        }
    }

    s = malloc(sizeof(*s));
    if (!s) {
        RETURN_WITH(NULL);
    }
    s->rbuf = malloc(SOCK_MIN_BUFSIZE);
    if (!s->rbuf) {
        int errno_save = errno;
        free(s);
        errno = errno_save;
        RETURN_WITH(NULL);
    }
    s->wbuf = malloc(SOCK_MIN_BUFSIZE);
    if (!s->wbuf) {
        int errno_save = errno;
        free(s->rbuf);
        free(s);
        errno = errno_save;
        RETURN_WITH(NULL);
    }

    s->fd              = -1;
    memset(&s->remote, 0, sizeof(s->remote));
    s->flags           = 0;
    s->cb_connect      = NULL;
    s->cb_disconn      = NULL;
    s->cb_accept       = NULL;
    s->cb_read         = NULL;
    s->cb_readline     = NULL;
    s->cb_trigger      = NULL;
    s->write_timeout   = 0;
    s->last_write_time = time(NULL);
    s->rptr            = s->rbuf;
    s->rend            = s->rbuf;
    s->rtop            = s->rbuf + SOCK_MIN_BUFSIZE;
    s->wptr            = s->wbuf;
    s->wend            = s->wbuf;
    s->wtop            = s->wbuf + SOCK_MIN_BUFSIZE;
    s->writemap        = NULL;
    s->writemap_tail   = NULL;
    s->total_read      = 0;
    s->total_written   = 0;
    LIST_INSERT(s, allsockets);
    total_bufsize += read_buffer_size(s) + write_buffer_size(s);
    RETURN_WITH(s);
}

/*************************************************************************/

/* Free a socket, first disconnecting/closing it if necessary. */

void sock_free(Socket *s)
{
    ENTER("%p", s);
    if (!s) {
        log("sockets: sock_free() with NULL socket!");
        errno = EINVAL;
        RETURN;
    }
    if (s->flags & (SF_CONNECTING | SF_CONNECTED)) {
        s->flags |= SF_DELETEME;
        do_disconn(s, DISCONN_LOCAL);
        /* do_disconn() will call us again at the appropriate time */
        RETURN;
    } else if (s->flags & SF_LISTENER) {
        close_listener(s);
    }
    LIST_REMOVE(s, allsockets);
    total_bufsize -= read_buffer_size(s) + write_buffer_size(s);
    free(s->rbuf);
    free(s->wbuf);
    free(s);
    /* If this was the last socket, free the sockets[] array too */
    if (!allsockets) {
        free(sockets);
        sockets = NULL;
    }
    RETURN;
}

/*************************************************************************/

/* Set a callback on a socket. */

void sock_setcb(Socket *s, SocketCallbackID which, SocketCallback func)
{
    ENTER("%p,%d,%p", s, which, func);
    if (!s) {
        log("sockets: sock_setcb() with NULL socket!");
        errno = EINVAL;
        RETURN;
    }
    switch (which) {
      case SCB_CONNECT:    s->cb_connect  = func; break;
      case SCB_DISCONNECT: s->cb_disconn  = func; break;
      case SCB_ACCEPT:     s->cb_accept   = func; break;
      case SCB_READ:       s->cb_read     = func; break;
      case SCB_READLINE:   s->cb_readline = func; break;
      case SCB_TRIGGER:    s->cb_trigger  = func; break;
      default:
        log("sockets: sock_setcb(): invalid callback ID %d", which);
        break;
    }
    RETURN;
}

/*************************************************************************/

/* Return whether the given socket is currently connected. */

int sock_isconn(const Socket *s)
{
    ENTER_WITH("%d", "%p", s);
    if (!s) {
        log("sockets: sock_isconn() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(0);
    }
    RETURN_WITH(s->flags & SF_CONNECTED ? 1 : 0);
}

/*************************************************************************/

/* Retrieve address of remote end of socket.  Functions the same way as
 * getpeername() (initialize *lenptr to sizeof(sa), address returned in sa,
 * non-truncated length of address returned in *lenptr).  Returns -1 with
 * errno == EINVAL if a NULL pointer is passed or the given socket is not
 * connected.
 */

int sock_remote(const Socket *s, struct sockaddr *sa, int *lenptr)
{
    ENTER_WITH("%d", "%p,%p,%p", s, sa, lenptr);
    if (!s || !sa || !lenptr || !(s->flags & SF_CONNECTED)) {
        if (!s || !sa || !lenptr) {
            log("sockets: sock_remote() with NULL %s!",
                !s ? "socket" : !sa ? "sockaddr" : "lenptr");
        }
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    if (sizeof(s->remote) <= *lenptr)
        memcpy(sa, &s->remote, sizeof(s->remote));
    else
        memcpy(sa, &s->remote, *lenptr);
    *lenptr = sizeof(s->remote);
    RETURN_WITH(0);
}

/*************************************************************************/

/* Set whether socket writes should block (blocking != 0) or not
 * (blocking == 0).
 */

void sock_set_blocking(Socket *s, int blocking)
{
    ENTER("%p,%d", s, blocking);
    if (!s) {
        log("sockets: sock_set_blocking() with NULL socket!");
        errno = EINVAL;
        RETURN;
    }
    if (blocking)
        s->flags |= SF_BLOCKING;
    else
        s->flags &= ~SF_BLOCKING;
    RETURN;
}

/*************************************************************************/

/* Return whether socket writes are blocking (return value > 0) or not
 * (return value == 0).  Returns -1 if socket is invalid.
 */

int sock_get_blocking(const Socket *s)
{
    ENTER_WITH("%d", "%p", s);
    if (!s) {
        log("sockets: sock_get_blocking() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    RETURN_WITH(s->flags & SF_BLOCKING);
}

/*************************************************************************/

/* Set (or clear, if seconds==0) the write timeout for a socket. */

void sock_set_wto(Socket *s, int seconds)
{
    ENTER("%p,%d", s, seconds);
    if (!s || seconds < 0) {
        log("sockets: sock_set_wto() with %s!",
            !s ? "NULL socket" : "negative timeout");
        errno = EINVAL;
        RETURN;
    }
    s->write_timeout = seconds;
    RETURN;
}

/*************************************************************************/

/* Mute a socket.  When a socket is muted, no arriving data will be
 * acknowledged (except to the extent that the operating system
 * automatically acknowledges and buffers the data), and the read and
 * accept callbacks will not be called.  However, a socket in the
 * process of conenction will still call the connect or disconnect
 * callbacks when it completes or fails.
 */

void sock_mute(Socket *s)
{
    ENTER("%p", s);
    if (!s) {
        log("sockets: sock_mute() with NULL socket!");
        RETURN;
    }
    if (!(s->flags & SF_MUTE)) {
        if (s->fd >= 0)
            FD_CLR(s->fd, &sock_fds);
        s->flags |= SF_MUTE;
    }
    RETURN;
}

/*************************************************************************/

/* Unmute a socket.  If any data is waiting, it will be acknowledged and
 * the appropriate callback (read or accept) called during the next call to
 * check_sockets().
 */

void sock_unmute(Socket *s)
{
    ENTER("%p", s);
    if (!s) {
        log("sockets: sock_unmute() with NULL socket!");
        RETURN;
    }
    if (s->flags & SF_MUTE) {
        if (s->fd >= 0)
            FD_SET(s->fd, &sock_fds);
        s->flags &= ~SF_MUTE;
        s->flags |= SF_UNMUTED;
    }
    RETURN;
}

/*************************************************************************/

/* Return amount of data in read buffer.  Assumes socket is valid. */

inline uint32 read_buffer_len(const Socket *s)
{
    if (s->rend >= s->rptr)
        return s->rend - s->rptr;
    else
        return (s->rend + read_buffer_size(s)) - s->rptr;
}


/*************************************************************************/

/* Return amount of data in write buffer.  Assumes socket is valid. */

inline uint32 write_buffer_len(const Socket *s)
{
    if (s->wend >= s->wptr)
        return s->wend - s->wptr;
    else
        return (s->wend + write_buffer_size(s)) - s->wptr;
}

/*************************************************************************/

/* Return total number of bytes received and sent on this socket in
 * *read_ret and *writekb_ret respectively.  Sent data count does not
 * include buffered but unsent data.  Returns 0 on success, -1 on error
 * (invalid socket).
 */

int sock_rwstat(const Socket *s, uint64 *read_ret, uint64 *written_ret)
{
    ENTER_WITH("%u", "%p,%p,%p", s, read_ret, written_ret);
    if (!s) {
        log("sockets: sock_rwstat() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    if (read_ret)
        *read_ret = s->total_read;
    if (written_ret)
        *written_ret = s->total_written;
    RETURN_WITH(0);
}

/*************************************************************************/

/* Return the larger of (1) the ratio of the given socket's total buffer
 * size (read and write buffers combined) to bufsize_limit and (2) the
 * ratio of the amount of memory used by all sockets' buffers to
 * total_bufsize_limit, as a percentage rounded up to the next integer.
 * Ratios (1) and (2) are zero if bufsize_limit or total_bufsize_limit,
 * respectively, are set to zero (unlimited).
 *
 * If any of `socksize_ret', `totalsize_ret', `ratio1_ret', and
 * `ratio2_ret' are non-NULL, they are set respectively to the given
 * socket's total buffer size, the amount of memory used by all sockets'
 * buffers, ratio (1) as a percentage, and ratio (2) as a percentage.
 *
 * If `s' is NULL, ratio (1) is set to zero, and *socksize_ret is not
 * modified.
 */

int sock_bufstat(const Socket *s, uint32 *socksize_ret,
                 uint32 *totalsize_ret, int *ratio1_ret, int *ratio2_ret)
{
    int ratio1 = 0, ratio2 = 0;

    ENTER_WITH("%d", "%p,%p,%p,%p,%p", s, socksize_ret, totalsize_ret,
               ratio1_ret, ratio2_ret);
    if (bufsize_limit && s) {
        uint32 size = read_buffer_size(s) + write_buffer_size(s);
        if (bufsize_limit <= 0x7FFFFFFF/100)
            ratio1 = (size*100 + bufsize_limit-1) / bufsize_limit;
        else
            ratio1 = (size + bufsize_limit/100-1) / (bufsize_limit/100);
    }
    if (total_bufsize_limit) {
        if (bufsize_limit <= 0x7FFFFFFF/100)
            ratio2 = (total_bufsize*100 + total_bufsize_limit-1)
                   / total_bufsize_limit;
        else
            ratio2 = (total_bufsize + total_bufsize_limit/100-1)
                   / (total_bufsize_limit/100);
    }
    if (socksize_ret && s)
        *socksize_ret = read_buffer_size(s) + write_buffer_size(s);
    if (totalsize_ret)
        *totalsize_ret = total_bufsize;
    if (ratio1_ret)
        *ratio1_ret = ratio1;
    if (ratio2_ret)
        *ratio2_ret = ratio2;
    if (ratio1 > ratio2)
        RETURN_WITH(ratio1);
    else
        RETURN_WITH(ratio2);
}

/*************************************************************************/
/*************************************************************************/

/* Check all sockets for activity, and call callbacks as necessary.
 * Returns after activity has been detected on at least one socket.
 */

void check_sockets(void)
{
    fd_set rfds, wfds;
    int i;
    int32 res;
    struct timeval tv;
    time_t now = time(NULL);
    Socket *s;

    ENTER("");
    rfds = sock_fds;
    wfds = write_fds;
    if (read_timeout < 0) {
        tv.tv_sec = -1;  // flag: use a NULL tv parameter to select()
        tv.tv_usec = 0;
    } else {
        tv.tv_sec = read_timeout/1000;
        tv.tv_usec = (read_timeout%1000) * 1000;
    }
    for (i = 0; i < max_fd; i++) {
        s = sockets[i];
        if (s && s->write_timeout && MORE_TO_WRITE(s)) {
            int tleft = s->write_timeout - (now - s->last_write_time);
            if (tleft < 0)
                tleft = 0;
            if (tv.tv_sec < 0 || tleft <= tv.tv_sec) {
                tv.tv_sec = tleft;
                tv.tv_usec = 0;
            }
        }
    }
    enable_signals();
    do {
        struct timeval thistv = tv;
        res = select(max_fd, &rfds, &wfds, NULL, tv.tv_sec<0?NULL:&thistv);
    } while (res < 0 && errno == EINTR);
    disable_signals();
    log_debug(3, "sockets: select returned %d", res);
    if (res < 0) {
        log_perror("sockets: select()");
        RETURN;
    }

    for (i = 0; i < max_fd; i++) {
        s = sockets[i];
        if (!s)
            continue;
        if (s->fd != i) {
            log("sockets: BUG: sockets[%d]->fd = %d (should be equal),"
                " clearing socket from table", i, s->fd);
            sockets[i] = NULL;
            continue;
        }

        if (FD_ISSET(i, &wfds)) {
            log_debug(3, "sockets: write ready on fd %d", i);
            if (!s) {
                log("sockets: BUG: got write-ready on fd %d but no socket"
                    " for it!", i);
                continue;
            } else if (s->flags & SF_CONNECTING) {
                /* Connection established (or failed) */
                int val;
                socklen_t vallen;
                vallen = sizeof(val);
                log_debug(2, "sockets: connect on fd %d returned", i);
                if (getsockopt(i, SOL_SOCKET, SO_ERROR, &val, &vallen) < 0) {
                    log_perror("sockets: getsockopt(SO_ERROR) for connect (%d"
                               " -> %s:%u)", i, inet_ntoa(s->remote.sin_addr),
                               htons(s->remote.sin_port));
                    do_disconn(s, DISCONN_CONNFAIL);
                    continue;
                }
                if (val != 0) {
                    errno = val;
                    log_perror("sockets: connect(%d -> %s:%u)", i,
                               inet_ntoa(s->remote.sin_addr),
                               htons(s->remote.sin_port));
                    do_disconn(s, DISCONN_CONNFAIL);
                    continue;
                } else {
                    if (!do_callback(s, s->cb_connect, 0))
                        continue;
                }
                s->flags &= ~SF_CONNECTING;
                s->flags |= SF_CONNECTED;
                FD_CLR(i, &write_fds);
                if (flush_write_buffer(s) < 0)  /* we may have pending data */
                    continue;  /* socket is not valid any more */
                if (!(s->flags & SF_MUTE))
                    FD_SET(i, &sock_fds);
            } else if (!(s->flags & SF_CONNECTED)) {
                log("sockets: BUG: got write-ready on fd %d but socket not"
                    " connected!", i);
            } else {
                if (flush_write_buffer(s) < 0)
                    continue;  /* socket is not valid any more */
            }
        } /* set in write fds */

        while (s->flags & SF_WTRIGGER) {
            /* Write trigger reached */
            void *userdata = (void *)s->writemap->map;
            next_wmap(s);
            s->flags &= ~SF_WTRIGGER;
            do_callback(s, s->cb_trigger, userdata);
        }

        if (FD_ISSET(i, &rfds) || (s && (s->flags & SF_UNMUTED))) {
            log_debug(3, "sockets: %s on fd %d",
                      FD_ISSET(i,&rfds) ? "read ready" : "unmute", i);
            if (!s) {
                log("sockets: BUG: got data on fd %d but no socket for it!",i);
                FD_CLR(i, &sock_fds);
                continue;
            }
            if (s->flags & SF_MUTE) {
                /* Socket was muted by a previous socket's callback */
                log_debug(3, "sockets: socket %d has been muted", i);
                continue;
            }
            s->flags &= ~SF_UNMUTED;
            if (FD_ISSET(i, &rfds)) {
                if (s->flags & SF_LISTENER) {
                    /* Connection arrived */
                    do_accept(s);
                    continue;
                } else if (!(s->flags & SF_CONNECTED)) {
                    log("sockets: BUG: got data on fd %d but not connected!",
                        i);
                    FD_CLR(i, &sock_fds);
                    continue;
                }
                /* Normal read */
                if (read_buffer_len(s) >= read_buffer_size(s)-1) {
                    /* Buffer is full, try to expand it */
                    int newsize = 0;
                    if (read_buffer_size(s) < SOCK_MIN_BUFSIZE)
                        newsize = SOCK_MIN_BUFSIZE;
                    else if (read_buffer_size(s)+SOCK_MIN_BUFSIZE<bufsize_limit)
                        newsize = read_buffer_size(s) + SOCK_MIN_BUFSIZE;
                    if (newsize > 0)
                        resize_rbuf(s, newsize);
                }
                res = fill_read_buffer(s);
                if (res < 0) {
                    /* Connection was closed (or some other error occurred) */
                    if (res < 0)
                        log_perror_debug(1, "sockets: recv(%d)", i);
                    do_disconn(s, DISCONN_REMOTE);
                    continue;
                } else if (res == 0) {
                    /* Read buffer is full (NOTE: this causes busy waiting
                     * until space is made available) */
                }
            } else {
                res = read_buffer_len(s);
            }
            if (res > 0) {
                uint32 left = read_buffer_len(s), newleft;
                if (left == 0) {
                    log("sockets: BUG: 0 bytes avail after successful read!");
                    continue;
                }
                /* Call read callback(s) in a loop until no more data is
                 * left or neither callback takes any data, or the socket is
                 * disconnected */
                newleft = left;
                do {
                    if (!do_callback(s, s->cb_read, (void *)(long)newleft))
                        goto disconnected;
                    if (s->flags & SF_MUTE)
                        break;
                    newleft = read_buffer_len(s);
                    if (s->cb_readline) {
                        char *newline;
                        if (s->rend > s->rptr) {
                            newline = memchr(s->rptr, '\n', newleft);
                        } else {
                            newline = memchr(s->rptr, '\n', s->rtop - s->rptr);
                            if (!newline)
                                newline = memchr(s->rbuf, '\n',
                                                 s->rend - s->rbuf);
                        }
                        if (newline) {
                            if (!do_callback(s, s->cb_readline,
                                             (void *)(long)newleft))
                                goto disconnected;
                            if (s->flags & SF_MUTE)
                                break;
                            newleft = read_buffer_len(s);
                        }
                    }
                } while (newleft != left && (left = newleft) != 0);
                reclaim_buffer_space_one(s);
            }
        } /* socket ready for reading or unmuted */

        if (s->write_timeout
         && MORE_TO_WRITE(s)
         && s->last_write_time + s->write_timeout <= now
        ) {
            if (s->flags & SF_DISCONNECT)
                do_disconn(s, DISCONN_LOCAL_RESUME);
            else
                do_disconn(s, DISCONN_REMOTE);
        }

      disconnected:
        /* at least one statement is required after a label */ ;

    } /* for all sockets */

    RETURN;
} /* check_sockets() */

/*************************************************************************/
/*************************************************************************/

/* Initiate a connection to the given host and port.  If an error occurs,
 * returns -1, else returns 0.  The connection is not necessarily complete
 * even if 0 is returned, and may later fail; use the SCB_CONNECT and
 * SCB_DISCONNECT callbacks.  If this function fails due to inability to
 * resolve a hostname, errno will be set to the negative of h_errno; pass
 * the negative of this value to hstrerror() to get an appropriate error
 * message.
 *
 * lhost/lport specify the local side of the connection.  If they are not
 * given (lhost==NULL, lport==0), then they are left to be set by the OS.
 *
 * If either host or lhost is not a valid IP address and the gethostbyname()
 * function is available, this function may block while the hostname is
 * being resolved.
 *
 * This function may be called from a socket's disconnect callback to
 * establish a new connection using the same socket.  It may not be called,
 * however, if the socket is being freed with sock_free().
 */

int conn(Socket *s, const char *host, int port, const char *lhost, int lport)
{
#if HAVE_GETHOSTBYNAME
    struct hostent *hp;
#endif
    uint8 *addr;
    struct sockaddr_in sa, lsa;
    int fd, i;

    ENTER_WITH("%d", "%p,[%s],%d,[%s],%d", s, host?host:"(null)", port,
               lhost?lhost:"(null)", lport);
    if (!s || !host || port <= 0 || port > 65535) {
        if (port <= 0 || port > 65535)
            log("sockets: conn() with bad port number (%d)!", port);
        else
            log("sockets: conn() with NULL %s!", !s ? "socket" : "hostname");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    if (s->flags & SF_DELETEME) {
        log("sockets: conn() called on a freeing socket (%p)", s);
        errno = EPERM;
        return -1;
    }
    memset(&lsa, 0, sizeof(lsa));
    lsa.sin_family = AF_INET;
    if (lhost) {
        if ((addr = pack_ip(lhost)) != 0)
            memcpy((char *)&lsa.sin_addr, addr, 4);
#if HAVE_GETHOSTBYNAME
        else if ((hp = gethostbyname(lhost)) != NULL)
            memcpy((char *)&lsa.sin_addr, hp->h_addr, hp->h_length);
#endif
        else
            lhost = NULL;
    }
    if (lport)
        lsa.sin_port = htons(lport);

    memset(&sa, 0, sizeof(sa));
    if ((addr = pack_ip(host)) != 0) {
        memcpy((char *)&sa.sin_addr, addr, 4);
        sa.sin_family = AF_INET;
    }
#if HAVE_GETHOSTBYNAME
    else if ((hp = gethostbyname(host)) != NULL) {
        memcpy((char *)&sa.sin_addr, hp->h_addr, hp->h_length);
        sa.sin_family = hp->h_addrtype;
    } else {
        errno = -h_errno;
        RETURN_WITH(-1);
    }
#else
    else {
        log("sockets: conn(): `%s' is not a valid IP address", host);
        errno = EINVAL;
        RETURN_WITH(-1);
    }
#endif
    sa.sin_port = htons((uint16)port);

    if ((fd = socket(sa.sin_family, SOCK_STREAM, 0)) < 0)
        RETURN_WITH(-1);

    if (fcntl(fd, F_SETFL, O_NDELAY) < 0) {
        int errno_save = errno;
        close(fd);
        errno = errno_save;
        RETURN_WITH(-1);
    }

    if ((lhost || lport) && bind(fd,(struct sockaddr *)&lsa,sizeof(sa)) < 0) {
        int errno_save = errno;
        close(fd);
        errno = errno_save;
        RETURN_WITH(-1);
    }

    if ((i = connect(fd, (struct sockaddr *)&sa, sizeof(sa))) < 0
        && errno != EINPROGRESS
    ) {
        int errno_save = errno;
        close(fd);
        errno = errno_save;
        RETURN_WITH(-1);
    }

    if (max_fd < fd+1) {
        int j;
        void *new_sockets = realloc(sockets, (fd+1) * sizeof(*sockets));
        if (!new_sockets) {
            int errno_save = errno;
            close(fd);
            errno = errno_save;
            RETURN_WITH(-1);
        }
        sockets = new_sockets;
        for (j = max_fd; j < fd; j++)
            sockets[j] = NULL;
        max_fd = fd+1;
    }
    sockets[fd] = s;
    s->remote = sa;
    s->fd = fd;
    s->flags &= ~(SF_CONNECTING | SF_CONNECTED);
    if (i == 0) {
        s->flags |= SF_CONNECTED;
        if (!(s->flags & SF_MUTE))
            FD_SET(fd, &sock_fds);
        do_callback(s, s->cb_connect, 0);
    } else {
        s->flags |= SF_CONNECTING;
        FD_SET(fd, &write_fds);
    }
    RETURN_WITH(0);
}

/*************************************************************************/

/* Disconnect a socket.  Returns 0 on success, -1 on error (s == NULL or
 * listener socket).  Calling this routine on an already-disconnected
 * socket returns success without doing anything.  Note that the socket may
 * not be disconnected immediately; callers who intend to reuse the socket
 * MUST wait until the disconnect callback is called before doing so.
 */

int disconn(Socket *s)
{
    ENTER_WITH("%d", "%p", s);
    if (!s) {
        log("sockets: disconn() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    } else if (s->flags & SF_LISTENER) {
        log("sockets: disconn() with listener socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    RETURN_WITH(do_disconn(s, DISCONN_LOCAL));
}

/*************************************************************************/

/* Open a listener socket on the given host and port; returns 0 on success
 * (the socket is set up and listening), -1 on error.  If `host' has
 * multiple addresses, only the first one is used; if `host' is NULL, all
 * addresses are bound to.  As with conn(), a negative errno value
 * indicates a failure to resolve the hostname `host'.  `backlog' is the
 * backlog limit for incoming connections, and is passed directly to the
 * listen() system call.
 *
 * Note that if the SCB_ACCEPT callback is not set, any connections to the
 * socket will be dropped immediately.
 *
 * If host is not a valid IP address and the gethostbyname() function is
 * available, this function may block while the hostname is being resolved.
 */

int open_listener(Socket *s, const char *host, int port, int backlog)
{
#if HAVE_GETHOSTBYNAME
    struct hostent *hp;
#endif
    uint8 *addr;
    struct sockaddr_in sa;
    int fd, i;

    ENTER_WITH("%d", "%p,[%s],%d,%d", s, host?host:"(null)", port, backlog);
    if (!s || port <= 0 || port > 65535 || backlog < 1) {
        if (port <= 0 || port > 65535)
            log("sockets: open_listener() with bad port number (%d)!", port);
        else if (backlog < 1)
            log("sockets: open_listener() with bad backlog (%d)!", backlog);
        else
            log("sockets: open_listener() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    memset(&sa, 0, sizeof(sa));
    if (host) {
        if ((addr = pack_ip(host)) != 0) {
            memcpy((char *)&sa.sin_addr, addr, 4);
            sa.sin_family = AF_INET;
        }
#if HAVE_GETHOSTBYNAME
        else if ((hp = gethostbyname(host)) != NULL) {
            memcpy((char *)&sa.sin_addr, hp->h_addr, hp->h_length);
            sa.sin_family = hp->h_addrtype;
        } else {
            errno = -h_errno;
            RETURN_WITH(-1);
        }
#else
        else {
            log("sockets: open_listener(): `%s' is not a valid IP address",
                host);
            errno = EINVAL;
            RETURN_WITH(-1);
        }
#endif
    } else {  /* !host */
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = INADDR_ANY;
    }
    sa.sin_port = htons((uint16)port);

    if ((fd = socket(sa.sin_family, SOCK_STREAM, 0)) < 0)
        RETURN_WITH(-1);

    i = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0) {
        log_perror("sockets: open_listener(): setsockopt(%d, SO_REUSEADDR,"
                   " 1) failed", fd);
    }

    if (fcntl(fd, F_SETFL, O_NDELAY) < 0) {
        int errno_save = errno;
        close(fd);
        errno = errno_save;
        RETURN_WITH(-1);
    }

    if (bind(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        int errno_save = errno;
        close(fd);
        errno = errno_save;
        RETURN_WITH(-1);
    }

    if (listen(fd, backlog) < 0) {
        int errno_save = errno;
        close(fd);
        errno = errno_save;
        RETURN_WITH(-1);
    }

    if (max_fd < fd+1) {
        int j;
        void *new_sockets = realloc(sockets, (fd+1) * sizeof(*sockets));
        if (!new_sockets) {
            int errno_save = errno;
            close(fd);
            errno = errno_save;
            RETURN_WITH(-1);
        }
        sockets = new_sockets;
        for (j = max_fd; j < fd; j++)
            sockets[j] = NULL;
        max_fd = fd+1;
    }
    sockets[fd] = s;
    if (!(s->flags & SF_MUTE))
        FD_SET(fd, &sock_fds);
    s->fd = fd;
    s->flags |= SF_LISTENER;

    /* Listener sockets don't need read/write buffers */
    free(s->rbuf);
    free(s->wbuf);
    s->rbuf = s->rptr = s->rend = s->rtop = NULL;
    s->wbuf = s->wptr = s->wend = s->wtop = NULL;

    RETURN_WITH(0);
}

/*************************************************************************/

/* Close a listener socket. */

int close_listener(Socket *s)
{
    ENTER_WITH("%d", "%p", s);
    if (s == NULL || !(s->flags & SF_LISTENER)) {
        if (s)
            log("sockets: close_listener() with non-listener socket (%d)!",
                s->fd);
        else
            log("sockets: close_listener() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    sock_closefd(s);
    s->flags &= ~SF_LISTENER;
    RETURN_WITH(0);
}

/*************************************************************************/

/* Read raw data from a socket, like read().  Returns number of bytes read,
 * or -1 on error.  Only reads from the buffer (does not attempt to fetch
 * more data from the connection).
 */

int32 sread(Socket *s, char *buf, int32 len)
{
    int32 nread = 0;

    ENTER_WITH("%d", "%p,%p,%d", s, buf, len);
    if (!s || !buf || len <= 0) {
        log("sockets: sread() with %s!",
            !s ? "NULL socket" : !buf ? "NULL buffer" : "len <= 0");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    if (s->rend < s->rptr) {
        /* Buffer data wraps around */
        if (s->rptr + len <= s->rtop) {
            /* Only need to read from end of buffer */
            memcpy(buf, s->rptr, len);
            s->rptr += len;
            if (s->rptr >= s->rtop)
                s->rptr -= read_buffer_size(s);
            RETURN_WITH(len);
        } else {
            /* Need to read from both end and beginning */
            nread = s->rtop - s->rptr;
            memcpy(buf, s->rptr, nread);
            s->rptr = s->rbuf;
            len -= nread;
            /* Continue below */
        }
    }
    /* Read data from beginning of buffer */
    if (s->rptr < s->rend) {
        if (len > s->rend - s->rptr)
            len = s->rend - s->rptr;
        memcpy(buf+nread, s->rptr, len);
        s->rptr += len;
        nread += len;
    }
    /* Return number of bytes read */
    RETURN_WITH(nread);
}

/*************************************************************************/

/* Write raw data to a socket, like write().  Returns number of bytes
 * written, or -1 on error.
 */

int32 swrite(Socket *s, const char *buf, int32 len)
{
    ENTER_WITH("%d", "%p,%p,%d", s, buf, len);
    if (!s || !buf || len < 0) {
        log("sockets: swrite() with %s!",
            !s ? "NULL socket" : !buf ? "NULL buffer" : "len < 0");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    RETURN_WITH(buffered_write(s, buf, len));
}

/*************************************************************************/

/* Write data from an mmap()ed area to a socket.  The area will be
 * automatically unmapped when the write completes.
 */

int32 swritemap(Socket *s, const char *buf, int32 len)
{
#ifndef DISABLE_SWRITEMAP
    struct wmapinfo *info;

    ENTER_WITH("%d", "%p,%p,%d", s, buf, len);
    if (!s || !buf || len < 0) {
        log("sockets: swritemap() with %s!",
            !s ? "NULL socket" : !buf ? "NULL buffer" : "len < 0");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    if (!len)
        RETURN_WITH(0);
    info = malloc(sizeof(*info));
    if (!info)
        RETURN_WITH(-1);
    info->next = NULL;
    info->wait = write_buffer_len(s);
    info->map = buf;
    info->maplen = len;
    info->pos = 0;
    if (s->writemap) {
        if (!s->writemap_tail) {
            log("sockets: BUG: socket %d: writemap non-NULL but writemap_tail"
                " NULL", s->fd);
            s->writemap_tail = s->writemap;
            while (s->writemap_tail->next)
                s->writemap_tail = s->writemap_tail->next;
        }
        s->writemap_tail->next = info;
    } else {
        if (s->writemap_tail) {
            log("sockets: BUG: socket %d: writemap NULL but writemap_tail"
                " non-NULL", s->fd);
        }
        s->writemap = info;
    }
    s->writemap_tail = info;
    flush_write_buffer(s);  /* to ensure FD is set in write_fds if needed */
    RETURN_WITH(len);
#else  /* DISABLE_SWRITEMAP */
    errno = ENOSYS;
    RETURN_WITH(-1);
#endif
}

/*************************************************************************/

/* Add a write trigger to the socket.  The `data' parameter will be passed
 * to the callback when the trigger is reached.  Returns 0 on success, -1
 * on failure.
 */

int swrite_trigger(Socket *s, void *data)
{
    struct wmapinfo *info;

    ENTER_WITH("%d", "%p,%p", s, data);
    if (!s) {
        log("sockets: swrite_trigger() with NULL socket!");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    info = malloc(sizeof(*info));
    if (!info)
        RETURN_WITH(-1);
    info->next = NULL;
    info->wait = write_buffer_len(s);
    info->map = (const char *)data;
    info->maplen = 0;
    info->pos = 0;
    if (s->writemap) {
        if (!s->writemap_tail) {
            log("sockets: BUG: socket %d: writemap non-NULL but writemap_tail"
                " NULL", s->fd);
            s->writemap_tail = s->writemap;
            while (s->writemap_tail->next)
                s->writemap_tail = s->writemap_tail->next;
        }
        s->writemap_tail->next = info;
    } else {
        if (s->writemap_tail) {
            log("sockets: BUG: socket %d: writemap NULL but writemap_tail"
                " non-NULL", s->fd);
        }
        s->writemap = info;
    }
    s->writemap_tail = info;
    flush_write_buffer(s);  /* to ensure FD is set in write_fds if needed */
    RETURN_WITH(0);
}

/*************************************************************************/
/*************************************************************************/

/* Read a character from a socket, like fgetc().  Returns EOF if no data
 * is available in the socket buffer.  Assumes the socket is valid.
 */

int sgetc(Socket *s)
{
    int c;

    ENTER_WITH("%d", "%p", s);
    /* No paranoia check here, to save time */
    if (s->rptr == s->rend)
        RETURN_WITH(EOF);
    c = *s->rptr++;
    if (s->rptr >= s->rtop)
        s->rptr -= read_buffer_size(s);
    RETURN_WITH(c);
}

/*************************************************************************/

/* Read a line from a socket, like fgets().  If not enough buffered data
 * is available to fill a complete line, or another error occurs, returns
 * NULL.
 */

char *sgets(char *buf, int32 len, Socket *s)
{
    char *ptr = s->rptr, *eol;
    int32 to_top = s->rtop - ptr;  /* used for efficiency */

    ENTER_WITH("%p", "%p,%d,%p", buf, len, s);
    if (!s || !buf || len <= 0) {
        log("sockets: sgets[2]() with %s!",
            !s ? "NULL socket" : !buf ? "NULL buffer" : "len <= 0");
        RETURN_WITH(NULL);
    }

    /* Find end of line */
    if (s->rend > s->rptr) {
        eol = memchr(s->rptr, '\n', s->rend - s->rptr);
    } else {
        eol = memchr(s->rptr, '\n', to_top);
        if (!eol)
            eol = memchr(s->rbuf, '\n', s->rend - s->rbuf);
    }
    if (!eol)
        RETURN_WITH(NULL);
    eol++;                      /* Point 1 byte after \n */

    /* Set rptr now; old value is in ptr variable */
    s->rptr = eol;
    if (s->rptr >= s->rtop)     /* >rtop is impossible, but just in case */
        s->rptr = s->rbuf;

    /* Note: The greatest possible value for eol is s->rend, so as long as
     * we ensure that rend doesn't wrap around and reach rptr (i.e. always
     * leave at least 1 byte in the buffer unused), we can never have
     * eol == ptr here. */

    /* Trim eol to <len bytes */
    if (eol > ptr) {
        if (eol-ptr >= len)
            eol = ptr + len-1;
    } else {
        if (to_top >= len-1)  /* we don't mind eol == rtop */
            eol = ptr + len-1;
        else if (to_top + (eol - s->rbuf) >= len)
            eol = s->rbuf + (len-1 - to_top);
    }

    /* Actually copy to buffer and return */
    if (eol > ptr) {
        memcpy(buf, ptr, eol-ptr);
        buf[eol-ptr] = 0;
    } else {
        memcpy(buf, ptr, to_top);
        memcpy(buf+to_top, s->rbuf, eol - s->rbuf);
        buf[to_top + (eol - s->rbuf)] = 0;
    }
    RETURN_WITH(buf);
}

/*************************************************************************/

/* Reads a line of text from a socket, and strips newline and carriage
 * return characters from the end of the line.
 */

char *sgets2(char *buf, int32 len, Socket *s)
{
    char *str;

    ENTER_WITH("%p", "%p,%d,%p", buf, len, s);
    str = sgets(buf, len, s);
    if (!str)
        RETURN_WITH(str);
    str = buf + strlen(buf)-1;
    if (*str == '\n')
        *str-- = 0;
    if (*str == '\r')
        *str = 0;
    RETURN_WITH(buf);
}

/*************************************************************************/

/* Write a string to a socket, like fputs().  Returns the number of bytes
 * written.
 */

int sputs(const char *str, Socket *s)
{
    ENTER_WITH("%d", "[%s],%p", str?str:"(null)", s);
    if (!str || !s) {
        log("sockets: sputs() with %s!",
            !s ? "NULL socket" : "NULL string");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    RETURN_WITH(buffered_write(s, str, strlen(str)));
}

/*************************************************************************/

/* Write to a socket a la [v]printf().  Returns the number of bytes written;
 * in no case will more than 65535 bytes be written (if the output would be
 * be longer than this, it will be truncated).
 */

int sockprintf(Socket *s, const char *fmt, ...)
{
    va_list args;
    int ret;

    ENTER_WITH("%d", "%p,[%s],...", s, fmt?fmt:"(null)");
    va_start(args, fmt);
    ret = vsockprintf(s, fmt, args);
    va_end(args);
    RETURN_WITH(ret);
}

int vsockprintf(Socket *s, const char *fmt, va_list args)
{
    char buf[65536];

    ENTER_WITH("%d", "%p,[%s],%p", s, fmt?fmt:"(null)", args);
    if (!s || !fmt) {
        log("sockets: [v]sockprintf() with %s!",
            !s ? "NULL socket" : "NULL format string");
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    RETURN_WITH(buffered_write(s, buf, vsnprintf(buf,sizeof(buf),fmt,args)));
}

/*************************************************************************/
/************************** Internal routines ****************************/
/*************************************************************************/

/* Call a callback on the given socket.  Handles setting and clearing
 * SF_CALLBACK, and checking for remote disconnects after the callback
 * completes.  Returns 0 if the socket has been disconnected, 1 otherwise.
 * If 0 is returned, the socket should be considered no longer valid; if it
 * was an automatically-created socket, the socket itself will have been
 * freed.
 *
 * `s' or `cb' may be NULL, in which case this routine does nothing and
 * returns 1.
 */

static int do_callback(Socket *s, SocketCallback cb, void *param)
{
    ENTER_WITH("%d", "%p,%p,%p", s, cb, param);
    if (s && cb) {
        /* Check whether the SF_CALLBACK flag was set; if it is, then this
         * is a nested callback, so don't clear the flag or disconnect the
         * socket afterwards. */
        int was_set = s->flags & SF_CALLBACK;

        if (!was_set)
            s->flags |= SF_CALLBACK;
        log_debug(3, "sockets: callback(%d,%p,%p)", s->fd, cb, param);
        cb(s, param);
        log_debug(3, "sockets: ...cbret(%d,%p,%p)", s->fd, cb, param);
        if (!was_set) {
            s->flags &= ~SF_CALLBACK;
            if (s->flags & SF_BROKEN) {
                log_debug(3, "sockets: ...broken");
                do_disconn(s, DISCONN_REMOTE_RESUME);
                RETURN_WITH(0);
            }
            if (s->flags & SF_DELETEME) {
                log_debug(3, "sockets: ...deleteme");
                sock_free(s);
                RETURN_WITH(0);
            }
            if ((s->flags & SF_DISCONNECT) || s->fd < 0) {
                log_debug(3, "sockets: ...disconnect");
                RETURN_WITH(0);
            }
            log_debug(3, "sockets: ...normalret");
        } else {  /* SF_CALLBACK was already set */
            log_debug(3, "sockets: ...nestedret");
        }
    }
    RETURN_WITH(1);
}

/*************************************************************************/

/* Accept a connection on the given socket.  Called from check_sockets(). */

static void do_accept(Socket *s)
{
    int i;
    struct sockaddr_in sin;
    socklen_t sin_len = sizeof(sin);
    int newfd;

    ENTER("%p", s);
    newfd = accept(s->fd, (struct sockaddr *)&sin, &sin_len);
    if (newfd < 0) {
        if (errno != ECONNRESET)
            log_perror("sockets: accept(%d)", s->fd);
    } else if (!s->cb_accept) {
        /* No accept callback, so just throw the connection away */
        close(newfd);
    } else if (fcntl(newfd, F_SETFL, O_NDELAY) < 0) {
        log_perror("sockets: fcntl(NDELAY) on accept(%d)", s->fd);
        close(newfd);
    } else {
        Socket *news = sock_new();
        if (!news) {
            log("sockets: accept(%d): Unable to create socket structure"
                " (out of buffer space?)", s->fd);
        } else {
            news->fd = newfd;
            news->flags |= SF_SELFCREATED | SF_CONNECTED;
            memcpy(&news->remote, &sin, sin_len);
            for (i = newfd; i < max_fd; i++);
            if (max_fd < newfd+1) {
                void *new_sockets =
                               realloc(sockets, (newfd+1) * sizeof(*sockets));
                if (!new_sockets) {
                    sock_free(news);
                    close(newfd);
                    RETURN;
                }
                sockets = new_sockets;
                for (i = max_fd; i < newfd; i++)
                    sockets[i] = NULL;
                max_fd = newfd+1;
            }
            sockets[newfd] = news;
            if (!(news->flags & SF_MUTE))
                FD_SET(newfd, &sock_fds);
            do_callback(s, s->cb_accept, news);
        }
    }
    RETURN;
}

/*************************************************************************/

/* Fill up the read buffer of a socket with any data that may have arrived.
 * Returns the number of bytes read (nonzero), or -1 on error; errno is set
 * by recv() calls but is otherwise preserved.
 */

static int fill_read_buffer(Socket *s)
{
    int nread = 0;
    int errno_save = errno;

    ENTER_WITH("%d", "%p", s);
    if (s->fd < 0) {
        errno = EBADF;
        RETURN_WITH(-1);
    }
    for (;;) {
        int maxread, res;
        if (read_buffer_len(s) >= read_buffer_size(s)-1) {
            /* Read buffer is full; try to expand the buffer. */
            int over;
            int32 more = resize_how_much(s, read_buffer_size(s), &over);
            if (more) {
                if (!resize_rbuf(s, read_buffer_size(s) + more)) {
                    log("sockets: attempt to expand socket %d read buffer"
                        " failed (out of memory?)", s->fd);
                    errno_save = EAGAIN;
                    if (nread == 0)
                        nread = -1;
                    break;
                }
            } else {
#ifdef WARN_ON_BUFSIZE
                if (!(s->flags & SF_WARNED)) {
                    log("sockets: socket %d exceeded %s buffer size limit"
                        " (%d)", s->fd, over ? "per-connection" : "total",
                        over ? bufsize_limit : total_bufsize_limit);
                    s->flags |= SF_WARNED;
                }
#endif
                errno_save = EAGAIN;
                if (nread == 0)
                    nread = -1;
                break;
            }
        }
        if (s->rend < s->rptr)  /* wrapped around? */
            maxread = (s->rptr-1) - s->rend;
        else if (s->rptr == s->rbuf)
            maxread = s->rtop - s->rend - 1;
        else
            maxread = s->rtop - s->rend;
        do {
            errno = 0;
            res = recv(s->fd, s->rend, maxread, 0);
            if (res <= 0 && errno == 0)
                errno = ECONNRESET;  /* make a guess */
        } while (res <= 0 && errno == EINTR);
        errno_save = errno;
        log_debug(3, "sockets: fill_read_buffer wanted %d, got %d",
                  maxread, nread);
        if (res <= 0) {
            if (nread == 0)
                nread = -1;
            break;
        }
        nread += res;
        s->total_read += res;
        s->rend += res;
        if (s->rend == s->rtop)
            s->rend = s->rbuf;
    }
    if (nread == 0) {
        nread = -1;
        errno = ENOBUFS;
    } else {
        errno = errno_save;
    }
    RETURN_WITH(nread);
}

/*************************************************************************/

/* Try and write up to one chunk of data from the buffer to the socket.
 * Return -1 on error, -2 if a socket waiting for disconnection was closed,
 * or else how many bytes were written.  (Note that a return value of zero
 * should not be considered an error.)
 */

static int flush_write_buffer(Socket *s)
{
    int32 maxwrite, nwritten;

    ENTER_WITH("%d", "%p", s);
    if (s->fd < 0) {
        errno = EBADF;
        RETURN_WITH(-1);
    }
    if (!sock_isconn(s))  /* not yet connected */
        RETURN_WITH(0);
    if (s->flags & SF_WTRIGGER) {
        /* Write trigger is still pending on this socket, don't do anything */
        nwritten = 0;
        goto check_result;
    }
    /* "while" instead of "if" to handle the case of:
     *     s->writemap->wait == 0
     *     s->writemap->pos == s->writemap->maplen (should be impossible here)
     *     s->writemap->next->wait == 0
     */
    while (s->writemap && s->writemap->wait <= 0) {
        /* Write data from the map instead of the buffer */
        if (!s->writemap->maplen) {
            /* It's a trigger, stop here.  We don't skip to the next wmap
             * yet in case we're called again before the trigger callback
             * is called. */
            s->flags |= SF_WTRIGGER;
            nwritten = 0;  /* return value */
            goto check_result;
        }
        maxwrite = s->writemap->maplen - s->writemap->pos;
        if (maxwrite > 0) {
            nwritten = send(s->fd, s->writemap->map + s->writemap->pos,
                            maxwrite, 0);
            log_debug(3, "sockets: flush_write_buffer/map wanted %d, got %d",
                      maxwrite, nwritten);
            if (nwritten > 0) {
                s->writemap->pos += nwritten;
                if (s->writemap->pos >= s->writemap->maplen)
                    next_wmap(s);
            }
            goto check_result;
        } else {
            next_wmap(s);
        }
    }
    nwritten = 0;
    if (s->wend != s->wptr) {
        if (s->wptr > s->wend)  /* wrapped around? */
            maxwrite = s->wtop - s->wptr;
        else
            maxwrite = s->wend - s->wptr;
        if (s->writemap && maxwrite > s->writemap->wait) {
            maxwrite = s->writemap->wait;
            if (maxwrite <= 0) {
                /* paranoia: this is impossible from the while() above */
                log("sockets: BUG: flush_write_buffer(%d): writemap wait 0"
                    " after while", s->fd);
                do_disconn(s, DISCONN_REMOTE);
                RETURN_WITH(-1);
            }
        }
        nwritten = send(s->fd, s->wptr, maxwrite, 0);
        log_debug(3, "sockets: flush_write_buffer wanted %d, got %d",
                  maxwrite, nwritten);
        if (nwritten > 0) {
            struct wmapinfo *info;
            for (info = s->writemap; info; info = info->next) {
                info->wait -= nwritten;
            }
            s->wptr += nwritten;
            if (s->wptr >= s->wtop)
                s->wptr = s->wbuf;
        }
      check_result:
        if (nwritten < 0 && errno != EAGAIN && errno != EINTR) {
            int errno_save = errno;
            if (errno != ECONNRESET && errno != EPIPE)
                log_perror("sockets: flush_write_buffer(%d)", s->fd);
            do_disconn(s, DISCONN_REMOTE);
            errno = errno_save;
            RETURN_WITH(-1);
        }
        if (nwritten > 0) {
            s->last_write_time = time(NULL);
            s->flags &= ~SF_WARNED;
            s->total_written += nwritten;
        } else {
            nwritten = 0;  /* return value */
        }
    }
    if ((s->flags & SF_CALLBACK) || MORE_TO_WRITE(s)) {
        FD_SET(s->fd, &write_fds);
    } else {
        FD_CLR(s->fd, &write_fds);
        if (s->flags & SF_DISCONNECT) {
            s->flags &= ~SF_DISCONNECT;
            do_disconn(s, DISCONN_LOCAL_RESUME);
            RETURN_WITH(-2);
        } else {
            reclaim_buffer_space_one(s);
        }
    }
    RETURN_WITH(nwritten);
}

/*************************************************************************/

/* Return the amount by which a socket's read or write buffer should be
 * expanded.  Return zero if the per-connection or total network buffer
 * size limits have been reached and the buffer cannot be expanded; in
 * this case, set *errp to 1 if the per-connection buffer size limit was
 * exceeded, 0 if the total buffer size limit was exceeded.
 */

static uint32 resize_how_much(const Socket *s, uint32 current_size, int *errp)
{
    uint32 socktotal = read_buffer_size(s) + write_buffer_size(s);
    int32 more;
    int over = 0, over_total = 0;

    /* Check current size against limits */
    if (bufsize_limit && socktotal >= bufsize_limit)
        over = 1;
    if (total_bufsize_limit && total_bufsize >= total_bufsize_limit)
        over_total = 1;
    if (over || over_total) {
        if (over)
            *errp = 1;
        else
            *errp = 0;
        return 0;
    }

    /* Expand by 10%, rounded up to a multiple of SOCK_MIN_BUFSIZE */
    more = current_size / 10;
    more = (more+SOCK_MIN_BUFSIZE-1) / SOCK_MIN_BUFSIZE * SOCK_MIN_BUFSIZE;

    /* Make sure new size doesn't exceed limits */
    if (bufsize_limit && socktotal + more >= bufsize_limit)
        more = bufsize_limit - socktotal;
    if (total_bufsize_limit && total_bufsize + more >= total_bufsize_limit)
        more = total_bufsize_limit - total_bufsize;
    if (more < 0) {
        more = 0;
        /* This shouldn't be possible; assume it's the total buffer size */
        *errp = 0;
    }

    /* Return it */
    return more;
}


/* Resize a socket's read or write buffer. */

static int resize_rbuf(Socket *s, uint32 size)
{
    ENTER("%p,%u", s, size);
    if (size <= read_buffer_len(s)) {
        log("sockets: BUG: resize_rbuf(%d): size (%d) <= rlen (%d)"
            " (cursize %ld)", s->fd, size, read_buffer_len(s),
            (long)(s->rtop - s->rbuf));
        RETURN_WITH(0);
    }
    RETURN_WITH(resize_buf(&s->rbuf, &s->rptr, &s->rend, &s->rtop, size));
}


static int resize_wbuf(Socket *s, uint32 size)
{
    ENTER("%p,%u", s, size);
    if (size <= write_buffer_len(s)) {
        log("sockets: BUG: resize_wbuf(%d): size (%d) <= wlen (%d)"
            " (cursize %ld)", s->fd, size, write_buffer_len(s),
            (long)(s->wtop - s->wbuf));
        RETURN_WITH(0);
    }
    RETURN_WITH(resize_buf(&s->wbuf, &s->wptr, &s->wend, &s->wtop, size));
}

/* Routine that does the actual resizing.  Assumes that newsize >= current
 * size.  Returns nonzero on success, zero on failure (out of memory). */
static int resize_buf(char **p_buf, char **p_ptr, char **p_end, char **p_top,
                       uint32 newsize)
{
    uint32 size = *p_top - *p_buf;
    char *newbuf;
    uint32 len = 0;

    ENTER_WITH("%d", "%p,%p,%p,%p,%u", p_buf, p_ptr, p_end, p_top, newsize);
    if (newsize <= size)
        RETURN_WITH(1);
    newbuf = malloc(newsize);
    if (!newbuf)
        RETURN_WITH(0);
    total_bufsize -= size;
    total_bufsize += newsize;
    /* Copy old data to new buffer, if any */
    if (*p_end < *p_ptr) {
        len = *p_top - *p_ptr;
        memcpy(newbuf, *p_ptr, len);
        *p_ptr = *p_buf;
    }
    if (*p_end > *p_ptr) {
        memcpy(newbuf+len, *p_ptr, *p_end - *p_ptr);
        len += *p_end - *p_ptr;
    }
    free(*p_buf);
    *p_buf = newbuf;
    *p_ptr = newbuf;
    *p_end = newbuf + len;
    *p_top = newbuf + newsize;
    RETURN_WITH(1);
}

/*************************************************************************/

/* Try to reclaim unused buffer space.  Return 1 if some buffer space was
 * freed, 0 if not.
 */

static int reclaim_buffer_space_one(Socket *s)
{
    uint32 rlen = read_buffer_len(s), wlen = write_buffer_len(s);
    int retval = 0;

    ENTER_WITH("%d", "%p", s);
    if (read_buffer_size(s) > SOCK_MIN_BUFSIZE
     && rlen < read_buffer_size(s) - SOCK_MIN_BUFSIZE
    ) {
        if (rlen < SOCK_MIN_BUFSIZE) {
            rlen = SOCK_MIN_BUFSIZE;
        } else {
            /* Round up to the next multiple of SOCK_MIN_BUFSIZE, leaving
             * at least one byte available */
            rlen += SOCK_MIN_BUFSIZE;
            rlen /= SOCK_MIN_BUFSIZE;
            rlen *= SOCK_MIN_BUFSIZE;
        }
        resize_rbuf(s, rlen);
        retval = 1;
    }
    if (write_buffer_size(s) > SOCK_MIN_BUFSIZE
     && wlen < write_buffer_size(s) - SOCK_MIN_BUFSIZE
    ) {
        if (wlen < SOCK_MIN_BUFSIZE) {
            wlen = SOCK_MIN_BUFSIZE;
        } else {
            wlen += SOCK_MIN_BUFSIZE;
            wlen /= SOCK_MIN_BUFSIZE;
            wlen *= SOCK_MIN_BUFSIZE;
        }
        resize_wbuf(s, wlen);
        retval = 1;
    }
    RETURN_WITH(retval);
}


static int reclaim_buffer_space(void)
{
    Socket *s;
    int retval = 0;

    ENTER_WITH("%d", "");
    LIST_FOREACH (s, allsockets) {
        retval |= reclaim_buffer_space_one(s);
    }
    RETURN_WITH(retval);
}

/*************************************************************************/

/* Free the first map on the socket's writemap list. */

static void next_wmap(Socket *s)
{
    struct wmapinfo *next;

    ENTER("%p", s);
    if (!s || !s->writemap) {
        log("sockets: BUG: next_wmap() with NULL %s!",
            s ? "writemap" : "socket");
        RETURN;
    }
    if (s->writemap->map) {
#ifdef DISABLE_SWRITEMAP
        log("sockets: BUG: DISABLE_SWRITEMAP but non-NULL map %p for socket"
            " %p", s->writemap->map, s);
#else
        munmap((void *)s->writemap->map, s->writemap->maplen);
#endif
    }
    next = s->writemap->next;
    free(s->writemap);
    if (next) {
        s->writemap = next;
    } else {
        s->writemap = s->writemap_tail = NULL;
    }
    RETURN;
}

/*************************************************************************/

/* Write data to a socket with buffering. */

static int buffered_write(Socket *s, const char *buf, int len)
{
    int nwritten, left = len;
    int errno_save = errno;

    ENTER_WITH("%d", "%p,%p,%d", s, buf, len);
    if (s->fd < 0) {
        errno = EBADF;
        RETURN_WITH(-1);
    }

    /* Reset the write timeout if the buffer is currently empty and we're
     * not busy writing a mapped buffer. */
    if (!MORE_TO_WRITE(s))
        s->last_write_time = time(NULL);

    while (left > 0) {

        /* Fill up to the current buffer size. */
        if (write_buffer_len(s) < write_buffer_size(s)-1) {
            int maxwrite;
            /* If buffer is empty, reset pointers to beginning for efficiency*/
            if (write_buffer_len(s) == 0)
                s->wptr = s->wend = s->wbuf;
            if (s->wptr == s->wbuf) {
                /* Buffer not wrapped */
                maxwrite = s->wtop - s->wend - 1;
            } else {
                /* Buffer is wrapped.  If this write would reach to or past
                 * the end of the buffer, write it first and reset the end
                 * pointer to the beginning of the buffer. */
                if (s->wend+left >= s->wtop && s->wptr <= s->wend) {
                    nwritten = s->wtop - s->wend;
                    memcpy(s->wend, buf, nwritten);
                    buf += nwritten;
                    left -= nwritten;
                    s->wend = s->wbuf;
                }
                /* Now we can copy a single chunk to wend. */
                if (s->wptr > s->wend)
                    maxwrite = s->wptr - s->wend - 1;
                else
                    maxwrite = left;  /* guaranteed to fit from above code */
            }
            if (left > maxwrite)
                nwritten = maxwrite;
            else
                nwritten = left;
            if (nwritten) {
                memcpy(s->wend, buf, nwritten);
                buf += nwritten;
                left -= nwritten;
                s->wend += nwritten;
            }
        }

        /* Now write to the socket as much as we can. */
        if (flush_write_buffer(s) < 0)
            RETURN_WITH(len - left);  /* socket is not valid any more */
        errno_save = errno;
        if (write_buffer_len(s) >= write_buffer_size(s)-1) {
            /* Write failed on full buffer; try to expand the buffer. */
            int over;
            uint32 more = resize_how_much(s, write_buffer_size(s), &over);
            if (more) {
                if (!resize_wbuf(s, write_buffer_size(s) + more)) {
                    /* It would be proper to block here, but if we ran out
                     * of memory the program's probably about to abort
                     * anyway, so just error out */
                    log("sockets: attempt to expand socket %d read buffer"
                        " failed (out of memory?)", s->fd);
                    errno_save = EAGAIN;
                    break;
                }
            } else {
                if ((s->flags & SF_BLOCKING) && !(s->flags & SF_WTRIGGER)) {
                    fd_set fds;
                    FD_ZERO(&fds);
                    FD_SET(s->fd, &fds);
                    if (select(s->fd+1, NULL, &fds, NULL, NULL) < 0) {
                        log("sockets: waiting on blocking socket %d: %s",
                            s->fd, strerror(errno));
                        break;
                    }
                    continue;  /* don't expand the buffer, since it's at max */
                } else {
#ifdef WARN_ON_BUFSIZE
                    if (!(s->flags & SF_WARNED)) {
                        log("sockets: socket %d exceeded %s buffer size"
                            " limit (%d)", s->fd,
                            over ? "per-connection" : "total",
                            over ? bufsize_limit : total_bufsize_limit);
                        s->flags |= SF_WARNED;
                    }
#endif
                    errno_save = EAGAIN;
                    break;
                }
            }
        }

    } /* while (left > 0) */

    errno = errno_save;
    RETURN_WITH(len - left);
}

/*************************************************************************/

/* Internal version of disconn(), used to pass a specific code to the
 * disconnect callback.  If code == DISCONN_LOCAL, attempt to first write
 * out any data left in the write buffer, and delay disconnection if we
 * can't.
 */

static int do_disconn(Socket *s, void *code)
{
    int errno_save = errno;  /* for passing to the callback */

    ENTER_WITH("%d", "%p,%p", s, code);
    if (s == NULL || (s->flags & SF_LISTENER)) {
        if (s)
            log("sockets: BUG: do_disconn(%d) with listener socket (%d)!",
                (int)(long)code, s->fd);
        else
            log("sockets: BUG: do_disconn(%d) with NULL socket!",
                (int)(long)code);
        errno = EINVAL;
        RETURN_WITH(-1);
    }
    if ((s->flags & SF_DISCONNECTING) && !((int)code & DISCONN_RESUME_FLAG))
        RETURN_WITH(0);
    if ((s->flags & SF_DISCONN_REQ) && code == DISCONN_LOCAL)
        RETURN_WITH(0);
    if (!(s->flags & (SF_CONNECTING | SF_CONNECTED)))
        RETURN_WITH(0);
    s->flags |= SF_DISCONN_REQ;
    FD_CLR(s->fd, &sock_fds);
    if (code == DISCONN_LOCAL && MORE_TO_WRITE(s)) {
        /* Write out any buffered data */
        if (flush_write_buffer(s) >= 0 && MORE_TO_WRITE(s)) {
            /* Some data is still buffered; request disconnect after it
             * goes out */
            s->flags |= SF_DISCONNECT;
            /* It's not technically disconnected yet, but it will (should)
             * succeed eventually */
            RETURN_WITH(0);
        }
    }
    s->flags |= SF_DISCONNECTING;
    if (code == DISCONN_REMOTE && (s->flags & SF_CALLBACK)) {
        /* Socket was closed while a callback was doing something; wait
         * until the callback finishes to actually close the socket */
        s->flags |= SF_BROKEN;
        RETURN_WITH(0);
    }
    shutdown(s->fd, 2);
    sock_closefd(s);
    while (s->writemap)
        next_wmap(s);
    s->writemap_tail = NULL;
    if (s->cb_disconn) {
        /* The disconnect callback doesn't need to check for disconnection,
         * so we just call it directly */
        errno = errno_save;
        s->cb_disconn(s, (void *)((int)code & ~DISCONN_RESUME_FLAG));
    }
    s->flags &= ~SF_DISCONNECTING;
    if (s->fd >= 0) {
        /* The socket was reconnected */
        return 0;
    }
    s->flags &= ~(SF_CONNECTING | SF_CONNECTED);
    if (s->flags & (SF_SELFCREATED | SF_DELETEME)) {
        if (s->flags & SF_CALLBACK)
            s->flags |= SF_DELETEME;
        else
            sock_free(s);
    } else {
        reclaim_buffer_space_one(s);
    }
    RETURN_WITH(0);
}

/*************************************************************************/

/* Close a socket's file descriptor, and clear it from all associated
 * structures (s->fd, sockets[], sock_fds, write_fds).
 */

static void sock_closefd(Socket *s)
{
    ENTER("%p", s);
    /* FIXME: apparently it's possible to come here twice for the same
     * connection; under what circumstances can that happen? */
    if (s->fd >= 0) {
        close(s->fd);
        FD_CLR(s->fd, &sock_fds);
        FD_CLR(s->fd, &write_fds);
        sockets[s->fd] = NULL;
        s->fd = -1;
    }
    RETURN;
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
