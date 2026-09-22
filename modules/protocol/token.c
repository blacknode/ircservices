/* Routines for processing message tokens.
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

#include "token.h"

/*************************************************************************/

/* Array of all possible 1- and 2-character tokens. */
static MessageFunc tokentable[65536];

/* Macro to get the token array entry for a given token.  May be used as
 * an lvalue. */
#define TOKEN_ENTRY(token) \
    tokentable[(uint8)((token)[0])<<8 | (uint8)((token)[1])]

/* Dummy value to indicate a recognized-but-has-no-handler message. */
#define MSGFUNC_NONE    ((MessageFunc)-1)

/*************************************************************************/
/*************************************************************************/

/* Callback to check for and handle tokens. */

static int token_receive_message(char *source, char *cmd, int ac, char **av)
{
    MessageFunc func;

    if ((cmd[1] && cmd[2]) || !(func = TOKEN_ENTRY(cmd)))
        return 0;
    if (func != MSGFUNC_NONE)
        func(source, ac, av);
    return 1;
}

/*************************************************************************/

/* Set up token handling. */

int init_token(TokenInfo *tokens)
{
    int32 i;

    /* Clear out token array, then copy token handler functions into array.
     * ANSI says NULL might not be binary 0, and this section isn't
     * particularly speed-critical, so we don't use memset(). */
    for (i = 0; i < 65536; i++)
        tokentable[i] = NULL;
    for (i = 0; tokens[i].token; i++) {
        if (strlen(tokens[i].token) > 2) {
            module_log("warning: init_token(): token name `%s' too long"
                       " (maximum 2 characters)", tokens[i].token);
        } else if (tokens[i].message) {
            Message *m = find_message(tokens[i].message);
            if (m) {
                TOKEN_ENTRY(tokens[i].token) =
                    m->func ? m->func : MSGFUNC_NONE;
            }
        }
    }

    /* Add receive-message callback. */
    if (!add_callback(NULL, "receive message", token_receive_message)) {
        module_log("Unable to add callback");
        return 0;
    }

    return 1;
}

/*************************************************************************/

/* Clean up on module unload. */

void exit_token(void)
{
    remove_callback(NULL, "receive message", token_receive_message);
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
