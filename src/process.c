/* Main processing code for Services.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "messages.h"
#include "modules.h"
#include "p10.h"
#include "services.h"

static Event* message_receive_event;

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

static char** sbargv = NULL; /* File scope so process_cleanup() can free it */

int split_buf(char* buf, char*** argv_ptr, int colon_special)
{
    static int argvsize = 8;
    int argc;
    char* s;

    if (!sbargv)
        sbargv = smalloc(sizeof(char*) * argvsize);
    argc = 0;
    while (*buf) {
        if (argc == argvsize) {
            argvsize += 8;
            sbargv = srealloc(sbargv, sizeof(char*) * argvsize);
        }
        if (*buf == ':' && colon_special) {
            sbargv[argc++] = buf + 1;
            *buf = 0;
        }
        else {
            s = strpbrk(buf, " ");
            if (s) {
                *s++ = 0;
                while (*s == ' ')
                    s++;
            }
            else {
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

int process_init(int ac, char** av)
{
    message_receive_event = event_declare(NULL, EVENT_MESSAGE_RECEIVE);
    if (!message_receive_event) {
        log("process_init: event_declare() failed");
        return 0;
    }
    return 1;
}

/*************************************************************************/

void process_cleanup(void)
{
    event_retract(message_receive_event);
    free(sbargv);
    sbargv = NULL;
}

/*************************************************************************/

/* process:  Main processing routine.  Takes the line in inbuf (global
 *           variable), decodes it (p10.c) and dispatches it to the
 *           handler registered for its command (messages.c). */

void process(void)
{
    char buf[BUFSIZE];
    const char *source, *cmd;
    int ac;
    char** av;

    log_debug(1, "Received: %s", inbuf);

    /* Work on a copy, so the original is still in inbuf if we crash. */
    strbcpy(buf, inbuf);
    if (p10_parse(buf, &source, &cmd, &ac, &av) &&
        event_emit(message_receive_event, source, cmd, ac, av) <= 0) {
        Message* m = find_message(cmd);
        if (m) {
            if (m->func)
                m->func((char*)source, ac, av);
        }
        else {
            log("unknown message from server (%s)", inbuf);
        }
    }

    /* Clear the first byte of `inbuf' to signal that we're finished. */
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
