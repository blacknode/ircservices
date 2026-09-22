/* Module for encryption using the Unix crypt() function.
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
#include "encrypt.h"

#if !HAVE_CRYPT
# define crypt(pass,salt) NULL
#endif

/*************************************************************************/

/* Allowable characters in the salt */
const char saltchars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./";

/*************************************************************************/
/*************************************************************************/

static int unixcrypt_encrypt(const char *src, int len, char *dest, int size)
{
    char buf[PASSMAX+1], salt[3];
    const char *res;

    if (len > sizeof(buf)-1)
        len = sizeof(buf)-1;
    memcpy(buf, src, len);
    buf[len] = 0;
    salt[0] = saltchars[random()%64];
    salt[1] = saltchars[random()%64];
    salt[2] = 0;
    res = crypt(buf, salt);
    if (!res) {
        module_log_perror("encrypt: crypt() failed");
        return -1;
    }
    if (strlen(res) > size-1) {
        module_log("encrypt: crypt() returned too long a string! (%d"
                   " characters)", (int)strlen(res));
        return strlen(res) + 1;
    }
    strscpy(dest, res, size);
    return 0;
}

/*************************************************************************/

static int unixcrypt_decrypt(const char *src, char *dest, int size)
{
    return -2;  /* decryption impossible (in reasonable time) */
}

/*************************************************************************/

static int unixcrypt_check_password(const char *plaintext, const char *password)
{
    const char *res;

    res = crypt(plaintext, password);
    if (!res) {
        module_log_perror("check_password: crypt() failed");
        return -1;
    }
    if (strcmp(res, password) == 0)
        return 1;
    return 0;
}

/*************************************************************************/
/*************************************************************************/

/* Module stuff. */

ConfigDirective module_config[] = {
    { NULL }
};

static CipherInfo unixcrypt_info = {
    .name = "unix-crypt",
    .encrypt = unixcrypt_encrypt,
    .decrypt = unixcrypt_decrypt,
    .check_password = unixcrypt_check_password,
};

int init_module(void)
{
#if !HAVE_CRYPT
    module_log("Your system does not support the crypt() function!");
    return 0;
#else
    if (PASSMAX < 14) {
        module_log("PASSMAX too small (must be at least 14)");
        return 0;
    }
    register_cipher(&unixcrypt_info);
    return 1;
#endif
}

int exit_module(int shutdown_unused)
{
    unregister_cipher(&unixcrypt_info);
    return 1;
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
