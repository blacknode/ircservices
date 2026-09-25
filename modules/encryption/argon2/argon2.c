/* Module for storing passwords as Argon2id hashes.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * The algorithm is ircu2's (argon2id.c, pwhash.c); this file is the cipher
 * Services see (include/encrypt.h).  A password is stored in the encoding
 * every Argon2 implementation reads,
 *
 *     $argon2id$v=19$m=65536,t=3,p=1$<salt>$<tag>
 *
 * with its costs, so that raising them later leaves the old passwords
 * working: each is made again with the new costs the next time its owner
 * gives it (see check_password() in encrypt.h).
 *
 * The three functions marked "worker thread" run in one (src/encrypt.c
 * sends them there): they read only what they are given, and call nothing
 * but the pure functions of argon2id.c and pwhash.c and getentropy().
 */

#include "conffile.h"
#include "encrypt.h"
#include "modules.h"
#include "services.h"

#include "argon2id.h"
#include "pwhash.h"

#include <fcntl.h>
#if HAVE_GETENTROPY
#include <sys/random.h>
#endif

/*************************************************************************/

/* Bytes of salt in a new password. */
#define SALT_LEN 16

/* Longest pepper, in bytes. */
#define PEPPER_MAX 256

/* Smallest memory cost accepted (kibibytes): below this Argon2 is no
 * longer a memory-hard function in any useful sense. */
#define MEMORY_MIN 8192

/* What crosses to the worker with every job: the settings, copied. */
typedef struct {
    unsigned int m_cost;
    unsigned int t_cost;
    size_t pepperlen;
    unsigned char pepper[PEPPER_MAX];
} Argon2Settings;

/* Configuration. */
static int32 Memory;
static int32 Passes;
static char* Pepper;

/*************************************************************************/
/*************************************************************************/

/* Worker thread: `len' random bytes (at most 256) into `buf'.  Returns
 * zero on success. */

static int random_bytes(void* buf, size_t len)
{
#if HAVE_GETENTROPY
    return getentropy(buf, len);
#else
    int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    size_t got = 0;

    if (fd < 0)
        return -1;
    while (got < len) {
        ssize_t n = read(fd, (char*)buf + got, len - got);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            break;
        got += (size_t)n;
    }
    close(fd);
    return got == len ? 0 : -1;
#endif
}

/*************************************************************************/

/* Main thread: the settings, into the job. */

static void argon2_params(CipherParams* params)
{
    Argon2Settings* s = (Argon2Settings*)params->data;

    memset(params, 0, sizeof(*params));
    s->m_cost = Memory > 0 ? (unsigned int)Memory : PWHASH_DEFAULT_MEMORY;
    s->t_cost = Passes > 0 ? (unsigned int)Passes : PWHASH_DEFAULT_TIME;
    if (Pepper && *Pepper) {
        s->pepperlen = strlen(Pepper);
        if (s->pepperlen > PEPPER_MAX)
            s->pepperlen = PEPPER_MAX;
        memcpy(s->pepper, Pepper, s->pepperlen);
    }
    params->len = sizeof(*s);
}

/*************************************************************************/

/* Worker thread: make a password. */

static int argon2_encrypt(const CipherParams* params, const char* src, int len,
                          char* dest, int size)
{
    const Argon2Settings* s = (const Argon2Settings*)params->data;
    unsigned char salt[SALT_LEN];
    char out[PWHASH_MAX + 1];
    size_t n;

    if (random_bytes(salt, sizeof(salt)) != 0)
        return -1;
    if (!a2_pwhash_make(out, src, (size_t)len, salt, sizeof(salt),
                        s->pepperlen ? s->pepper : NULL, s->pepperlen,
                        s->m_cost, s->t_cost)) {
        a2_wipe(salt, sizeof(salt));
        return -1;
    }
    a2_wipe(salt, sizeof(salt));
    n = strlen(out);
    if (n + 1 > (size_t)size) {
        a2_wipe(out, sizeof(out));
        return (int)n + 1;
    }
    memset(dest, 0, size);
    memcpy(dest, out, n);
    a2_wipe(out, sizeof(out));
    return 0;
}

/*************************************************************************/

/* Worker thread: check a password. */

static int argon2_check_password(const CipherParams* params,
                                 const char* plaintext, int len,
                                 const char* password)
{
    const Argon2Settings* s = (const Argon2Settings*)params->data;

    return a2_pwhash_verify(password, plaintext, (size_t)len,
                            s->pepperlen ? s->pepper : NULL, s->pepperlen)
               ? 1
               : 0;
}

/*************************************************************************/

/* Worker thread: was it made with lower costs than the current ones? */

static int argon2_outdated(const CipherParams* params, const char* password)
{
    const Argon2Settings* s = (const Argon2Settings*)params->data;

    return a2_pwhash_outdated(password, s->m_cost, s->t_cost);
}

/*************************************************************************/
/*************************************************************************/

/* Module stuff. */

ConfigDirective module_config[] = {{"Memory", {{CD_POSINT, 0, &Memory}}},
                                   {"Passes", {{CD_POSINT, 0, &Passes}}},
                                   {"Pepper", {{CD_STRING, 0, &Pepper}}},
                                   {NULL}};

static CipherInfo argon2_info = {
    .name = "argon2",
    .params = argon2_params,
    .encrypt = argon2_encrypt,
    .check_password = argon2_check_password,
    .outdated = argon2_outdated,
    .decrypt = NULL, /* a hash cannot be decrypted */
};

/*************************************************************************/

/* The settings, checked: called at load and after every REHASH.  A bad
 * value is logged and the default used, rather than refusing a REHASH
 * over it. */

static void check_config(void)
{
    if (Memory && Memory < MEMORY_MIN) {
        module_log("Memory %d is too small; using %d KiB", (int)Memory,
                   MEMORY_MIN);
        Memory = MEMORY_MIN;
    }
    if (Memory > 4 * 1024 * 1024) {
        module_log("Memory %d KiB is more than 4 GiB; using %d KiB",
                   (int)Memory, PWHASH_DEFAULT_MEMORY);
        Memory = PWHASH_DEFAULT_MEMORY;
    }
    if (Passes > 100) {
        module_log("Passes %d is too many; using %d", (int)Passes,
                   PWHASH_DEFAULT_TIME);
        Passes = PWHASH_DEFAULT_TIME;
    }
    if (Pepper && strlen(Pepper) > PEPPER_MAX)
        module_log("Pepper is longer than %d bytes; only the first %d are"
                   " used",
                   PEPPER_MAX, PEPPER_MAX);
}

static int do_reconfigure(int after_configure)
{
    if (after_configure)
        check_config();
    return 0;
}

/*************************************************************************/

int init_module(void)
{
    if (sizeof(Argon2Settings) > CIPHER_PARAMS_MAX) {
        module_log("BUG: Argon2Settings do not fit in CipherParams");
        return 0;
    }
    if (!add_callback(NULL, "reconfigure", do_reconfigure)) {
        module_log("Unable to add callback");
        return 0;
    }
    check_config();
    argon2_info.owner = THIS_MODULE;
    register_cipher(&argon2_info);
    return 1;
}

/*************************************************************************/

int exit_module(int shutdown_unused)
{
    unregister_cipher(&argon2_info);
    remove_callback(NULL, "reconfigure", do_reconfigure);
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
