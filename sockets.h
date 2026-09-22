/* Definitions/declarations for socket utility routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>  /* for struct sockaddr */

/*************************************************************************/

/* Services implements an event-based socket management system using
 * callbacks for socket-related events (connect, disconnect, read).
 * Sockets are created using sock_new() and freed using sock_free();
 * callbacks are registered using sock_setcb(SCB_*,function).  The
 * check_sockets() function does the actual checking for socket activity,
 * and should be called by the program's main loop.
 *
 * By default, sockets are non-blocking; if a socket is not ready for
 * writing and no more buffer space is available, write attempts will stop
 * at that point, setting errno to EAGAIN.  sock_set_blocking() allows
 * sockets to be set to blocking mode, which causes write attempts to wait
 * until either buffer space becomes available or a hard error (such as the
 * connection being broken) occurs.  sock_get_blocking() returns whether
 * the socket is set to blocking mode (nonzero return value, but not -1) or
 * not (zero).
 *
 * Since ordinary writes are buffered, the caller has no easy way to check
 * how much of the data has actually been transferred, or, consequently,
 * whether the remote host is actually accepting data or just sitting
 * around doing nothing.  Since the latter case would cause socket slots to
 * be used up for long periods of time, the sock_set_wto() function allows
 * a write timeout to be set for a socket; if no data is accepted by the
 * remote side within the specified number of seconds, the socket will be
 * considered dead and processed as if the remote host had closed the
 * connection.  The write timeout is disabled by default, but if enabled,
 * it can be disabled again by setting a timeout value of zero.
 *
 * Likewise, the program may wish to perform some periodic actions even if
 * no socket activity is occurring.  The sock_set_rto() function can be
 * used to set a global read timeout in milliseconds; if no new data
 * arrives during this period, check_sockets() will return control to the
 * caller.  sock_set_rto(-1) will revert to the default behavior of waiting
 * indefinitely for socket activity.
 *
 * When a callback function is called, it is passed two parameters: a
 * pointer to the socket structure (Socket *) for the socket on which the
 * event occurred, and a void * parameter whose meaning depends on the
 * particular callback (see below).  Callbacks will never be called nested
 * unless a callback explicitly calls check_sockets(), which is not
 * recommended; thus it is safe to set new callbacks for a socket inside of
 * a callback function.
 *
 * There are currently six callbacks implemented:
 *
 * SCB_CONNECT
 *      Called when a connection initiated with conn() completes.  The
 *      void * parameter is not used.
 *
 * SCB_DISCONNECT
 *      Called when a connection is broken, either by the remote end or as
 *      a result of calling disconn().  Also called when a connection
 *      initiated with conn() fails.  The void * parameter is set to either
 *      DISCONN_LOCAL, DISCONN_REMOTE, or DISCONN_CONNFAIL to indicate the
 *      cause of the disconnection.  For DISCONN_REMOTE or DISCONN_CONNFAIL,
 *      the `errno' variable will contain the cause of disconnection if
 *      known, zero otherwise.
 *
 * SCB_ACCEPT
 *      Called when a listener socket receives a connection.  The void *
 *      parameter, cast to Socket *, is the new socket created for the
 *      incoming connection; the address of the remote connection can be
 *      retrieved with sock_remote().  NOTE: a listener socket will not
 *      accept any connections unless this callback is set.
 *
 * SCB_READ
 *      Called when data is available for reading.  The void * parameter,
 *      cast to uint32, indicates the number of bytes available for reading.
 *      If some, but not all, of the data is read by the routine (using
 *      sread(), sgetc(), etc.), the remainder will be kept in the read
 *      buffer and the callback will be called again immediately.
 *
 * SCB_READLINE
 *      Like SCB_READ, called when data is available for reading; however,
 *      this callback is only called when at least one full line of data is
 *      available for reading.  The void * parameter, cast to uint32,
 *      indicates the number of bytes available for reading.  If both
 *      SCB_READ and SCB_READLINE are set, SCB_READ will be called first,
 *      and if it leaves any data (including a newline) in the buffer,
 *      SCB_READLINE will then be called.  Likewise, if SCB_READLINE leaves
 *      data in the buffer--which may include a partial line, if a
 *      fractional number of lines was received--SCB_READ will be called
 *      immediately following.
 *
 * SCB_TRIGGER
 *      Called when a "write trigger" is encountered.  These triggers are
 *      set with swrite_trigger(), and are called when all data written to
 *      the socket before the swrite_trigger() call has been sent to the
 *      remote host, but before any data written after the trigger is sent.
 *      The void * parameter is the parameter passed to swrite_trigger().
 */

/*************************************************************************/

/* Minimum (also initial) buffer size for socket; also serves as the
 * increment for buffer expansion. */
#define SOCK_MIN_BUFSIZE 4096

/* Structure for socket data.  This structure is not actually defined here
 * in order to hide it from the user. */
struct socket_;

/* Typedef for socket structure. */
typedef struct socket_ Socket;

/* Type of socket callback functions. */
typedef void (*SocketCallback)(Socket *s, void *param);

/* Identifiers for callback functions (used with sock_setcb()). */
typedef enum {
    SCB_CONNECT = 1,
    SCB_DISCONNECT,
    SCB_ACCEPT,
    SCB_READ,
    SCB_READLINE,
    SCB_TRIGGER,
} SocketCallbackID;

/* Values of parameter to disconnect callback. */
#define DISCONN_LOCAL    ((void *)1)    /* disconn() function called */
#define DISCONN_REMOTE   ((void *)2)    /* Transmission error */
#define DISCONN_CONNFAIL ((void *)3)    /* Connection attempt failed */

/*************************************************************************/

extern void sock_set_buflimits(uint32 per_conn, uint32 total);
extern void sock_set_rto(int msec);

extern Socket *sock_new(void);
extern void sock_free(Socket *s);
extern void sock_setcb(Socket *s, SocketCallbackID which, SocketCallback func);
extern int sock_isconn(const Socket *s);
extern int sock_remote(const Socket *s, struct sockaddr *sa, int *lenptr);
extern void sock_set_blocking(Socket *s, int blocking);
extern int sock_get_blocking(const Socket *s);
extern void sock_set_wto(Socket *s, int seconds);
extern void sock_mute(Socket *s);
extern void sock_unmute(Socket *s);
extern uint32 read_buffer_len(const Socket *s);
extern uint32 write_buffer_len(const Socket *s);
extern int sock_rwstat(const Socket *s, uint64 *read_ret, uint64 *written_ret);
extern int sock_bufstat(const Socket *s, uint32 *socksize_ret,
                        uint32 *totalsize_ret, int *ratio1_ret,
                        int *ratio2_ret);

extern void check_sockets(void);

extern int conn(Socket *s, const char *host, int port, const char *lhost,
                int lport);
extern int disconn(Socket *s);
extern int open_listener(Socket *s, const char *host, int port, int backlog);
extern int close_listener(Socket *s);
extern int32 sread(Socket *s, char *buf, int32 len);
extern int32 swrite(Socket *s, const char *buf, int32 len);
extern int32 swritemap(Socket *s, const char *buf, int32 len);
extern int swrite_trigger(Socket *s, void *param);
extern int sgetc(Socket *s);
extern char *sgets(char *buf, int32 len, Socket *s);
extern char *sgets2(char *buf, int32 len, Socket *s);
extern int sputs(const char *str, Socket *s);
extern int sockprintf(Socket *s, const char *fmt,...);
extern int vsockprintf(Socket *s, const char *fmt, va_list args);

/*************************************************************************/

#endif  /* SOCKETS_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
