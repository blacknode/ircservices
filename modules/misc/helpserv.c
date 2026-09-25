/* HelpServ functions.
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
#include "language.h"
#include <sys/stat.h>

/*************************************************************************/

static char *HelpDir;

/*************************************************************************/
/*************************************************************************/

/* Main HelpServ routine: a PRIVMSG to HelpServ names a help topic. */

static void helpserv_message(struct Service *service, User *u, char *topic)
{
    FILE *f;
    struct stat st;
    char buf[PATH_MAX+1], *ptr;
    const char *s;
    char *old_topic;    /* an unclobbered (by strtok) copy */

    old_topic = sstrdup(topic);

    /* As we copy path parts, (1) lowercase everything and (2) make sure
     * we don't let any special characters through -- this includes '.'
     * (which could get parent dir) or '/' (which couldn't _really_ do
     * anything nasty if we keep '.' out, but better to be on the safe
     * side).  Special characters turn into '_'.
     */
    strbcpy(buf, HelpDir);
    ptr = buf + strlen(buf);
    for (s = strtok(topic, " "); s && ptr-buf < sizeof(buf)-1;
                                                s = strtok(NULL, " ")) {
        *ptr++ = '/';
        while (*s && ptr-buf < sizeof(buf)-1) {
            if (*s == '.' || *s == '/')
                *ptr++ = '_';
            else
                *ptr++ = tolower(*s);
            s++;
        }
        *ptr = 0;
    }

    /* If we end up at a directory, go for an "index" file/dir if
     * possible.
     */
    while (ptr-buf < sizeof(buf)-6
                && stat(buf, &st) == 0 && S_ISDIR(st.st_mode)) {
        strcpy(ptr, "/index");
        ptr += strlen(ptr);
    }

    /* Send the file, if it exists.
     */
    if (!(f = fopen(buf, "r"))) {
        module_log_perror_debug(1, "Cannot open help file %s", buf);
        notice_lang(service->nick, u, NO_HELP_AVAILABLE, old_topic);
    } else {
        while (fgets(buf, sizeof(buf), f)) {
            s = strtok(buf, "\n");
            /* Use this construction to prevent any %'s in the text from
             * doing weird stuff to the output.  Also replace blank lines by
             * spaces (see send.c/notice_list() for an explanation of why).
             */
            notice(service->nick, u->nick, "%s", s ? s : " ");
        }
        fclose(f);
    }
    free(old_topic);
}

/*************************************************************************/
/***************************** Module stuff ******************************/
/*************************************************************************/

/* HelpServName = <nick>, <description>; in the module block. */
static struct Service helpserv_service = {
    .directive = "HelpServName",
    .on_message = helpserv_message,
};

static ConfigDirective helpserv_config[] = {
    { "HelpDir",          { { CD_STRING, CF_DIRREQ, &HelpDir } } },
    { NULL }
};

ModuleInfo module_info = {
    .abi = MODULE_ABI,
    .description = "HelpServ: help topics read from files",
    .config = helpserv_config,
    .services = MODULE_SERVICES(&helpserv_service),
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
