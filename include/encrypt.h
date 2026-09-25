/* Include file for encryption routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * A password is stored as its cipher writes it -- with encryption/argon2,
 * an Argon2id hash -- and making one or checking one is deliberately slow:
 * a tenth of a second and tens of megabytes, which is what makes a stolen
 * database expensive to attack.  That work never runs in the main thread.
 *
 * HOW A COMMAND WAITS FOR IT.  check_password() and encrypt_password()
 * are called from a command handler as they always were.  The first time,
 * they hand the work to a worker thread and return PASSWORD_PENDING: the
 * handler stops there, silently.  When the work is done, the core runs the
 * whole command again -- the same line, for the same user, through the
 * pseudoclient that received it -- and this time the same call, with the
 * same password, returns the answer at once.  Everything the handler
 * checked before the password is checked again (the user may have quit,
 * the nick may have been dropped meanwhile), so no handler has to keep
 * state across the wait.  For this to work the pseudoclient says which
 * command is running (password_command_begin/end), and a handler must not
 * say or change anything before its password call that it would not want
 * said or changed twice.
 *
 * A user has at most one password being worked on at a time; a command
 * that needs another while the first is running is dropped (it returns
 * PASSWORD_PENDING and is never run again).
 */

#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "services.h"

struct Module_;

/*************************************************************************/

/* Structure encapsulating a password and the type of encryption used to
 * encrypt it. */

typedef struct {
    char password[PASSHASHMAX]; /* The password as its cipher stores it */
    const char* cipher;         /* Cipher name, or NULL: stored as given
                                 * (from before a cipher was required) */
} Password;

/* Returned by check_password() and encrypt_password() when the answer is
 * being worked out in a worker thread: stop, the command will be run
 * again (see above). */
#define PASSWORD_PENDING (-3)

/*************************************************************************/

/* High-level password manipulation functions. */

/* Allocate and return a new, empty Password structure.  Always succeeds
 * (smalloc() will throw a signal if memory cannot be allocated). */
extern Password* new_password(void);

/* Initialize a preallocated Password structure.  Identical in behavior to
 * new_password(), except that the passed-in structure is used instead of
 * allocating a new one, and the structure pointer is not returned. */
extern void init_password(Password* password);

/* Set the contents of a Password structure to the given values.  If
 * cipher is not NULL, a copy of it is made, so the original string may be
 * disposed of after calling set_password(). */
extern void set_password(Password* password,
                         const char password_buffer[PASSHASHMAX],
                         const char* cipher);

/* Copy the contents of a Password structure to another Password structure.
 * The destination password comes first, a la memcpy(). */
extern void copy_password(Password* to, const Password* from);

/* Clear and free memory used by the contents of a Password structure,
 * without freeing the structure itself.  Similar to init_password(), but
 * assumes that the contents of the Password structure are valid (in
 * particular, assumes that password->cipher needs to be freed if it is not
 * NULL). */
extern void clear_password(Password* password);

/* Free a Password structure allocated with new_password().  Does nothing
 * if NULL is given. */
extern void free_password(Password* password);

/* Encrypt string `plaintext' of length `len' with the cipher of options
 * { encryption } (argon2 unless set otherwise), placing the result in
 * `password'.  Returns:
 *     0 on success
 *    -2 if the encrypted password is too long to fit in the buffer
 *    -1 on other error (the cipher's module is not loaded, the workers
 *       are too busy, or this is not a command)
 *    PASSWORD_PENDING: stop; the command will be run again */
extern int encrypt_password(const char* plaintext, int len,
                            Password* password);

/* Decrypt `password' into buffer `dest' of length `size'.  Runs in the
 * main thread (decrypting is not slow; with a hash it is impossible).
 * Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the decrypted password
 *    -2 if the encryption algorithm does not allow decryption
 *    -1 on other error */
extern int decrypt_password(const Password* password, char* dest, int size);

/* Check an input password `plaintext' against a stored password
 * `password'.  Return value is:
 *     1 if the password matches
 *     0 if the password does not match
 *    -1 if an error occurred while checking
 *    PASSWORD_PENDING: stop; the command will be run again
 * When it matches and the stored password was made without a cipher, or
 * with a cost below the current one, `password' is replaced by a new one
 * made with the current cipher and cost: the caller's record then holds
 * the new one and is written as any change is. */
extern int check_password(const char* plaintext, Password* password);

/*************************************************************************/

/* The command being run, for the password work above.  A pseudoclient
 * calls password_command_begin() with the line it received before
 * running the command, and password_command_end() afterwards; `replay'
 * is how that line is run again, as `u', once a password is ready (it is
 * given a copy it may write to).  `mod' is THIS_MODULE: work pending when
 * it is unloaded is dropped. */
typedef void (*PasswordReplayFn)(User* u, char* line);
extern void password_command_begin(struct Module_* mod, User* u,
                                   PasswordReplayFn replay,
                                   const char* line);
extern void password_command_end(void);

/* Nonzero while a command is being run again: for a pseudoclient that
 * logs its commands and would rather not log one twice. */
extern int password_replaying(void);

/* Log a warning if new passwords cannot be made: the cipher named by
 * options { encryption } is not loaded.  Called after the modules are. */
extern void check_encryption(void);

/* Drop the password work of a module being unloaded (core only). */
extern void password_drop_module(struct Module_* mod);

/*************************************************************************/

/* Ciphers: the modules that make and check stored passwords
 * (encryption/argon2). */

/* The settings a cipher's functions need, copied in the main thread and
 * handed to the worker thread with the work: a worker never reads the
 * configuration, which REHASH replaces. */
#define CIPHER_PARAMS_MAX 512
typedef struct {
    unsigned char data[CIPHER_PARAMS_MAX];
    size_t len;
} CipherParams;

typedef struct cipherinfo_ CipherInfo;
struct cipherinfo_ {
    CipherInfo *next, *prev; /* Internal use only */
    const char* name;        /* Cipher name, as stored with passwords */
    struct Module_* owner;   /* THIS_MODULE: its work is dropped, and
                              * waited for, when it is unloaded */

    /* Main thread: copy the current settings into `params'.  NULL if
     * the cipher has none. */
    void (*params)(CipherParams* params);

    /* Worker thread, pure (see worker.h): no Services state, no
     * smalloc(), no log().  */

    /* Encrypt `src' of length `len' into `dest' of size `size'.  Returns
     * 0 on success, +N if `dest' is too small (N is the size needed), -1
     * on other error. */
    int (*encrypt)(const CipherParams* params, const char* src, int len,
                   char* dest, int size);
    /* Check `len' bytes of `plaintext' against the stored `password'
     * (NUL-terminated).  Returns 1 if it matches, 0 if not, -1 on
     * error. */
    int (*check_password)(const CipherParams* params, const char* plaintext,
                          int len, const char* password);
    /* Nonzero if `password' was made with settings weaker than the
     * current ones and should be made again.  NULL if never. */
    int (*outdated)(const CipherParams* params, const char* password);

    /* Main thread.  Decrypt `src' into `dest' of size `size'; see
     * decrypt_password() for the return values. */
    int (*decrypt)(const char* src, char* dest, int size);
};

/* Register a new cipher. */
extern void register_cipher(CipherInfo* ci);

/* Unregister a cipher.  Does nothing if the cipher was not registered. */
extern void unregister_cipher(CipherInfo* ci);

/*************************************************************************/

#endif /* ENCRYPT_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
