/* High-level encryption routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

#include "services.h"
#include "modules.h"
#include "encrypt.h"

/*************************************************************************/

/* List of available ciphers. */
static CipherInfo *cipherlist;

/*************************************************************************/
/*************************************************************************/

/* Default (no encryption) routines.  Used when no encryption is selected. */

/*************************************************************************/

/* encrypt(): Encrypt `src' of length `len' into `dest' of size `size'.
 * Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the encrypted text
 *    -1 on other error
 */

static int default_encrypt(const char *src, int len, char *dest, int size)
{
    if (size < PASSMAX) {
        return PASSMAX;
    }
    memset(dest, 0, size);
    memcpy(dest, src, (len > size) ? size : len);
    return 0;
}

/*************************************************************************/

/* Decrypt `src' into buffer `dest' of size `size'.  Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the decrypted text
 *    -2 if the encryption algorithm does not allow decryption
 *    -1 on other error
 */

static int default_decrypt(const char *src, char *dest, int size)
{
    int passlen;
    for (passlen = 0; passlen < PASSMAX; passlen++) {
        if (!src[passlen]) {
            break;
        }
    }
    if (size < passlen+1) {
        return passlen+1 - size;
    }
    memset(dest, 0, size);
    memcpy(dest, src, passlen);
    return 0;
}

/*************************************************************************/

/* Check an input password `plaintext' against a stored, encrypted password
 * `password'.  Return value is:
 *     1 if the password matches
 *     0 if the password does not match
 *    -1 if an error occurred while checking
 */

static int default_check_password(const char *plaintext, const char *password)
{
    if (strncmp(plaintext, password, PASSMAX) == 0) {
        return 1;
    } else {
        return 0;
    }
}

/*************************************************************************/
/*************************************************************************/

/* High-level password encryption routines. */

/*************************************************************************/

/* Allocate and return a new, empty Password structure.  Always succeeds
 * (smalloc() will throw a signal if memory cannot be allocated).
 */

Password *new_password(void)
{
    Password *password = smalloc(sizeof(*password));
    init_password(password);
    return password;
}

/*************************************************************************/

/* Initialize a preallocated Password structure.  Identical in behavior to
 * new_password(), except that the passed-in structure is used instead of
 * allocating a new one, and the structure pointer is not returned.
 */

void init_password(Password *password)
{
    memset(password->password, 0, sizeof(password->password));
    password->cipher = NULL;
}

/*************************************************************************/

/* Set the contents of a Password structure to the given values.  If
 * cipher is not NULL, a copy of it is made, so the original string may be
 * disposed of after calling set_password().
 */

void set_password(Password *password,
                  const char password_buffer[PASSMAX],
                  const char *cipher)
{
    memcpy(password->password, password_buffer, PASSMAX);
    if (cipher) {
        password->cipher = sstrdup(cipher);
    } else {
        password->cipher = NULL;
    }
}

/*************************************************************************/

/* Copy the contents of a Password structure to another Password structure.
 * The destination password comes first, a la memcpy().
 */

void copy_password(Password *to, const Password *from)
{
    clear_password(to);
    memcpy(to->password, from->password, sizeof(to->password));
    if (from->cipher) {
        to->cipher = sstrdup(from->cipher);
    } else {
        to->cipher = NULL;
    }
}

/*************************************************************************/

/* Clear and free memory used by the contents of a Password structure,
 * without freeing the structure itself.  Similar to init_password(), but
 * assumes that the contents of the Password structure are valid (in
 * particular, assumes that password->cipher needs to be freed if it is
 * not NULL).
 */

void clear_password(Password *password)
{
    memset(password->password, 0, sizeof(password->password));
    free((char *)password->cipher);
    password->cipher = NULL;
}

/*************************************************************************/

/* Free a Password structure allocated with new_password().  Does nothing
 * if NULL is given.
 */

void free_password(Password *password)
{
    if (password) {
        clear_password(password);
        free(password);
    }
}

/*************************************************************************/

/* Encrypt string `plaintext' of length `len', placing the result in
 * `password'.  Returns:
 *     0 on success
 *    -2 if the encrypted password is too long to fit in the buffer
 *    -1 on other error
 */

int encrypt_password(const char *plaintext, int len, Password *password)
{
    encrypt_func_t low_encrypt = default_encrypt;
    int res;

    if (EncryptionType) {
        CipherInfo *ci;
        LIST_SEARCH(cipherlist, name, EncryptionType, strcmp, ci);
        if (!ci) {
            log("encrypt_password(): cipher `%s' not available!",
                EncryptionType);
            return -1;
        }
        low_encrypt = ci->encrypt;
    }
    clear_password(password);
    res = (*low_encrypt)(plaintext, len, password->password,
                         sizeof(password->password));
    if (res == 0) {
        if (EncryptionType) {
            password->cipher = strdup(EncryptionType);
            if (!password->cipher) {
                module_log_perror("strdup() failed in encrypt_password()");
                clear_password(password);
                return -1;
            }
        }
        return 0;
    } else {
        clear_password(password);
        if (res > 0) {  /* buffer too small */
            return -2;
        } else {
            return -1;
        }
    }
}

/*************************************************************************/

/* Decrypt `password' into buffer `dest' of length `size'.  Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the decrypted password
 *    -2 if the encryption algorithm does not allow decryption
 *    -1 on other error
 */

int decrypt_password(const Password *password, char *dest, int size)
{
    decrypt_func_t low_decrypt = default_decrypt;

    if (password->cipher) {
        CipherInfo *ci;
        LIST_SEARCH(cipherlist, name, password->cipher, strcmp, ci);
        if (!ci) {
            log("decrypt_password(): cipher `%s' not available!",
                password->cipher);
            return -1;
        }
        low_decrypt = ci->decrypt;
    }
    return (*low_decrypt)(password->password, dest, size);
}

/*************************************************************************/

/* Check an input password `plaintext' against a stored, encrypted password
 * `password'.  Return value is:
 *     1 if the password matches
 *     0 if the password does not match
 *    -1 if an error occurred while checking
 */

int check_password(const char *plaintext, const Password *password)
{
    check_password_func_t low_check_password = default_check_password;

    if (password->cipher) {
        CipherInfo *ci;
        LIST_SEARCH(cipherlist, name, password->cipher, strcmp, ci);
        if (!ci) {
            log("check_password(): cipher `%s' not available!",
                password->cipher);
            return -1;
        }
        low_check_password = ci->check_password;
    }
    return (*low_check_password)(plaintext, password->password);
}

/*************************************************************************/
/*************************************************************************/

/* Cipher registration/unregistration. */

/*************************************************************************/

/* Register a new cipher. */

void register_cipher(CipherInfo *ci)
{
    LIST_INSERT(ci, cipherlist);
}

/*************************************************************************/

/* Unregister a cipher.  Does nothing if the cipher was not registered. */

void unregister_cipher(CipherInfo *ci)
{
    CipherInfo *ci2;
    LIST_FOREACH (ci2, cipherlist) {
        if (ci2 == ci) {
            LIST_REMOVE(ci, cipherlist);
            break;
        }
    }
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
