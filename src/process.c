/* Main processing code for Services.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "messages.h"

static int cb_recvmsg = -1;

/*************************************************************************/
/*************************************************************************/

/* split_buf:  Split a buffer into arguments and store a pointer to the
 *             argument vector in argv_ptr; return the argument count.
 *             The argument vector will point to a static buffer;
 *             subsequent calls will overwrite this buffer.
 *             If colon_special is non-zero, then treat a parameter with a
 *             leading ':' as the last parameter of the line, per the IRC
 *             RFC.  Destroys the buffer by side effect.
 */

static char **sbargv = NULL;  /* File scope so process_cleanup() can free it */

int split_buf(char *buf, char ***argv_ptr, int colon_special)
{
    static int argvsize = 8;
    int argc;
    char *s;

    if (!sbargv)
        sbargv = smalloc(sizeof(char *) * argvsize);
    argc = 0;
    while (*buf) {
        if (argc == argvsize) {
            argvsize += 8;
            sbargv = srealloc(sbargv, sizeof(char *) * argvsize);
        }
        if (*buf == ':' && colon_special) {
            sbargv[argc++] = buf+1;
            *buf = 0;
        } else {
            s = strpbrk(buf, " ");
            if (s) {
                *s++ = 0;
                while (*s == ' ')
                    s++;
            } else {
                s = buf + strlen(buf);
            }
            sbargv[argc++] = buf;
            buf = s;
        }
    }
    *argv_ptr = sbargv;
    return argc;
}

/*************************************************************************/
/*************************************************************************/

int process_init(int ac, char **av)
{
    cb_recvmsg = register_callback("receive message");
    if (cb_recvmsg < 0) {
        log("process_init: register_callback() failed\n");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void process_cleanup(void)
{
    unregister_callback(cb_recvmsg);
    free(sbargv);
    sbargv = NULL;
}

/*************************************************************************/

/* process:  Main processing routine.  Takes the string in inbuf (global
 *           variable) and does something appropriate with it. */

void process(void)
{
    char source[64];
    char cmd[64];
    char buf[512];              /* Longest legal IRC command line */
    char *s;
    int ac;                     /* Parameters for the command */
    char **av;


    /* If debugging, log the buffer. */
    log_debug(1, "Received: %s", inbuf);

    /* First make a copy of the buffer so we have the original in case we
     * crash - in that case, we want to know what we crashed on. */
    strbcpy(buf, inbuf);

    /* Split the buffer into pieces. */
    if (*buf == ':') {
        s = strpbrk(buf, " ");
        if (!s)
            return;
        *s = 0;
        while (isspace(*++s))
            ;
        strbcpy(source, buf+1);
        strmove(buf, s);
    } else {
        *source = 0;
    }
    if (!*buf)
        return;
    s = strpbrk(buf, " ");
    if (s) {
        *s = 0;
        while (isspace(*++s))
            ;
    } else
        s = buf + strlen(buf);
    strbcpy(cmd, buf);
    ac = split_buf(s, &av, 1);

    /* Do something with the message. */
    if (call_callback_4(cb_recvmsg, source, cmd, ac, av) <= 0) {
        Message *m = find_message(cmd);
        if (m) {
            if (m->func)
                m->func(source, ac, av);
        } else {
            log("unknown message from server (%s)", inbuf);
        }
    }

    /* Finally, clear the first byte of `inbuf' to signal that we're
     * finished processing. */
    *inbuf = 0;
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
