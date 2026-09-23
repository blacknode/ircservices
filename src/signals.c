/* Signal handling routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include <setjmp.h>

/*************************************************************************/

/* If we get a signal, use this to jump out of the main loop. */
static sigjmp_buf *panic_ptr = NULL;

/*************************************************************************/
/*************************************************************************/

/* Various signal handlers. */

/*************************************************************************/

/* SIGHUP = save databases and rehash configuration files */
static void sighup_handler(int sig_unused)
{
    signal(SIGHUP, SIG_IGN);  /* in case we get double signalled */
    log("Received SIGHUP, saving data and rehashing.");
    wallops(NULL,
            "Received SIGHUP, saving data and rehashing configuration files");
    save_data_now();
    reconfigure();
    signal(SIGHUP, sighup_handler);
}

/*************************************************************************/

/* SIGTERM = save databases and shut down */
static void sigterm_handler(int sig_unused)
{
    save_data = 1;
    delayed_quit = 1;
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    log("Received SIGTERM, exiting.");
    strbcpy(quitmsg, "Shutting down on SIGTERM");
    siglongjmp(*panic_ptr, 1);
}

/*************************************************************************/

/* SIGUSR2 = close and reopen log file */
static void sigusr2_handler(int sig_unused)
{
    log("Received SIGUSR2, cycling log file.");
    if (log_is_open()) {
        close_log();
        open_log();
    }
    signal(SIGUSR2, sigusr2_handler);
}

/*************************************************************************/

/* If we get a weird signal, come here. */
static void weirdsig_handler(int signum)
{
    static int dying = 0;  /* Flag to avoid infinite recursion */

    if (dying++) {
        /* Double signal, give up.  Set `servsock' to NULL to avoid a
         * message going out that way, just in case the socket code is
         * confused/broken */
        servsock = NULL;
        if (signum == SIGUSR2) {
            fatal("Out of memory while shutting down");
        } else {
#if HAVE_STRSIGNAL
            fatal("Caught signal %d (%s) while shutting down", signum,
                  strsignal(signum));
#else
            fatal("Caught signal %d while shutting down", signum);
#endif
        }
    }

    /* Avoid spurious keyboard signals killing us while shutting down */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    /* If we died processing a message, let people know about it */
    if (signum != SIGINT && signum != SIGQUIT) {
        if (*inbuf) {
            log("PANIC! signal %d, buffer = %s", signum, inbuf);
            /* Cut off if this would make IRC command >510 characters. */
            if (strlen(inbuf) > 448) {
                inbuf[446] = '>';
                inbuf[447] = '>';
                inbuf[448] = 0;
            }
            wallops(NULL, "PANIC! buffer = %s\r\n", inbuf);
        } else {
            log("PANIC! signal %d (no buffer)", signum);
            wallops(NULL, "PANIC! signal %d (no buffer)", signum);
        }
    }

    /* Pick an appropriate quit message */
    if (signum == SIGUSR1) {
        strbcpy(quitmsg, "Out of memory!");
        quitting = 1;
    } else {
#if HAVE_STRSIGNAL
        snprintf(quitmsg, sizeof(quitmsg),
                 "Services terminating: %s", strsignal(signum));
#else
        snprintf(quitmsg, sizeof(quitmsg),
                 "Services terminating on signal %d", signum);
#endif
        quitting = 1;
    }

    /* Actually quit */
    if (panic_ptr) {
        siglongjmp(*panic_ptr, 1);
    } else {
        log("%s", quitmsg);
        if (isatty(2))
            fprintf(stderr, "%s\n", quitmsg);
        exit(1);
    }
}

/*************************************************************************/
/*************************************************************************/

/* Set up signal handlers.  Catch certain signals to let us do things or
 * panic as necessary, and ignore all others.
 */

void init_signals(void)
{
    int i;

    /* Start out with special signals disabled */
    disable_signals();

    /* Set all signals to "ignore" */
    for (i = 1; i <= NSIG; i++) {
#if DUMPCORE
        if (i != SIGSEGV)
#endif
            if (i != SIGPROF && i != SIGCHLD)
                signal(i, SIG_IGN);
    }

    /* Specify particular signals we want to catch */

    /* Signals that probably mean bad things have happened */
#if !DUMPCORE
    signal(SIGSEGV, weirdsig_handler);
#endif
    signal(SIGBUS, weirdsig_handler);
    signal(SIGILL, weirdsig_handler);
    signal(SIGTRAP, weirdsig_handler);
    signal(SIGFPE, weirdsig_handler);
#ifdef SIGIOT
    signal(SIGIOT, weirdsig_handler);
#endif

    /* This is our "out-of-memory" panic switch */
    signal(SIGUSR1, weirdsig_handler);

    /* Other special handlers */
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, weirdsig_handler);
    signal(SIGQUIT, weirdsig_handler);
    signal(SIGHUP, sighup_handler);
    signal(SIGUSR2, sigusr2_handler);

}

/*************************************************************************/

/* Helper routine for main.c's DO_SIGSETJMP() macro; saves a pointer to the
 * environment buffer locally.
 */

void do_sigsetjmp(void *bufptr)
{
    panic_ptr = bufptr;
}

/*************************************************************************/

/* Enable or disable receipt of certain signals (in particular, those which
 * cause us to take actions other than simply terminating the program, to
 * avoid such signals happening at inopportune times and causing things to
 * break).
 */

void enable_signals(void)
{
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGHUP);
    sigaddset(&sigs, SIGTERM);
    sigaddset(&sigs, SIGUSR2);
    sigprocmask(SIG_UNBLOCK, &sigs, NULL);
}


void disable_signals(void)
{
    sigset_t sigs;
    sigemptyset(&sigs);
    sigaddset(&sigs, SIGHUP);
    sigaddset(&sigs, SIGTERM);
    sigaddset(&sigs, SIGUSR2);
    sigprocmask(SIG_BLOCK, &sigs, NULL);
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
