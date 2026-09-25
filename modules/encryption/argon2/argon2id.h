/* Argon2id (RFC 9106) and the BLAKE2b it is built on.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/ircd_argon2.h).
 *
 * What a password is stored as.  Argon2 is deliberately expensive in both
 * time and memory, which is the point: an attacker with the stored hashes
 * and a warehouse of graphics cards is trying to guess billions of
 * passwords, and memory is the one cost that does not fall away when the
 * work moves off a general-purpose processor.
 *
 * NEVER CALL THIS ON THE MAIN THREAD.  A sensible cost setting takes
 * between fifty and two hundred and fifty milliseconds and allocates tens
 * of megabytes, on purpose.  At a tenth of a second per check, ten people
 * identifying at once would stop Services for a second; a hundred, for
 * ten.  The core runs it in a worker thread (src/encrypt.c), and nothing
 * here touches core state, which is what makes that safe.  This file and
 * the ones that include it do not include services.h (see worker.h).
 */

#ifndef ARGON2ID_H
#define ARGON2ID_H

#include <stddef.h>

/* Length of a BLAKE2b-512 digest in bytes. */
#define BLAKE2B_DIGEST_LEN 64

/* BLAKE2b over `len' bytes of `data', into `outlen' bytes of `out' (at
 * most BLAKE2B_DIGEST_LEN), keyed with `keylen' (at most 64) bytes of
 * `key', or unkeyed if `key' is NULL. */
extern void a2_blake2b(unsigned char* out, size_t outlen, const void* key,
                       size_t keylen, const void* data, size_t len);

/* Everything about an Argon2 hash other than the password and the salt. */
struct Argon2Params {
    /* A key mixed into every hash, or NULL: the pepper.  It is not stored
     * with the hashes, so a stolen database on its own is not enough to
     * start guessing passwords against; the attacker needs the
     * configuration too.  It costs nothing and it is the difference
     * between one theft and two. */
    const void* secret;
    size_t secretlen;

    /* Associated data mixed in, or NULL.  Rarely wanted; there for the
     * specification's sake and because the test vectors use it. */
    const void* ad;
    size_t adlen;

    unsigned int t_cost; /* Passes over the memory; at least 1 */
    unsigned int m_cost; /* Memory in kibibytes */
    unsigned int lanes;  /* Degree of parallelism; at least 1 */
};

/* Argon2id: a tag of `outlen' bytes (at least 4) into `out', from the
 * password `pwd' and the salt `salt' (at least 8 bytes, and different
 * every time).  Returns nonzero on success, zero if a parameter was out of
 * range or the memory could not be had. */
extern int a2_argon2id(unsigned char* out, size_t outlen, const void* pwd,
                       size_t pwdlen, const void* salt, size_t saltlen,
                       const struct Argon2Params* params);

/* Compare `len' bytes without leaking where they differ.  Nonzero if they
 * are equal. */
extern int a2_equal(const void* a, const void* b, size_t len);

/* Overwrite memory in a way the optimiser may not remove. */
extern void a2_wipe(void* p, size_t len);

#endif /* ARGON2ID_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
