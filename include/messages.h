/* Declarations of IRC message structures, variables, and functions.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef MESSAGES_H
#define MESSAGES_H

/*************************************************************************/

/* Type of a message function: */
typedef void (*MessageFunc)(char *source, int ac, char **av);

typedef struct {
    const char *name;
    MessageFunc func;
} Message;

extern int register_messages(Message *table);
extern int unregister_messages(Message *table);
extern Message *find_message(const char *name);
extern int messages_init(int ac, char **av);
extern void messages_cleanup(void);

/*************************************************************************/

#endif  /* MESSAGES_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
