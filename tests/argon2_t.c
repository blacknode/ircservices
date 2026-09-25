/* argon2_t.c - Test encryption/argon2 against the vectors the standards
 * publish.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/test/crypto_t.c).  BLAKE2b's initialisation
 * vector and message permutation are constants fully determined by their
 * definition, and a wrong digit in either produces something that still
 * compiles, still runs, and still gives consistent-looking output.  The
 * published vectors are the only thing that can tell the difference.
 *
 * Sources: RFC 7693 (BLAKE2b), RFC 9106 (Argon2id).
 */

#include "argon2id.h"
#include "pwhash.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/** Render \a len bytes of \a p as hex into \a out. */
static const char* hex(const unsigned char* p, size_t len)
{
    static char out[256];
    size_t i;

    for (i = 0; i < len && i * 2 + 2 < sizeof(out); i++)
        sprintf(out + i * 2, "%02x", p[i]);

    return out;
}

static void check(const char* what, const unsigned char* got, size_t len,
                  const char* want)
{
    if (strcmp(hex(got, len), want)) {
        printf("FAIL %s\n  got  %s\n  want %s\n", what, hex(got, len), want);
        assert(0);
    }
}

/** The comparison that must not say where it stopped. */
static void test_equal(void)
{
    unsigned char a[16];
    unsigned char b[16];

    memset(a, 0x5a, sizeof(a));
    memcpy(b, a, sizeof(b));

    assert(a2_equal(a, b, sizeof(a)));

    b[0] ^= 1;
    assert(!a2_equal(a, b, sizeof(a)));
    b[0] ^= 1;

    b[sizeof(b) - 1] ^= 0x80;
    assert(!a2_equal(a, b, sizeof(a)));

    assert(a2_equal(a, b, 0) && "nothing equals nothing");

    printf("ok - constant-time comparison\n");
}

/** BLAKE2b, the vectors of RFC 7693 and the reference implementation. */
static void test_blake2b(void)
{
    unsigned char d[BLAKE2B_DIGEST_LEN];

    a2_blake2b(d, 64, 0, 0, "", 0);
    check("blake2b(\"\")", d, sizeof(d),
          "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419"
          "d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce");

    a2_blake2b(d, 64, 0, 0, "abc", 3);
    check("blake2b(\"abc\")", d, sizeof(d),
          "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d1"
          "7d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923");

    printf("ok - BLAKE2b (RFC 7693)\n");
}

/** Argon2id, the test vector of RFC 9106. */
static void test_argon2id(void)
{
    unsigned char pwd[32];
    unsigned char salt[16];
    unsigned char secret[8];
    unsigned char ad[12];
    unsigned char tag[32];
    struct Argon2Params p;

    memset(pwd, 0x01, sizeof(pwd));
    memset(salt, 0x02, sizeof(salt));
    memset(secret, 0x03, sizeof(secret));
    memset(ad, 0x04, sizeof(ad));

    memset(&p, 0, sizeof(p));
    p.secret = secret;
    p.secretlen = sizeof(secret);
    p.ad = ad;
    p.adlen = sizeof(ad);
    p.t_cost = 3;
    p.m_cost = 32;
    p.lanes = 4;

    assert(a2_argon2id(tag, sizeof(tag), pwd, sizeof(pwd), salt, sizeof(salt),
                       &p));
    check("argon2id RFC 9106", tag, sizeof(tag),
          "0d640df58d78766c08c037a34a8b53c9d01ef0452d75b65eb52520e96b01e659");

    printf("ok - Argon2id (RFC 9106)\n");
}

/** What the hash is actually for: the same password twice, and a wrong
 * one never.
 */
static void test_argon2_behaviour(void)
{
    unsigned char a[32], b[32], c[32];
    const char* salt = "a salt long enough";
    struct Argon2Params q;

    memset(&q, 0, sizeof(q));
    q.t_cost = 2;
    q.m_cost = 64;
    q.lanes = 1;

    assert(a2_argon2id(a, sizeof(a), "correct horse", 13, salt, strlen(salt),
                       &q));
    assert(a2_argon2id(b, sizeof(b), "correct horse", 13, salt, strlen(salt),
                       &q));
    assert(a2_equal(a, b, sizeof(a)) && "not deterministic");

    assert(a2_argon2id(c, sizeof(c), "correct horsf", 13, salt, strlen(salt),
                       &q));
    assert(!a2_equal(a, c, sizeof(a)));

    /* A different salt is a different hash: two people with one password do
     * not get one entry an attacker can crack once.
     */
    assert(a2_argon2id(c, sizeof(c), "correct horse", 13, "another salt!", 13,
                       &q));
    assert(!a2_equal(a, c, sizeof(a)));

    /* And so is a different cost, so a hash cannot be replayed as a cheaper
     * one.
     */
    q.t_cost = 3;
    assert(a2_argon2id(c, sizeof(c), "correct horse", 13, salt, strlen(salt),
                       &q));
    assert(!a2_equal(a, c, sizeof(a)));
    q.t_cost = 2;
    q.m_cost = 128;
    assert(a2_argon2id(c, sizeof(c), "correct horse", 13, salt, strlen(salt),
                       &q));
    assert(!a2_equal(a, c, sizeof(a)));
    q.m_cost = 64;

    /* The pepper changes the answer, which is the whole of what it is for:
     * the same password and salt hash differently on a server that has one.
     */
    q.secret = "a server-wide key";
    q.secretlen = 17;
    assert(a2_argon2id(c, sizeof(c), "correct horse", 13, salt, strlen(salt),
                       &q));
    assert(!a2_equal(a, c, sizeof(a)));
    q.secret = 0;
    q.secretlen = 0;

    /* Parameters that would weaken it are refused rather than adjusted. */
    assert(!a2_argon2id(a, sizeof(a), "x", 1, "short", 5, &q));
    q.t_cost = 0;
    assert(!a2_argon2id(a, sizeof(a), "x", 1, salt, strlen(salt), &q));
    q.t_cost = 2;
    q.lanes = 0;
    assert(!a2_argon2id(a, sizeof(a), "x", 1, salt, strlen(salt), &q));
    q.lanes = 1;
    assert(!a2_argon2id(a, 2, "x", 1, salt, strlen(salt), &q));
    assert(!a2_argon2id(a, sizeof(a), "x", 1, salt, strlen(salt), 0));

    printf("ok - Argon2id behaves like a password hash\n");
}

/** A stored password: what it looks like, and what it does. */
static void test_pwhash(void)
{
    char stored[PWHASH_MAX + 1];
    char other[PWHASH_MAX + 1];
    const char* salt = "sixteen bytes!!!";
    const char* pepper = "a server-wide key";

    /* Cheap costs: this is testing the wrapping, not the hash, and the
     * defaults take a fifth of a second each on purpose.
     */
    assert(
        a2_pwhash_make(stored, "hunter2", 7, salt, strlen(salt), 0, 0, 64, 2));

    assert(0 == strncmp(stored, "$argon2id$v=19$m=64,t=2,p=1$", 28));
    assert(strlen(stored) <= PWHASH_MAX);

    assert(a2_pwhash_verify(stored, "hunter2", 7, 0, 0));
    assert(!a2_pwhash_verify(stored, "hunter3", 7, 0, 0));
    assert(!a2_pwhash_verify(stored, "hunter2", 6, 0, 0));
    assert(!a2_pwhash_verify(stored, "", 0, 0, 0));

    /* Two people with one password do not get one entry to crack. */
    assert(a2_pwhash_make(other, "hunter2", 7, "another salt!!!!", 16, 0, 0,
                          64, 2));
    assert(strcmp(stored, other) != 0);
    assert(a2_pwhash_verify(other, "hunter2", 7, 0, 0));

    /* The pepper has to match too: a stolen database without the server's
     * configuration is not enough to start guessing against.
     */
    assert(a2_pwhash_make(stored, "hunter2", 7, salt, strlen(salt), pepper,
                          strlen(pepper), 64, 2));
    assert(a2_pwhash_verify(stored, "hunter2", 7, pepper, strlen(pepper)));
    assert(!a2_pwhash_verify(stored, "hunter2", 7, 0, 0));
    assert(!a2_pwhash_verify(stored, "hunter2", 7, "wrong key", 9));

    printf("ok - a stored password: %.44s...\n", stored);
}

/** A stored hash this server cannot read must never match.
 *
 * The failure that matters: a parser that gave up and said yes would let
 * anybody in with any password, and it would look like it was working.
 */
static void test_pwhash_refuses_nonsense(void)
{
    static const char* nonsense[] = {
        "",
        "not a hash at all",
        "$argon2id$",
        "$argon2i$v=19$m=64,t=2,p=1$c2FsdHNhbHQ$aGFzaA",  /* wrong variant */
        "$argon2id$v=16$m=64,t=2,p=1$c2FsdHNhbHQ$aGFzaA", /* wrong version */
        "$argon2id$v=19$m=0,t=2,p=1$c2FsdHNhbHQ$aGFzaA",  /* no cost */
        "$argon2id$v=19$m=64,t=0,p=1$c2FsdHNhbHQ$aGFzaA",
        "$argon2id$v=19$m=64,t=2,p=0$c2FsdHNhbHQ$aGFzaA",
        "$argon2id$v=19$m=64,t=2,p=1$c2hvcnQ$aGFzaA",    /* salt too short */
        "$argon2id$v=19$m=64,t=2,p=1$c2FsdHNhbHQ$short", /* tag wrong size */
        "$argon2id$v=19$m=64,t=2,p=1$!!!notbase64!!!$aGFzaA",
        0};
    int i;

    for (i = 0; nonsense[i]; i++)
        assert(!a2_pwhash_verify(nonsense[i], "hunter2", 7, 0, 0));

    assert(!a2_pwhash_verify(0, "hunter2", 7, 0, 0));

    /* Truncating a real one must not help either. */
    {
        char stored[PWHASH_MAX + 1];
        size_t len;

        assert(a2_pwhash_make(stored, "hunter2", 7, "sixteen bytes!!!", 16, 0,
                              0, 64, 2));
        for (len = strlen(stored); len > 0; len--) {
            stored[len - 1] = '\0';
            assert(!a2_pwhash_verify(stored, "hunter2", 7, 0, 0));
        }
    }

    printf("ok - a hash that cannot be read never matches\n");
}

/** Costs can be raised, and old passwords keep working. */
static void test_pwhash_costs(void)
{
    char stored[PWHASH_MAX + 1];

    assert(a2_pwhash_make(stored, "hunter2", 7, "sixteen bytes!!!", 16, 0, 0,
                          64, 2));

    /* Verifying uses the costs in the hash, not today's: raising them does
     * not lock everybody out.
     */
    assert(a2_pwhash_verify(stored, "hunter2", 7, 0, 0));

    assert(a2_pwhash_outdated(stored, 128, 2));
    assert(a2_pwhash_outdated(stored, 64, 3));
    assert(!a2_pwhash_outdated(stored, 64, 2));
    assert(!a2_pwhash_outdated(stored, 32, 1));

    /* And something unreadable is worth replacing whatever the costs. */
    assert(a2_pwhash_outdated("rubbish", 64, 2));

    printf("ok - costs travel with the hash and can be raised\n");
}

int main(void)
{
    test_equal();
    test_blake2b();
    test_argon2id();
    test_argon2_behaviour();
    test_pwhash();
    test_pwhash_refuses_nonsense();
    test_pwhash_costs();
    printf("all argon2 tests passed\n");
    return 0;
}
