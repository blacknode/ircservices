/* IRC Services -- main source file.
 * Copyright (c) 1996-2009 Andrew Church <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*************************************************************************/

#include "services.h"
#include "databases.h"
#include "modules.h"
#include "timeout.h"
#include <fcntl.h>
#include <setjmp.h>


/* Hack for sigsetjmp(); since (at least with glibc, and it shouldn't hurt
 * anywhere else) sigsetjmp() only works if you don't leave the stack frame
 * it was called from, we have to call it before calling the signals.c
 * wrapper. */

#define DO_SIGSETJMP() do {     \
    static sigjmp_buf buf;      \
    if (!sigsetjmp(buf, 1))     \
        do_sigsetjmp(&buf);     \
} while (0)


/******** Global variables! ********/

/* Command-line options: (note that configuration variables are in init.c) */
const char *services_dir = SERVICES_DIR;/* -dir=dirname */
int   debug        = 0;                 /* -debug */
int   readonly     = 0;                 /* -readonly */
int   nofork       = 0;                 /* -nofork */
int   noexpire     = 0;                 /* -noexpire */
int   noakill      = 0;                 /* -noakill */
int   forceload    = 0;                 /* -forceload */
int   encrypt_all  = 0;                 /* -encrypt-all */


/* Set to 1 while we are linked to the network */
int linked = 0;

/* Set to 1 if we are to quit */
int quitting = 0;

/* Set to 1 if we are to quit after saving databases */
int delayed_quit = 0;

/* Set to 1 if we are to restart */
int restart = 0;

/* Contains a message as to why services is terminating */
char quitmsg[BUFSIZE] = "";

/* Input buffer - global, so we can dump it if something goes wrong */
char inbuf[BUFSIZE];

/* Socket for talking to server */
Socket *servsock = NULL;

/* Should we update the databases now? */
int save_data = 0;

/* At what time were we started? */
time_t start_time;

/* Were we unable to open the log? (and the error that occurred) */
int openlog_failed, openlog_errno;

/* Module callbacks (global so init.c can set them): */
int cb_connect       = -1;
int cb_save_complete = -1;


/*************************************************************************/
/*************************************************************************/

/* Callbacks for uplink IRC server socket. */

/*************************************************************************/

/* Actions to perform when connection to server completes. */

void connect_callback(Socket *s, void *param_unused)
{
    sock_set_blocking(s, 1);
    sock_setcb(s, SCB_READLINE, readfirstline_callback);
    send_server();
}

/*************************************************************************/

/* Actions to perform when connection to server is broken. */

void disconnect_callback(Socket *s, void *param)
{
    /* We are no longer linked */
    linked = 0;

    if (param == DISCONN_REMOTE || param == DISCONN_CONNFAIL) {
        int errno_save = errno;
        const char *msg = (param==DISCONN_REMOTE ? "Read error from server"
                           : "Connection to server failed");
        snprintf(quitmsg, sizeof(quitmsg),
                 "%s: %s", msg, strerror(errno_save));
        if (param == DISCONN_REMOTE) {
            /* If we were already connected, make sure any changed data is
             * updated before we terminate. */
            delayed_quit = 1;
            save_data = 1;
        } else {
            /* The connection was never made in the first place, so we
             * discard any changes (such as expirations) made on the
             * assumption that either a configuration problem or other
             * external problem exists.  Such changes will be saved the
             * next time Services successfully connects to a server. */
            quitting = 1;
        }
    }
    sock_setcb(s, SCB_READLINE, NULL);
}

/*************************************************************************/

/* Actions to perform when first line is read from socket. */

void readfirstline_callback(Socket *s, void *param_unused)
{
    sock_setcb(s, SCB_READLINE, readline_callback);

    if (!sgets2(inbuf, sizeof(inbuf), s)) {
        /* This shouldn't happen, but just in case... */
        disconn(s);
        fatal("Unable to read greeting from server socket");
    }
    if (strnicmp(inbuf, "ERROR", 5) == 0) {
        /* Close server socket first to stop wallops, since the other
         * server doesn't want to listen to us anyway */
        disconn(s);
        fatal("Remote server returned: %s", inbuf);
    }

    /* We're now linked to the network */
    linked = 1;

    /* Announce a logfile error if there was one */
    if (openlog_failed) {
        wallops(NULL, "Warning: couldn't open logfile: %s",
                strerror(openlog_errno));
        openlog_failed = 0;
    }

    /* Bring in our pseudo-clients */
    introduce_user(NULL);

    /* Let modules do their startup stuff */
    call_callback(cb_connect);

    /* Process the line we read in above */
    process();
}

/*************************************************************************/

/* Actions to perform when subsequent lines are read from socket. */

void readline_callback(Socket *s, void *param_unused)
{
    if (sgets2(inbuf, sizeof(inbuf), s))
        process();
}

/*************************************************************************/
/*************************************************************************/

/* Lock the data directory if possible; return nonzero on success, zero on
 * failure (data directory already locked or cannot create lock file).
 * On failure, errno will be EEXIST if the directory was already locked or
 * a value other than EEXIST if an error occurred creating the lock file.
 *
 * This does not attempt to correct for NFS brokenness w.r.t. O_EXCL and
 * will contain a race condition when used on an NFS filesystem (or any
 * other filesystem which does not support O_EXCL properly).
 */

int lock_data(void)
{
    int fd;

    errno = 0;
    fd = open(LockFilename, O_WRONLY | O_CREAT | O_EXCL, 0);
    if (fd >= 0) {
        close(fd);
        return 1;
    }
    return 0;
}

/*************************************************************************/

/* Check whether the data directory is locked without actually attempting
 * to lock it.  Returns 1 if locked, 0 if not, or -1 if an error occurred
 * while trying to check (in which case errno will be set to an appropriate
 * value, i.e. whatever access() returned).
 */

int is_data_locked(void)
{
    errno = 0;
    if (access(LockFilename, F_OK) == 0)
        return 1;
    if (errno == ENOENT)
        return 0;
    return -1;
}

/*************************************************************************/

/* Unlock the data directory.  Assumes we locked it in the first place.
 * Returns 1 on success, 0 on failure (unable to remove the lock file), or
 * -1 if the lock file didn't exist in the first place (possibly because it
 * was removed by another (misbehaving) program).
 */

int unlock_data(void)
{
    errno = 0;
    if (unlink(LockFilename) == 0)
        return 1;
    if (errno == ENOENT)
        return -1;
    return 0;
}

/*************************************************************************/

/* Subroutine to save databases. */

void save_data_now(void)
{
    if (!lock_data()) {
        if (errno == EEXIST) {
            log("warning: databases are locked, not updating");
            wallops(NULL,
                    "\2Warning:\2 Databases are locked, and cannot be updated."
                    "  Remove the `%s%s%s' file to allow database updates.",
                    *LockFilename=='/' ? "" : services_dir,
                    *LockFilename=='/' ? "" : "/", LockFilename);
        } else {
            log_perror("warning: unable to lock databases, not updating");
            wallops(NULL, "\2Warning:\2 Unable to lock databases; databases"
                          " will not be updated.");
        }
        call_callback_1(cb_save_complete, 0);
    } else {
        log_debug(1, "Saving databases");
        save_all_dbtables();
        if (!unlock_data()) {
            log_perror("warning: unable to unlock databases");
            wallops(NULL,
                    "\2Warning:\2 Unable to unlock databases; future database"
                    " updates may fail until the `%s%s%s' file is removed.",
                    *LockFilename=='/' ? "" : services_dir,
                    *LockFilename=='/' ? "" : "/", LockFilename);
        }
        call_callback_1(cb_save_complete, 1);
    }
}

/*************************************************************************/
/*************************************************************************/

/* Main routine.  (What does it look like? :-) ) */

int main(int ac, char **av, char **envp)
{
    volatile time_t last_update; /* When did we last update the databases? */
    volatile uint32 last_check;  /* When did we last check timeouts? */


    /*** Initialization stuff. ***/

    if (init(ac, av) < 0) {
        fprintf(stderr, "Initialization failed, exiting.\n");
        return 1;
    }


    /* Set up timers. */
    last_send   = time(NULL);
    last_update = time(NULL);
    last_check  = time(NULL);

    /* The signal handler routine will drop back here with quitting != 0
     * if it gets called. */
    DO_SIGSETJMP();


    /*** Main loop. ***/

    while (!quitting) {
        time_t now = time(NULL);
        int32 now_msec = time_msec();

        log_debug(2, "Top of main loop");

        if (!readonly && (save_data || now-last_update >= UpdateTimeout)) {
            save_data_now();
            save_data = 0;
            last_update = now;
        }
        if (delayed_quit)
            break;

        if (sock_isconn(servsock)) {
            if (PingFrequency && now - last_send >= PingFrequency)
                send_cmd(NULL, "PING :%s", ServerName);
        }

        if (now_msec - last_check >= TimeoutCheck) {
            check_timeouts();
            last_check = now_msec;
        }

        check_sockets();

        if (!MergeChannelModes)
            set_cmode(NULL, NULL);  /* flush out any mode changes made */
    }


    /*** Cleanup stuff. ***/

    cleanup();

    /* Check for restart instead of exit */
    if (restart) {
        execve(SERVICES_BIN, av, envp);
        /* If we get here, the exec() failed; override readonly and write a
         * note to the log file */
        {
            int errno_save = errno;
            open_log();
            errno = errno_save;
        }
        log_perror("Restart failed");
        close_log();
        return 1;
    }

    /* Terminate program */
    return 0;
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
