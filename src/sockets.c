/* Buffered, callback-driven sockets on top of the event engine.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The public interface and its semantics are documented in sockets.h.
 * This file owns the per-socket state (buffers, callbacks, connection
 * state machine); waiting for readiness is delegated to an Engine
 * (engine.h), picked once at start-up like ircu does.
 *
 * Lifetime rule: a Socket is never freed while one of its callbacks is
 * running.  sock_free() or a disconnection inside a callback only marks it
 * (SF_DELETEME), and run_callback() frees it once the callback returns.
 * Every code path that runs a callback therefore re-checks the socket
 * through run_callback()'s return value before touching it again.
 */

#include "engine.h"
#include "services.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>

/*************************************************************************/
/***************************** Data types ********************************/
/*************************************************************************/

/* A growable byte buffer; the data lives in data[start .. start+len). */
typedef struct {
    char* data;
    uint32 start, len, size;
} Buffer;

/* A pending write trigger: fires once `offset' bytes (counted over the
 * lifetime of the socket) have been sent. */
typedef struct trigger_ {
    struct trigger_* next;
    uint64 offset;
    void* param;
} Trigger;

struct socket_ {
    Socket *next, *prev;
    int fd;
    int flags;             /* SF_* */
    unsigned int interest; /* ENGINE_* registered with engine */
    struct sockaddr_storage remote;
    socklen_t remote_len;

    SocketCallback cb[SCB_TRIGGER + 1]; /* Indexed by SocketCallbackID */
    int cb_depth;                       /* Callbacks currently running */

    int write_timeout; /* Seconds, 0 = none */
    time_t last_write_time;

    Buffer rbuf, wbuf;
    Trigger *triggers, *triggers_tail;

    uint64 total_read;
    uint64 total_written;
};

#define SF_SELFCREATED 0x0001 /* Created by accept(); freed on disconnect */
#define SF_BLOCKING    0x0002 /* Writes wait for buffer space */
#define SF_LISTENER    0x0004 /* Listening socket */
#define SF_CONNECTING  0x0008 /* connect() in progress */
#define SF_CONNECTED   0x0010 /* Connection established */
#define SF_MUTE        0x0020 /* Not reading */
#define SF_UNMUTED     0x0040 /* Just unmuted; deliver buffered data */
#define SF_DISCONNECT  0x0080 /* Close once the write buffer drains */
#define SF_DELETEME    0x0100 /* Free once callbacks return */

#define IS_OPEN(s) ((s)->flags & (SF_CONNECTING | SF_CONNECTED))

/*************************************************************************/
/*************************** Global state ********************************/
/*************************************************************************/

static const Engine* const engine_list[] = {
#if ENGINE_EPOLL
    &engine_epoll,
#endif
#if ENGINE_KQUEUE
    &engine_kqueue,
#endif
#if ENGINE_POLL
    &engine_poll,
#endif
    &engine_select,
};
static const Engine* engine; /* Chosen on first use */

static Socket* allsockets; /* Every Socket, open or not */
static Socket** fd_table;  /* Open sockets, indexed by descriptor */
static int fd_table_size;

static uint32 total_bufsize; /* All buffers, in bytes */
static uint32 bufsize_limit, total_bufsize_limit;
static int read_timeout = -1; /* check_sockets(), msec */

/*************************************************************************/

static int do_disconn(Socket* s, void* code);

/*************************************************************************/
/*************************** Event engine ********************************/
/*************************************************************************/

static const Engine* get_engine(void)
{
    int i;

    if (engine)
        return engine;
    for (i = 0; i < lenof(engine_list); i++) {
        if (engine_list[i]->init()) {
            engine = engine_list[i];
            log_debug(1, "sockets: using the %s event engine", engine->name);
            return engine;
        }
    }
    fatal("sockets: no event engine could be initialized");
    return NULL; /* not reached */
}

const char* sock_engine_name(void)
{
    return get_engine()->name;
}

/* Tell the engine what we want to hear about `s'. */
static void set_interest(Socket* s, unsigned int interest)
{
    if (s->fd < 0 || s->interest == interest)
        return;
    if (get_engine()->update(s->fd, interest) < 0)
        log_perror("sockets: engine update(%d)", s->fd);
    s->interest = interest;
}

/* Recompute the interest set from the socket state. */
static void update_interest(Socket* s)
{
    unsigned int interest = 0;

    if (s->flags & SF_LISTENER) {
        interest = (s->flags & SF_MUTE) ? 0 : ENGINE_READ;
    }
    else if (s->flags & SF_CONNECTING) {
        interest = ENGINE_WRITE;
    }
    else if (s->flags & SF_CONNECTED) {
        if (!(s->flags & (SF_MUTE | SF_DISCONNECT)))
            interest |= ENGINE_READ;
        if (s->wbuf.len > 0)
            interest |= ENGINE_WRITE;
    }
    set_interest(s, interest);
}

/* Attach an open descriptor to `s' and register it with the engine. */
static int attach_fd(Socket* s, int fd, unsigned int interest)
{
    if (fd >= fd_table_size) {
        int newsize = fd + 64, i;
        Socket** t = realloc(fd_table, newsize * sizeof(*t));
        if (!t)
            return -1;
        for (i = fd_table_size; i < newsize; i++)
            t[i] = NULL;
        fd_table = t;
        fd_table_size = newsize;
    }
    if (get_engine()->add(fd, interest) < 0)
        return -1;
    fd_table[fd] = s;
    s->fd = fd;
    s->interest = interest;
    return 0;
}

/* Detach and close the descriptor of `s', if it has one. */
static void close_fd(Socket* s)
{
    if (s->fd < 0)
        return;
    get_engine()->remove(s->fd);
    fd_table[s->fd] = NULL;
    close(s->fd);
    s->fd = -1;
    s->interest = 0;
}

static int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        return -1;
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    return 0;
}

/*************************************************************************/
/****************************** Buffers **********************************/
/*************************************************************************/

static int buffer_init(Buffer* b)
{
    b->data = malloc(SOCK_MIN_BUFSIZE);
    if (!b->data)
        return 0;
    b->start = b->len = 0;
    b->size = SOCK_MIN_BUFSIZE;
    total_bufsize += b->size;
    return 1;
}

static void buffer_free(Buffer* b)
{
    total_bufsize -= b->size;
    free(b->data);
    b->data = NULL;
    b->start = b->len = b->size = 0;
}

static void buffer_consume(Buffer* b, uint32 n)
{
    if (n >= b->len) {
        b->start = b->len = 0;
    }
    else {
        b->start += n;
        b->len -= n;
    }
}

/* Reallocate `b' to exactly `newsize' bytes (>= b->len), compacting it. */
static int buffer_resize(Buffer* b, uint32 newsize)
{
    char* p;

    if (b->start > 0 && b->len > 0)
        memmove(b->data, b->data + b->start, b->len);
    b->start = 0;
    if (newsize == b->size)
        return 1;
    p = realloc(b->data, newsize);
    if (!p)
        return 0;
    total_bufsize += newsize - b->size;
    b->data = p;
    b->size = newsize;
    return 1;
}

/* Make room for at least `want' more bytes at the end of `b', within the
 * buffer limits.  Return the number of bytes of free space available
 * afterwards (which may be less than `want', even zero). */
static uint32 buffer_reserve(const Socket* s, Buffer* b, uint32 want)
{
    uint32 room = b->size - b->start - b->len, need, grow, socktotal;

    if (room >= want)
        return room;
    if (b->start > 0) { /* Reclaim the consumed prefix first */
        buffer_resize(b, b->size);
        room = b->size - b->len;
        if (room >= want)
            return room;
    }

    /* Grow by at least SOCK_MIN_BUFSIZE and at least 10%. */
    need = want - room;
    grow = b->size / 10;
    if (grow < need)
        grow = need;
    grow = (grow + SOCK_MIN_BUFSIZE - 1) / SOCK_MIN_BUFSIZE * SOCK_MIN_BUFSIZE;

    socktotal = s->rbuf.size + s->wbuf.size;
    if (bufsize_limit && socktotal + grow > bufsize_limit)
        grow = bufsize_limit > socktotal ? bufsize_limit - socktotal : 0;
    if (total_bufsize_limit && total_bufsize + grow > total_bufsize_limit)
        grow = total_bufsize_limit > total_bufsize
                   ? total_bufsize_limit - total_bufsize
                   : 0;
    if (grow > 0 && !buffer_resize(b, b->size + grow))
        log("sockets: out of memory expanding buffer of socket %d", s->fd);
    return b->size - b->start - b->len;
}

/* Give back memory from a buffer that is mostly empty. */
static void buffer_shrink(Buffer* b)
{
    uint32 target;

    if (b->size <= SOCK_MIN_BUFSIZE || b->len >= b->size - SOCK_MIN_BUFSIZE)
        return;
    target = (b->len / SOCK_MIN_BUFSIZE + 1) * SOCK_MIN_BUFSIZE;
    buffer_resize(b, target);
}

static void reclaim_buffer_space(void)
{
    Socket* s;

    LIST_FOREACH(s, allsockets)
    {
        if (s->rbuf.data)
            buffer_shrink(&s->rbuf);
        if (s->wbuf.data)
            buffer_shrink(&s->wbuf);
    }
}

/*************************************************************************/
/***************************** Callbacks *********************************/
/*************************************************************************/

/* Run one callback of `s'.  Return nonzero if the socket is still usable
 * afterwards, zero if it was freed or closed meanwhile. */
static int run_callback(Socket* s, SocketCallbackID which, void* param)
{
    SocketCallback cb = s->cb[which];

    if (!cb)
        return 1;
    s->cb_depth++;
    cb(s, param);
    s->cb_depth--;
    if (s->cb_depth == 0 && (s->flags & SF_DELETEME)) {
        sock_free(s);
        return 0;
    }
    return s->fd >= 0;
}

/* Fire the write triggers whose data has all been sent. */
static int fire_triggers(Socket* s)
{
    while (s->triggers && s->triggers->offset <= s->total_written) {
        Trigger* t = s->triggers;
        void* param = t->param;

        s->triggers = t->next;
        if (!s->triggers)
            s->triggers_tail = NULL;
        free(t);
        if (!run_callback(s, SCB_TRIGGER, param))
            return 0;
    }
    return 1;
}

static void free_triggers(Socket* s)
{
    while (s->triggers) {
        Trigger* t = s->triggers;
        s->triggers = t->next;
        free(t);
    }
    s->triggers_tail = NULL;
}

/*************************************************************************/
/************************** Reading / writing ****************************/
/*************************************************************************/

/* Read whatever the kernel has for `s'.  Return the number of bytes read,
 * 0 if the buffer is full or nothing was pending, -1 on EOF or error. */
static int fill_read_buffer(Socket* s)
{
    int total = 0;

    for (;;) {
        uint32 room = buffer_reserve(s, &s->rbuf, SOCK_MIN_BUFSIZE);
        ssize_t n;

        if (room == 0)
            return total; /* Limit reached; retry once consumed */
        n = recv(s->fd, s->rbuf.data + s->rbuf.start + s->rbuf.len, room, 0);
        if (n > 0) {
            s->rbuf.len += n;
            s->total_read += n;
            total += n;
            if ((uint32)n < room)
                return total;
            continue;
        }
        if (n == 0) {
            errno = 0; /* Orderly shutdown by the remote side */
            return -1;
        }
        if (errno == EINTR)
            continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return total;
        return -1;
    }
}

/* Send as much buffered data as the kernel takes, stopping at the next
 * pending write trigger.  Return 0 on success (even if data remains),
 * -1 if the connection failed (the socket has been disconnected). */
static int flush_write_buffer(Socket* s)
{
    while (s->wbuf.len > 0) {
        uint32 len = s->wbuf.len;
        ssize_t n;

        if (s->triggers && s->triggers->offset - s->total_written < len)
            len = (uint32)(s->triggers->offset - s->total_written);
        if (len == 0)
            break; /* Trigger due; check_sockets() fires it */
        n = send(s->fd, s->wbuf.data + s->wbuf.start, len, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            if (errno != ECONNRESET && errno != EPIPE)
                log_perror("sockets: send(%d)", s->fd);
            do_disconn(s, DISCONN_REMOTE);
            return -1;
        }
        buffer_consume(&s->wbuf, n);
        s->total_written += n;
        s->last_write_time = time(NULL);
    }

    if (s->wbuf.len == 0 && (s->flags & SF_DISCONNECT)) {
        s->flags &= ~SF_DISCONNECT;
        do_disconn(s, DISCONN_LOCAL);
        return -1;
    }
    update_interest(s);
    return 0;
}

/* Wait until `s' can take more data (blocking sockets only). */
static int wait_writable(Socket* s)
{
    struct pollfd pfd;

    pfd.fd = s->fd;
    pfd.events = POLLOUT;
    pfd.revents = 0;
    while (poll(&pfd, 1, -1) < 0) {
        if (errno != EINTR) {
            log_perror("sockets: waiting on blocking socket %d", s->fd);
            return -1;
        }
    }
    return 0;
}

/* Append data to the write buffer and push out what the kernel accepts.
 * Return the number of bytes accepted, or -1 on error. */
static int32 buffered_write(Socket* s, const char* buf, int32 len)
{
    int32 done = 0;

    if (s->fd < 0 || !IS_OPEN(s)) {
        errno = ENOTCONN;
        return -1;
    }
    if (s->wbuf.len == 0)
        s->last_write_time = time(NULL);

    while (done < len) {
        uint32 room = buffer_reserve(s, &s->wbuf, len - done);
        uint32 chunk =
            (uint32)(len - done) < room ? (uint32)(len - done) : room;

        if (chunk > 0) {
            memcpy(s->wbuf.data + s->wbuf.start + s->wbuf.len, buf + done,
                   chunk);
            s->wbuf.len += chunk;
            done += chunk;
        }
        if (!(s->flags & SF_CONNECTED))
            break; /* Sent once the connection completes */
        if (flush_write_buffer(s) < 0)
            return done > 0 ? done : -1;
        if (chunk == 0) {
            /* Buffer limit reached. */
            if (!(s->flags & SF_BLOCKING) || wait_writable(s) < 0) {
                errno = EAGAIN;
                break;
            }
        }
    }
    update_interest(s);
    return done;
}

/*************************************************************************/
/*************************** Connection state ****************************/
/*************************************************************************/

/* Close a connection and run the disconnect callback.  With DISCONN_LOCAL
 * and data still buffered, the close is deferred until it has been sent. */
static int do_disconn(Socket* s, void* code)
{
    int errno_save = errno;

    if (!IS_OPEN(s))
        return 0;
    if (code == DISCONN_LOCAL && s->wbuf.len > 0 &&
        (s->flags & SF_CONNECTED) && !(s->flags & SF_DISCONNECT)) {
        s->flags |= SF_DISCONNECT;
        if (flush_write_buffer(s) < 0 || !IS_OPEN(s))
            return 0; /* Flushed and closed (or failed) already */
        if (s->flags & SF_DISCONNECT) {
            update_interest(s);
            return 0; /* Rest goes out from check_sockets() */
        }
    }

    s->flags &= ~(SF_CONNECTING | SF_CONNECTED | SF_DISCONNECT | SF_UNMUTED);
    shutdown(s->fd, SHUT_RDWR);
    close_fd(s);
    free_triggers(s);
    buffer_consume(&s->rbuf, s->rbuf.len);
    buffer_consume(&s->wbuf, s->wbuf.len);

    errno = errno_save;
    if (s->cb[SCB_DISCONNECT]) {
        s->cb_depth++;
        s->cb[SCB_DISCONNECT](s, code);
        s->cb_depth--;
    }
    if (s->fd >= 0)
        return 0; /* Reconnected from the callback */

    if (s->flags & (SF_SELFCREATED | SF_DELETEME)) {
        if (s->cb_depth > 0)
            s->flags |= SF_DELETEME;
        else
            sock_free(s);
    }
    else {
        buffer_shrink(&s->rbuf);
        buffer_shrink(&s->wbuf);
    }
    return 0;
}

/* Accept every pending connection on listener `s'. */
static void do_accept(Socket* s)
{
    for (;;) {
        struct sockaddr_storage sa;
        socklen_t salen = sizeof(sa);
        Socket* news;
        int fd;

        fd = accept(s->fd, (struct sockaddr*)&sa, &salen);
        if (fd < 0) {
            if (errno == EINTR)
                continue;
            if (errno != EAGAIN && errno != EWOULDBLOCK &&
                errno != ECONNABORTED)
                log_perror("sockets: accept(%d)", s->fd);
            return;
        }
        if (!s->cb[SCB_ACCEPT] || set_nonblocking(fd) < 0 ||
            !(news = sock_new())) {
            close(fd);
            continue;
        }
        if (attach_fd(news, fd, ENGINE_READ) < 0) {
            log_perror("sockets: accept(%d): cannot watch fd %d", s->fd, fd);
            close(fd);
            sock_free(news);
            continue;
        }
        news->flags |= SF_SELFCREATED | SF_CONNECTED;
        memcpy(&news->remote, &sa, salen);
        news->remote_len = salen;
        if (!run_callback(s, SCB_ACCEPT, news))
            return; /* Listener closed by the callback */
    }
}

/* Hand buffered input to the read callbacks, until they stop consuming
 * it or the socket goes away.  Return zero if the socket is gone. */
static int deliver_input(Socket* s)
{
    uint32 left = s->rbuf.len, newleft;

    while (left > 0) {
        if (!run_callback(s, SCB_READ, (void*)(uintptr_t)left))
            return 0;
        if (s->flags & SF_MUTE)
            break;
        newleft = s->rbuf.len;
        if (s->cb[SCB_READLINE] && newleft > 0 &&
            memchr(s->rbuf.data + s->rbuf.start, '\n', newleft)) {
            if (!run_callback(s, SCB_READLINE, (void*)(uintptr_t)newleft))
                return 0;
            if (s->flags & SF_MUTE)
                break;
            newleft = s->rbuf.len;
        }
        if (newleft == left)
            break; /* Nobody took anything */
        left = newleft;
    }
    buffer_shrink(&s->rbuf);
    return 1;
}

/* A connect() in progress finished, one way or the other. */
static void finish_connect(Socket* s)
{
    int err = 0;
    socklen_t len = sizeof(err);

    if (getsockopt(s->fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
        err = errno;
    if (err) {
        errno = err;
        log_perror_debug(1, "sockets: connect(%d)", s->fd);
        do_disconn(s, DISCONN_CONNFAIL);
        return;
    }
    s->flags = (s->flags & ~SF_CONNECTING) | SF_CONNECTED;
    update_interest(s);
    if (!run_callback(s, SCB_CONNECT, NULL))
        return;
    flush_write_buffer(s); /* Anything written while connecting */
}

/* Engine callback: `fd' has `events' pending. */
static void handle_events(int fd, unsigned int events)
{
    Socket* s = (fd >= 0 && fd < fd_table_size) ? fd_table[fd] : NULL;
    int res;

    if (!s)
        return;

    if (s->flags & SF_LISTENER) {
        if (events & (ENGINE_READ | ENGINE_ERROR))
            do_accept(s);
        return;
    }
    if (s->flags & SF_CONNECTING) {
        if (events & (ENGINE_WRITE | ENGINE_ERROR))
            finish_connect(s);
        return;
    }
    if (!(s->flags & SF_CONNECTED))
        return;

    if (events & ENGINE_WRITE) {
        if (flush_write_buffer(s) < 0 || s->fd != fd)
            return;
        if (!fire_triggers(s))
            return;
    }
    if (events & (ENGINE_READ | ENGINE_ERROR)) {
        if (s->flags & SF_MUTE) {
            /* A muted socket is not read, but a hang-up is reported
             * regardless of interest and would otherwise spin. */
            if (events & ENGINE_ERROR)
                do_disconn(s, DISCONN_REMOTE);
            return;
        }
        res = fill_read_buffer(s);
        if (s->rbuf.len > 0 && !deliver_input(s))
            return;
        if (res < 0 && IS_OPEN(s)) {
            if (errno)
                log_perror_debug(1, "sockets: recv(%d)", fd);
            do_disconn(s, DISCONN_REMOTE);
        }
    }
}

/*************************************************************************/
/************************** Global routines ******************************/
/*************************************************************************/

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

void sock_set_rto(int msec)
{
    read_timeout = msec;
}

/*************************************************************************/

Socket* sock_new(void)
{
    Socket* s;

    if (total_bufsize_limit &&
        total_bufsize + 2 * SOCK_MIN_BUFSIZE > total_bufsize_limit) {
        reclaim_buffer_space();
        if (total_bufsize + 2 * SOCK_MIN_BUFSIZE > total_bufsize_limit) {
            log("sockets: sock_new(): out of buffer space (%lu of %lu)",
                (unsigned long)total_bufsize,
                (unsigned long)total_bufsize_limit);
            errno = ENOBUFS;
            return NULL;
        }
    }

    s = calloc(1, sizeof(*s));
    if (!s)
        return NULL;
    if (!buffer_init(&s->rbuf) || !buffer_init(&s->wbuf)) {
        int errno_save = errno;
        if (s->rbuf.data)
            buffer_free(&s->rbuf);
        free(s);
        errno = errno_save;
        return NULL;
    }
    s->fd = -1;
    s->last_write_time = time(NULL);
    LIST_INSERT(s, allsockets);
    return s;
}

void sock_free(Socket* s)
{
    if (!s) {
        log("sockets: sock_free() with NULL socket!");
        return;
    }
    if (s->cb_depth > 0) {
        /* Inside one of its own callbacks: finish once it returns. */
        s->flags |= SF_DELETEME;
        if (IS_OPEN(s))
            do_disconn(s, DISCONN_LOCAL);
        return;
    }
    if (IS_OPEN(s)) {
        /* Close first; do_disconn() frees the socket (SF_DELETEME) as
         * soon as the connection is actually down, which may be after
         * the write buffer drains. */
        s->flags |= SF_DELETEME;
        do_disconn(s, DISCONN_LOCAL);
        return;
    }
    if (s->flags & SF_LISTENER)
        close_listener(s);

    LIST_REMOVE(s, allsockets);
    free_triggers(s);
    if (s->rbuf.data)
        buffer_free(&s->rbuf);
    if (s->wbuf.data)
        buffer_free(&s->wbuf);
    free(s);

    if (!allsockets) {
        free(fd_table);
        fd_table = NULL;
        fd_table_size = 0;
    }
}

void sock_setcb(Socket* s, SocketCallbackID which, SocketCallback func)
{
    if (!s || which < SCB_CONNECT || which > SCB_TRIGGER) {
        log("sockets: sock_setcb(): invalid %s", !s ? "socket" : "callback");
        errno = EINVAL;
        return;
    }
    s->cb[which] = func;
}

int sock_isconn(const Socket* s)
{
    return s && (s->flags & SF_CONNECTED) ? 1 : 0;
}

int sock_remote(const Socket* s, struct sockaddr* sa, int* lenptr)
{
    if (!s || !sa || !lenptr || !(s->flags & SF_CONNECTED)) {
        errno = EINVAL;
        return -1;
    }
    memcpy(sa, &s->remote,
           (int)s->remote_len < *lenptr ? (int)s->remote_len : *lenptr);
    *lenptr = s->remote_len;
    return 0;
}

void sock_set_blocking(Socket* s, int blocking)
{
    if (!s)
        return;
    if (blocking)
        s->flags |= SF_BLOCKING;
    else
        s->flags &= ~SF_BLOCKING;
}

int sock_get_blocking(const Socket* s)
{
    if (!s) {
        errno = EINVAL;
        return -1;
    }
    return (s->flags & SF_BLOCKING) ? 1 : 0;
}

void sock_set_wto(Socket* s, int seconds)
{
    if (!s || seconds < 0) {
        errno = EINVAL;
        return;
    }
    s->write_timeout = seconds;
}

void sock_mute(Socket* s)
{
    if (!s || (s->flags & SF_MUTE))
        return;
    s->flags |= SF_MUTE;
    s->flags &= ~SF_UNMUTED;
    update_interest(s);
}

void sock_unmute(Socket* s)
{
    if (!s || !(s->flags & SF_MUTE))
        return;
    s->flags &= ~SF_MUTE;
    s->flags |= SF_UNMUTED;
    update_interest(s);
}

uint32 read_buffer_len(const Socket* s)
{
    return s->rbuf.len;
}

uint32 write_buffer_len(const Socket* s)
{
    return s->wbuf.len;
}

int sock_rwstat(const Socket* s, uint64* read_ret, uint64* written_ret)
{
    if (!s) {
        errno = EINVAL;
        return -1;
    }
    if (read_ret)
        *read_ret = s->total_read;
    if (written_ret)
        *written_ret = s->total_written;
    return 0;
}

/* Percentage of `used' over `limit', rounded up; 0 when unlimited. */
static int percent_of(uint32 used, uint32 limit)
{
    if (!limit)
        return 0;
    return (int)(((uint64)used * 100 + limit - 1) / limit);
}

int sock_bufstat(const Socket* s, uint32* socksize_ret, uint32* totalsize_ret,
                 int* ratio1_ret, int* ratio2_ret)
{
    uint32 socksize = s ? s->rbuf.size + s->wbuf.size : 0;
    int ratio1 = s ? percent_of(socksize, bufsize_limit) : 0;
    int ratio2 = percent_of(total_bufsize, total_bufsize_limit);

    if (socksize_ret && s)
        *socksize_ret = socksize;
    if (totalsize_ret)
        *totalsize_ret = total_bufsize;
    if (ratio1_ret)
        *ratio1_ret = ratio1;
    if (ratio2_ret)
        *ratio2_ret = ratio2;
    return ratio1 > ratio2 ? ratio1 : ratio2;
}

/*************************************************************************/

/* Wait for activity on any socket (up to the read timeout, or the nearest
 * write timeout) and run the callbacks for whatever happened. */
void check_sockets(void)
{
    const Engine* eng = get_engine();
    time_t now = time(NULL);
    int timeout = read_timeout, fd;
    Socket* s;

    LIST_FOREACH(s, allsockets)
    {
        if (s->fd >= 0 && s->write_timeout && s->wbuf.len > 0) {
            time_t left = s->last_write_time + s->write_timeout - now;
            int ms = left > 0 ? (int)left * 1000 : 0;
            if (timeout < 0 || ms < timeout)
                timeout = ms;
        }
        /* Unmuted sockets with buffered input must not wait. */
        if ((s->flags & SF_UNMUTED) && s->rbuf.len > 0)
            timeout = 0;
    }

    enable_signals();
    if (eng->wait(timeout, handle_events) < 0)
        log_perror("sockets: %s engine wait", eng->name);
    disable_signals();

    /* Housekeeping on open sockets.  Walk the descriptor table rather
     * than the socket list: a callback may free any socket, and the table
     * is re-read on every step. */
    now = time(NULL);
    for (fd = 0; fd < fd_table_size; fd++) {
        if (!(s = fd_table[fd]))
            continue;
        if ((s->flags & SF_UNMUTED) && (s->flags & SF_CONNECTED)) {
            s->flags &= ~SF_UNMUTED;
            if (s->rbuf.len > 0 && !deliver_input(s))
                continue;
        }
        if (s->fd >= 0 && (s->flags & SF_CONNECTED) && s->triggers &&
            !fire_triggers(s))
            continue;
        if (s->fd >= 0 && s->write_timeout && s->wbuf.len > 0 &&
            s->last_write_time + s->write_timeout <= now) {
            log_debug(1, "sockets: write timeout on socket %d", s->fd);
            s->flags &= ~SF_DISCONNECT;
            do_disconn(s, DISCONN_REMOTE);
        }
    }
}

/*************************************************************************/

/* Resolve `host'/`port' into `ai' (caller frees).  Return 0 or -1. */
static int resolve(const char* host, int port, int passive,
                   struct addrinfo** ai)
{
    struct addrinfo hints;
    char portbuf[8];
    int err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = passive ? AI_PASSIVE : 0;
    snprintf(portbuf, sizeof(portbuf), "%d", port);
    err = getaddrinfo(host, port ? portbuf : NULL, &hints, ai);
    if (err) {
        log("sockets: cannot resolve %s: %s", host ? host : "*",
            gai_strerror(err));
        errno = (err == EAI_SYSTEM) ? errno : EADDRNOTAVAIL;
        return -1;
    }
    return 0;
}

int conn(Socket* s, const char* host, int port, const char* lhost, int lport)
{
    struct addrinfo *ai, *local = NULL;
    int fd = -1, res, errno_save;

    if (!s || !host || port <= 0 || port > 65535) {
        log("sockets: conn() with %s", !s      ? "NULL socket"
                                       : !host ? "NULL host"
                                               : "bad port number");
        errno = EINVAL;
        return -1;
    }
    if (s->flags & SF_DELETEME) {
        errno = EPERM;
        return -1;
    }
    if (IS_OPEN(s) || (s->flags & SF_LISTENER)) {
        errno = EISCONN;
        return -1;
    }
    if (resolve(host, port, 0, &ai) < 0)
        return -1;
    if ((lhost || lport) && resolve(lhost, lport, 1, &local) < 0) {
        freeaddrinfo(ai);
        return -1;
    }

    fd = socket(ai->ai_family, SOCK_STREAM, 0);
    if (fd < 0)
        goto fail;
    if (set_nonblocking(fd) < 0)
        goto fail;
    if (local && bind(fd, local->ai_addr, local->ai_addrlen) < 0)
        goto fail;
    do {
        res = connect(fd, ai->ai_addr, ai->ai_addrlen);
    } while (res < 0 && errno == EINTR);
    if (res < 0 && errno != EINPROGRESS)
        goto fail;

    memcpy(&s->remote, ai->ai_addr, ai->ai_addrlen);
    s->remote_len = ai->ai_addrlen;
    freeaddrinfo(ai);
    if (local)
        freeaddrinfo(local);

    s->flags &= ~(SF_DISCONNECT | SF_UNMUTED);
    s->flags |= (res == 0) ? SF_CONNECTED : SF_CONNECTING;
    if (attach_fd(s, fd, res == 0 ? ENGINE_READ : ENGINE_WRITE) < 0) {
        errno_save = errno;
        close(fd);
        s->flags &= ~(SF_CONNECTED | SF_CONNECTING);
        errno = errno_save;
        return -1;
    }
    update_interest(s);
    if (res == 0)
        run_callback(s, SCB_CONNECT, NULL);
    return 0;

fail:
    errno_save = errno;
    if (fd >= 0)
        close(fd);
    freeaddrinfo(ai);
    if (local)
        freeaddrinfo(local);
    errno = errno_save;
    return -1;
}

int disconn(Socket* s)
{
    if (!s || (s->flags & SF_LISTENER)) {
        errno = EINVAL;
        return -1;
    }
    return do_disconn(s, DISCONN_LOCAL);
}

int open_listener(Socket* s, const char* host, int port, int backlog)
{
    struct addrinfo* ai;
    int fd = -1, one = 1, errno_save;

    if (!s || port <= 0 || port > 65535 || backlog < 1) {
        errno = EINVAL;
        return -1;
    }
    if (IS_OPEN(s) || (s->flags & SF_LISTENER)) {
        errno = EISCONN;
        return -1;
    }
    if (resolve(host, port, 1, &ai) < 0)
        return -1;

    fd = socket(ai->ai_family, SOCK_STREAM, 0);
    if (fd < 0)
        goto fail;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
        log_perror("sockets: setsockopt(%d, SO_REUSEADDR)", fd);
    if (set_nonblocking(fd) < 0 || bind(fd, ai->ai_addr, ai->ai_addrlen) < 0 ||
        listen(fd, backlog) < 0 ||
        attach_fd(s, fd, (s->flags & SF_MUTE) ? 0 : ENGINE_READ) < 0)
        goto fail;
    freeaddrinfo(ai);
    s->flags |= SF_LISTENER;
    return 0;

fail:
    errno_save = errno;
    if (fd >= 0)
        close(fd);
    freeaddrinfo(ai);
    errno = errno_save;
    return -1;
}

int close_listener(Socket* s)
{
    if (!s || !(s->flags & SF_LISTENER)) {
        errno = EINVAL;
        return -1;
    }
    close_fd(s);
    s->flags &= ~SF_LISTENER;
    return 0;
}

/*************************************************************************/

int32 sread(Socket* s, char* buf, int32 len)
{
    uint32 n;

    if (!s || !buf || len <= 0) {
        errno = EINVAL;
        return -1;
    }
    n = (uint32)len < s->rbuf.len ? (uint32)len : s->rbuf.len;
    memcpy(buf, s->rbuf.data + s->rbuf.start, n);
    buffer_consume(&s->rbuf, n);
    return n;
}

int32 swrite(Socket* s, const char* buf, int32 len)
{
    if (!s || !buf || len < 0) {
        errno = EINVAL;
        return -1;
    }
    return len ? buffered_write(s, buf, len) : 0;
}

int swrite_trigger(Socket* s, void* param)
{
    Trigger* t;

    if (!s) {
        errno = EINVAL;
        return -1;
    }
    t = malloc(sizeof(*t));
    if (!t)
        return -1;
    t->next = NULL;
    t->offset = s->total_written + s->wbuf.len;
    t->param = param;
    if (s->triggers_tail)
        s->triggers_tail->next = t;
    else
        s->triggers = t;
    s->triggers_tail = t;
    return 0;
}

int sgetc(Socket* s)
{
    int c;

    if (!s->rbuf.len)
        return EOF;
    c = (unsigned char)s->rbuf.data[s->rbuf.start];
    buffer_consume(&s->rbuf, 1);
    return c;
}

char* sgets(char* buf, int32 len, Socket* s)
{
    const char *start, *eol;
    uint32 linelen, copy;

    if (!s || !buf || len <= 0) {
        errno = EINVAL;
        return NULL;
    }
    start = s->rbuf.data + s->rbuf.start;
    eol = memchr(start, '\n', s->rbuf.len);
    if (!eol)
        return NULL;
    linelen = (uint32)(eol - start) + 1; /* Including the newline */
    copy = linelen < (uint32)len - 1 ? linelen : (uint32)len - 1;
    memcpy(buf, start, copy);
    buf[copy] = 0;
    buffer_consume(&s->rbuf, linelen); /* The rest of a long line is lost */
    return buf;
}

char* sgets2(char* buf, int32 len, Socket* s)
{
    char* end;

    if (!sgets(buf, len, s))
        return NULL;
    end = buf + strlen(buf);
    if (end > buf && end[-1] == '\n')
        *--end = 0;
    if (end > buf && end[-1] == '\r')
        *--end = 0;
    return buf;
}

int sputs(const char* str, Socket* s)
{
    if (!str || !s) {
        errno = EINVAL;
        return -1;
    }
    return buffered_write(s, str, strlen(str));
}

int sockprintf(Socket* s, const char* fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vsockprintf(s, fmt, args);
    va_end(args);
    return ret;
}

int vsockprintf(Socket* s, const char* fmt, va_list args)
{
    char buf[65536];
    int len;

    if (!s || !fmt) {
        errno = EINVAL;
        return -1;
    }
    len = vsnprintf(buf, sizeof(buf), fmt, args);
    if (len < 0)
        return -1;
    if (len >= (int)sizeof(buf))
        len = sizeof(buf) - 1;
    return buffered_write(s, buf, len);
}
