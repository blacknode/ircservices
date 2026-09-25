/* Storing a password, and checking one.  See pwhash.h.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/ircd_pwhash.c).  Runs in worker threads: this
 * file does not include services.h.
 */

#include "pwhash.h"
#include "argon2id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Length of the tag stored, in bytes. */
#define PWHASH_TAG_LEN  32
/** Longest salt accepted when parsing. */
#define PWHASH_SALT_MAX 64

/** The alphabet Argon2's encoding uses: standard base64, without the
 * padding. */
static const char b64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/** Encode \a len bytes, unpadded.
 * @return Characters written, not counting the terminator.
 */
static size_t b64_encode(char* out, size_t outlen, const unsigned char* in,
                         size_t len)
{
    size_t n = 0;
    size_t i = 0;

    while (i < len) {
        unsigned int v = in[i] << 16;
        size_t have = 1;

        if (i + 1 < len) {
            v |= in[i + 1] << 8;
            have++;
        }
        if (i + 2 < len) {
            v |= in[i + 2];
            have++;
        }

        if (n + have + 1 >= outlen)
            return 0;

        out[n++] = b64[(v >> 18) & 0x3f];
        out[n++] = b64[(v >> 12) & 0x3f];
        if (have > 1)
            out[n++] = b64[(v >> 6) & 0x3f];
        if (have > 2)
            out[n++] = b64[v & 0x3f];

        i += 3;
    }

    out[n] = '\0';
    return n;
}

/** The value of one base64 character, or -1. */
static int b64_value(char c)
{
    const char* p = strchr(b64, c);

    return (p && c) ? (int)(p - b64) : -1;
}

/** Decode unpadded base64.
 * @return Bytes written, or -1 if \a in is not base64 or does not fit.
 */
static int b64_decode(unsigned char* out, size_t outlen, const char* in)
{
    size_t n = 0;
    unsigned int acc = 0;
    int bits = 0;

    for (; *in; in++) {
        int v = b64_value(*in);

        if (v < 0)
            return -1;

        acc = (acc << 6) | (unsigned int)v;
        bits += 6;

        if (bits >= 8) {
            bits -= 8;
            if (n >= outlen)
                return -1;
            out[n++] = (unsigned char)(acc >> bits);
        }
    }

    return (int)n;
}

/** What one stored hash says about itself. */
struct PwHashFields {
    unsigned int m_cost;
    unsigned int t_cost;
    unsigned int lanes;
    unsigned char salt[PWHASH_SALT_MAX];
    size_t saltlen;
    unsigned char tag[PWHASH_TAG_LEN];
    size_t taglen;
};

/** Take a stored hash apart.
 * @return Non-zero if it was one this server understands.
 */
/* Widths for the two base64 fields below.  They are written out in the
 * scanf format, which cannot take them from a variable, so the buffers are
 * sized from the same names and the sizes are asserted at compile time: a
 * width that outgrew its buffer is a stack overflow written by sscanf, and
 * it is not the kind of mistake that shows up in ordinary testing.
 */
#define PWHASH_SALT_B64_MAX 127
#define PWHASH_TAG_B64_MAX  63

static int pwhash_parse(const char* stored, struct PwHashFields* out)
{
    char salt_b64[PWHASH_SALT_B64_MAX + 1];
    char tag_b64[PWHASH_TAG_B64_MAX + 1];
    unsigned int v = 0;
    int n;

    /* Base64 of the longest salt and tag this server makes must still fit. */
    {
        typedef char salt_fits
            [(PWHASH_SALT_MAX * 4 + 2) / 3 <= PWHASH_SALT_B64_MAX ? 1 : -1];
        typedef char
            tag_fits[(PWHASH_TAG_LEN * 4 + 2) / 3 <= PWHASH_TAG_B64_MAX ? 1
                                                                        : -1];
        (void)sizeof(salt_fits);
        (void)sizeof(tag_fits);
    }

    if (!stored || !*stored)
        return 0;

    memset(out, 0, sizeof(*out));

    /* One variant, one version.  Anything else is refused rather than
     * guessed at: a stored hash that cannot be read must never verify, and
     * reading it wrong is a way of verifying it wrongly.
     */
    /* Both fields are bounded and stop at the separator: a %s here would
     * write past the buffer on a long enough stored hash.
     */
    if (sscanf(stored, "$argon2id$v=%u$m=%u,t=%u,p=%u$%127[^$]$%63[^$]", &v,
               &out->m_cost, &out->t_cost, &out->lanes, salt_b64,
               tag_b64) != 6)
        return 0;

    if (v != 19)
        return 0;
    if (!out->m_cost || !out->t_cost || !out->lanes)
        return 0;

    n = b64_decode(out->salt, sizeof(out->salt), salt_b64);
    if (n < 8)
        return 0;
    out->saltlen = (size_t)n;

    n = b64_decode(out->tag, sizeof(out->tag), tag_b64);
    if (n != PWHASH_TAG_LEN)
        return 0;
    out->taglen = (size_t)n;

    return 1;
}

/** Make a stored password.  See pwhash.h. */
int a2_pwhash_make(char* out, const void* pwd, size_t pwdlen, const void* salt,
                   size_t saltlen, const void* secret, size_t secretlen,
                   unsigned int m_cost, unsigned int t_cost)
{
    struct Argon2Params params;
    unsigned char tag[PWHASH_TAG_LEN];
    char salt_b64[PWHASH_SALT_MAX * 2];
    char tag_b64[PWHASH_TAG_LEN * 2];
    int len;

    if (!out || saltlen < 8 || saltlen > PWHASH_SALT_MAX)
        return 0;

    memset(&params, 0, sizeof(params));
    params.secret = secretlen ? secret : 0;
    params.secretlen = secretlen;
    params.m_cost = m_cost ? m_cost : PWHASH_DEFAULT_MEMORY;
    params.t_cost = t_cost ? t_cost : PWHASH_DEFAULT_TIME;
    params.lanes = PWHASH_DEFAULT_LANES;

    if (!a2_argon2id(tag, sizeof(tag), pwd, pwdlen, salt, saltlen, &params))
        return 0;

    if (!b64_encode(salt_b64, sizeof(salt_b64), (const unsigned char*)salt,
                    saltlen) ||
        !b64_encode(tag_b64, sizeof(tag_b64), tag, sizeof(tag))) {
        a2_wipe(tag, sizeof(tag));
        return 0;
    }

    len = snprintf(out, PWHASH_MAX + 1, "$argon2id$v=19$m=%u,t=%u,p=%u$%s$%s",
                   params.m_cost, params.t_cost, params.lanes, salt_b64,
                   tag_b64);

    a2_wipe(tag, sizeof(tag));
    a2_wipe(tag_b64, sizeof(tag_b64));

    return len > 0 && len <= PWHASH_MAX;
}

/** Check a password.  See pwhash.h. */
int a2_pwhash_verify(const char* stored, const void* pwd, size_t pwdlen,
                     const void* secret, size_t secretlen)
{
    struct PwHashFields f;
    struct Argon2Params params;
    unsigned char tag[PWHASH_TAG_LEN];
    int ok;

    if (!pwhash_parse(stored, &f))
        return 0;

    memset(&params, 0, sizeof(params));
    params.secret = secretlen ? secret : 0;
    params.secretlen = secretlen;

    /* The costs come from the stored hash and not from the configuration:
     * that is what lets them be raised without invalidating every password
     * already stored.
     */
    params.m_cost = f.m_cost;
    params.t_cost = f.t_cost;
    params.lanes = f.lanes;

    if (!a2_argon2id(tag, sizeof(tag), pwd, pwdlen, f.salt, f.saltlen,
                     &params)) {
        a2_wipe(&f, sizeof(f));
        return 0;
    }

    ok = a2_equal(tag, f.tag, sizeof(tag));

    a2_wipe(tag, sizeof(tag));
    a2_wipe(&f, sizeof(f));

    return ok;
}

/** Is this hash worth making again?  See pwhash.h. */
int a2_pwhash_outdated(const char* stored, unsigned int m_cost,
                       unsigned int t_cost)
{
    struct PwHashFields f;
    int outdated;

    if (!pwhash_parse(stored, &f))
        return 1; /* unreadable is as good a reason to replace it as any */

    if (!m_cost)
        m_cost = PWHASH_DEFAULT_MEMORY;
    if (!t_cost)
        t_cost = PWHASH_DEFAULT_TIME;

    outdated = (f.m_cost < m_cost) || (f.t_cost < t_cost);

    a2_wipe(&f, sizeof(f));

    return outdated;
}
