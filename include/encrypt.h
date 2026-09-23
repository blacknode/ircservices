/* Include file for encryption routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#ifndef ENCRYPT_H
#define ENCRYPT_H

/*************************************************************************/

/* Structure encapsulating a password and the type of encryption used to
 * encrypt it. */

typedef struct {
    char password[PASSMAX];     /* The password itself, possibly encrypted */
    const char *cipher;         /* Encryption cipher name, or NULL for none */
} Password;

/*************************************************************************/

/* High-level password manipulation functions. */


/* Allocate and return a new, empty Password structure.  Always succeeds
 * (smalloc() will throw a signal if memory cannot be allocated). */
extern Password *new_password(void);

/* Initialize a preallocated Password structure.  Identical in behavior to
 * new_password(), except that the passed-in structure is used instead of
 * allocating a new one, and the structure pointer is not returned. */
extern void init_password(Password *password);

/* Set the contents of a Password structure to the given values.  If
 * cipher is not NULL, a copy of it is made, so the original string may be
 * disposed of after calling set_password(). */
extern void set_password(Password *password,
                         const char password_buffer[PASSMAX],
                         const char *cipher);

/* Copy the contents of a Password structure to another Password structure.
 * The destination password comes first, a la memcpy(). */
extern void copy_password(Password *to, const Password *from);

/* Clear and free memory used by the contents of a Password structure,
 * without freeing the structure itself.  Similar to init_password(), but
 * assumes that the contents of the Password structure are valid (in
 * particular, assumes that password->cipher needs to be freed if it is not
 * NULL). */
extern void clear_password(Password *password);

/* Free a Password structure allocated with new_password().  Does nothing
 * if NULL is given. */
extern void free_password(Password *password);

/* Encrypt string `plaintext' of length `len', placing the result in
 * `password'.  Returns:
 *     0 on success
 *    -2 if the encrypted password is too long to fit in the buffer
 *    -1 on other error */
extern int encrypt_password(const char *plaintext, int len,
                            Password *password);

/* Decrypt `password' into buffer `dest' of length `size'.  Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the decrypted password
 *    -2 if the encryption algorithm does not allow decryption
 *    -1 on other error */
extern int decrypt_password(const Password *password, char *dest, int size);

/* Check an input password `plaintext' against a stored, encrypted password
 * `password'.  Return value is:
 *     1 if the password matches
 *     0 if the password does not match
 *    -1 if an error occurred while checking */
extern int check_password(const char *plaintext, const Password *password);

/*************************************************************************/

/* Low-level encryption/decryption functions.  Each encryption module must
 * implement all of these functions. */


/* encrypt(): Encrypt `src' of length `len' into `dest' of size `size'.
 * Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the encrypted text
 *    -1 on other error */
typedef int (*encrypt_func_t)(const char *src, int len, char *dest, int size);

/* Decrypt `src' into buffer `dest' of size `size'.  Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the decrypted text
 *    -2 if the encryption algorithm does not allow decryption
 *    -1 on other error */
typedef int (*decrypt_func_t)(const char *src, char *dest, int size);

/* Check an input password `plaintext' against a stored, encrypted password
 * `password'.  Return value is:
 *     1 if the password matches
 *     0 if the password does not match
 *    -1 if an error occurred while checking */
typedef int (*check_password_func_t)(const char *plaintext,
                                     const char *password);

/*************************************************************************/

/* Registration and de-registration of ciphers (encryption modules). */

typedef struct cipherinfo_ CipherInfo;
struct cipherinfo_ {
    CipherInfo *next, *prev;    /* Internal use only */
    const char *name;           /* Cipher name (use the module name) */
    encrypt_func_t encrypt;     /* Cipher functions */
    decrypt_func_t decrypt;
    check_password_func_t check_password;
};

/* Register a new cipher. */
extern void register_cipher(CipherInfo *ci);

/* Unregister a cipher.  Does nothing if the cipher was not registered. */
extern void unregister_cipher(CipherInfo *ci);

/*************************************************************************/

#endif  /* ENCRYPT_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
