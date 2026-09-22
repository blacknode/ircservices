/* Declarations for token handling.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef TOKEN_H
#define TOKEN_H

/* Information about a token (what message it corresponds to): */
typedef struct {
    const char *token, *message;
} TokenInfo;

#define init_token RENAME_SYMBOL(init_token)
#define exit_token RENAME_SYMBOL(exit_token)
extern int init_token(TokenInfo *tokens);
extern void exit_token(void);

#endif  /* TOKEN_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
