/* Argon2id and BLAKE2b.  See argon2id.h.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/ircd_argon2.c).
 *
 * RFC 9106 and RFC 7693, written out.  The only constants are BLAKE2b's
 * initialisation vector -- the same eight words as SHA-512, the fractional
 * parts of the square roots of the first eight primes -- and its message
 * permutation; the published test vectors would catch a wrong digit in
 * either (tests/argon2_t.c checks them).
 *
 * Runs in worker threads: this file does not include services.h, and
 * calloc()/free() here are the system's.
 */

#include "argon2id.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------
 * BLAKE2b (RFC 7693)
 * ------------------------------------------------------------------ */

/** Initialisation vector: the fractional parts of the square roots of the
 * first eight primes. */
static const uint64_t blake2b_iv[8] = {
    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL,
    0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL};

/** Message permutation, one row per round. */
static const unsigned char blake2b_sigma[12][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};

/** State of a BLAKE2b in progress. */
struct Blake2bCtx {
    uint64_t h[8];
    uint64_t t[2];
    unsigned char buf[128];
    size_t used;
    size_t outlen;
};

/** Rotate a 64-bit word right. */
#define ROTR64(x, n) (((x) >> (n)) | ((x) << (64 - (n))))

static uint64_t load64(const unsigned char* p)
{
    uint64_t v = 0;
    int i;

    for (i = 7; i >= 0; i--)
        v = (v << 8) | p[i];

    return v;
}

static void store64(unsigned char* p, uint64_t v)
{
    int i;

    for (i = 0; i < 8; i++)
        p[i] = (unsigned char)(v >> (i * 8));
}

static void store32le(unsigned char* p, uint32_t v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

/** The mixing function. */
#define B2B_G(a, b, c, d, x, y)                                               \
    do {                                                                      \
        v[a] = v[a] + v[b] + (x);                                             \
        v[d] = ROTR64(v[d] ^ v[a], 32);                                       \
        v[c] = v[c] + v[d];                                                   \
        v[b] = ROTR64(v[b] ^ v[c], 24);                                       \
        v[a] = v[a] + v[b] + (y);                                             \
        v[d] = ROTR64(v[d] ^ v[a], 16);                                       \
        v[c] = v[c] + v[d];                                                   \
        v[b] = ROTR64(v[b] ^ v[c], 63);                                       \
    } while (0)

/** Absorb one 128-byte block. */
static void blake2b_block(struct Blake2bCtx* ctx, const unsigned char* p,
                          int last)
{
    uint64_t v[16];
    uint64_t m[16];
    int i;

    for (i = 0; i < 16; i++)
        m[i] = load64(p + i * 8);

    for (i = 0; i < 8; i++)
        v[i] = ctx->h[i];
    for (i = 0; i < 8; i++)
        v[8 + i] = blake2b_iv[i];

    v[12] ^= ctx->t[0];
    v[13] ^= ctx->t[1];
    if (last)
        v[14] = ~v[14];

    for (i = 0; i < 12; i++) {
        const unsigned char* s = blake2b_sigma[i];

        B2B_G(0, 4, 8, 12, m[s[0]], m[s[1]]);
        B2B_G(1, 5, 9, 13, m[s[2]], m[s[3]]);
        B2B_G(2, 6, 10, 14, m[s[4]], m[s[5]]);
        B2B_G(3, 7, 11, 15, m[s[6]], m[s[7]]);
        B2B_G(0, 5, 10, 15, m[s[8]], m[s[9]]);
        B2B_G(1, 6, 11, 12, m[s[10]], m[s[11]]);
        B2B_G(2, 7, 8, 13, m[s[12]], m[s[13]]);
        B2B_G(3, 4, 9, 14, m[s[14]], m[s[15]]);
    }

    for (i = 0; i < 8; i++)
        ctx->h[i] ^= v[i] ^ v[8 + i];

    a2_wipe(v, sizeof(v));
    a2_wipe(m, sizeof(m));
}

static void blake2b_init(struct Blake2bCtx* ctx, size_t outlen,
                         const void* key, size_t keylen)
{
    int i;

    memset(ctx, 0, sizeof(*ctx));
    for (i = 0; i < 8; i++)
        ctx->h[i] = blake2b_iv[i];

    /* The parameter block, of which only the first word is ever non-zero
     * here: digest length, key length, fanout 1, depth 1.
     */
    ctx->h[0] ^= 0x01010000ULL ^ ((uint64_t)keylen << 8) ^ (uint64_t)outlen;
    ctx->outlen = outlen;

    if (keylen) {
        unsigned char block[128];

        memset(block, 0, sizeof(block));
        memcpy(block, key, keylen);
        /* A keyed hash begins with the key padded to a whole block. */
        ctx->t[0] = 128;
        blake2b_block(ctx, block, 0);
        a2_wipe(block, sizeof(block));
    }
}

static void blake2b_update(struct Blake2bCtx* ctx, const void* data,
                           size_t len)
{
    const unsigned char* p = (const unsigned char*)data;

    while (len > 0) {
        size_t take;

        /* A full buffer is only absorbed once something follows it: the last
         * block is finalised differently, and we do not know it is the last
         * until we run out of input.
         */
        if (ctx->used == 128) {
            ctx->t[0] += 128;
            if (ctx->t[0] < 128)
                ctx->t[1]++;
            blake2b_block(ctx, ctx->buf, 0);
            ctx->used = 0;
        }

        take = 128 - ctx->used;
        if (take > len)
            take = len;

        memcpy(ctx->buf + ctx->used, p, take);
        ctx->used += take;
        p += take;
        len -= take;
    }
}

static void blake2b_final(struct Blake2bCtx* ctx, unsigned char* out)
{
    unsigned char full[64];
    int i;

    ctx->t[0] += ctx->used;
    if (ctx->t[0] < ctx->used)
        ctx->t[1]++;

    memset(ctx->buf + ctx->used, 0, 128 - ctx->used);
    blake2b_block(ctx, ctx->buf, 1);

    for (i = 0; i < 8; i++)
        store64(full + i * 8, ctx->h[i]);

    memcpy(out, full, ctx->outlen);

    a2_wipe(full, sizeof(full));
    a2_wipe(ctx, sizeof(*ctx));
}

/** BLAKE2b in one call.  See argon2id.h. */
void a2_blake2b(unsigned char* out, size_t outlen, const void* key,
                size_t keylen, const void* data, size_t len)
{
    struct Blake2bCtx ctx;

    blake2b_init(&ctx, outlen, key, keylen);
    blake2b_update(&ctx, data, len);
    blake2b_final(&ctx, out);
}

/* ------------------------------------------------------------------
 * Argon2id (RFC 9106)
 * ------------------------------------------------------------------ */

/** Bytes in one Argon2 block. */
#define ARGON2_BLOCK_LEN 1024
/** 64-bit words in one. */
#define ARGON2_WORDS     (ARGON2_BLOCK_LEN / 8)
/** Slices per pass, fixed by the specification. */
#define ARGON2_SLICES    4
/** The variant this file implements. */
#define ARGON2_TYPE_ID   2
/** The version it implements. */
#define ARGON2_VERSION   0x13

/** Argon2's variable-length hash, H' (RFC 9106, 3.3).
 *
 * Up to 64 bytes it is plain BLAKE2b; beyond that it is a chain, each link
 * giving 32 bytes, which is the part nothing else in the server needs.
 */
static void argon2_hash_long(unsigned char* out, uint32_t outlen,
                             const unsigned char* in, size_t inlen)
{
    unsigned char lenbuf[4];
    struct Blake2bCtx ctx;

    store32le(lenbuf, outlen);

    if (outlen <= 64) {
        blake2b_init(&ctx, outlen, 0, 0);
        blake2b_update(&ctx, lenbuf, 4);
        blake2b_update(&ctx, in, inlen);
        blake2b_final(&ctx, out);
        return;
    }

    {
        unsigned char v[64];
        uint32_t left = outlen;

        blake2b_init(&ctx, 64, 0, 0);
        blake2b_update(&ctx, lenbuf, 4);
        blake2b_update(&ctx, in, inlen);
        blake2b_final(&ctx, v);

        memcpy(out, v, 32);
        out += 32;
        left -= 32;

        while (left > 64) {
            a2_blake2b(v, 64, 0, 0, v, 64);
            memcpy(out, v, 32);
            out += 32;
            left -= 32;
        }

        a2_blake2b(out, left, 0, 0, v, 64);
        a2_wipe(v, sizeof(v));
    }
}

/** The permutation applied to a row and then a column of a block. */
#define A2_G(a, b, c, d)                                                      \
    do {                                                                      \
        (a) = (a) + (b) + 2 * ((a) & 0xffffffffULL) * ((b) & 0xffffffffULL);  \
        (d) = ROTR64((d) ^ (a), 32);                                          \
        (c) = (c) + (d) + 2 * ((c) & 0xffffffffULL) * ((d) & 0xffffffffULL);  \
        (b) = ROTR64((b) ^ (c), 24);                                          \
        (a) = (a) + (b) + 2 * ((a) & 0xffffffffULL) * ((b) & 0xffffffffULL);  \
        (d) = ROTR64((d) ^ (a), 16);                                          \
        (c) = (c) + (d) + 2 * ((c) & 0xffffffffULL) * ((d) & 0xffffffffULL);  \
        (b) = ROTR64((b) ^ (c), 63);                                          \
    } while (0)

/** The permutation P, over sixteen contiguous words.
 *
 * The same shape as BLAKE2b's round -- four mixes down the columns of a
 * 4x4 arrangement, then four along its diagonals -- with Argon2's G in
 * place of BLAKE2b's.  Reference implementations express this over eight
 * 128-bit registers, which is the same sixteen words in pairs; written out
 * in whole words there is nothing to misread.
 */
static void argon2_permute(uint64_t* v)
{
    A2_G(v[0], v[4], v[8], v[12]);
    A2_G(v[1], v[5], v[9], v[13]);
    A2_G(v[2], v[6], v[10], v[14]);
    A2_G(v[3], v[7], v[11], v[15]);
    A2_G(v[0], v[5], v[10], v[15]);
    A2_G(v[1], v[6], v[11], v[12]);
    A2_G(v[2], v[7], v[8], v[13]);
    A2_G(v[3], v[4], v[9], v[14]);
}

/** The compression function G: next = G(prev, ref), optionally xor'd into
 * what was already there (RFC 9106, 3.5). */
static void argon2_fill_block(const uint64_t* prev, const uint64_t* ref,
                              uint64_t* next, int with_xor)
{
    uint64_t r[ARGON2_WORDS];
    uint64_t t[ARGON2_WORDS];
    int i;

    for (i = 0; i < ARGON2_WORDS; i++)
        r[i] = prev[i] ^ ref[i];
    memcpy(t, r, sizeof(t));

    /* The block is an eight by eight arrangement of 128-bit registers: the
     * permutation runs along each row, then down each column.
     */
    for (i = 0; i < 8; i++)
        argon2_permute(t + i * 16);

    for (i = 0; i < 8; i++) {
        uint64_t col[16];
        int r;

        /* Column i is the pair of words at 16*r + 2*i in every row. */
        for (r = 0; r < 8; r++) {
            col[r * 2] = t[r * 16 + i * 2];
            col[r * 2 + 1] = t[r * 16 + i * 2 + 1];
        }

        argon2_permute(col);

        for (r = 0; r < 8; r++) {
            t[r * 16 + i * 2] = col[r * 2];
            t[r * 16 + i * 2 + 1] = col[r * 2 + 1];
        }
    }

    for (i = 0; i < ARGON2_WORDS; i++) {
        uint64_t value = t[i] ^ r[i];

        next[i] = with_xor ? (next[i] ^ value) : value;
    }

    a2_wipe(r, sizeof(r));
    a2_wipe(t, sizeof(t));
}

/** Argon2id.  See argon2id.h. */
int a2_argon2id(unsigned char* out, size_t outlen, const void* pwd,
                size_t pwdlen, const void* salt, size_t saltlen,
                const struct Argon2Params* params)
{
    unsigned int t_cost;
    unsigned int m_cost;
    unsigned int lanes;
    const void* secret;
    size_t secretlen;
    const void* ad;
    size_t adlen;

    unsigned char h0[64 + 8];
    unsigned char lenbuf[4];
    struct Blake2bCtx ctx;
    uint64_t* memory;
    uint32_t blocks;
    uint32_t lane_len;
    uint32_t seg_len;
    uint32_t pass;
    uint32_t lane;
    uint32_t slice;
    uint32_t i;

    if (!params)
        return 0;

    t_cost = params->t_cost;
    m_cost = params->m_cost;
    lanes = params->lanes;
    secret = params->secret;
    secretlen = params->secret ? params->secretlen : 0;
    ad = params->ad;
    adlen = params->ad ? params->adlen : 0;

    if (outlen < 4 || !lanes || !t_cost || saltlen < 8)
        return 0;
    if (m_cost < 8 * lanes)
        m_cost = 8 * lanes;

    /* The memory is trimmed to a whole number of segments, four per lane
     * per pass, which is what the addressing below assumes.
     */
    blocks = m_cost - (m_cost % (ARGON2_SLICES * lanes));
    lane_len = blocks / lanes;
    seg_len = lane_len / ARGON2_SLICES;

    memory = (uint64_t*)calloc((size_t)blocks, ARGON2_BLOCK_LEN);
    if (!memory)
        return 0;

    /* H0: every parameter, then the password, salt, secret and associated
     * data.  Everything that could change the answer goes in, which is what
     * stops a hash made with one cost being accepted as another.
     */
    blake2b_init(&ctx, 64, 0, 0);
    store32le(lenbuf, lanes);
    blake2b_update(&ctx, lenbuf, 4);
    store32le(lenbuf, (uint32_t)outlen);
    blake2b_update(&ctx, lenbuf, 4);
    store32le(lenbuf, m_cost);
    blake2b_update(&ctx, lenbuf, 4);
    store32le(lenbuf, t_cost);
    blake2b_update(&ctx, lenbuf, 4);
    store32le(lenbuf, ARGON2_VERSION);
    blake2b_update(&ctx, lenbuf, 4);
    store32le(lenbuf, ARGON2_TYPE_ID);
    blake2b_update(&ctx, lenbuf, 4);
    store32le(lenbuf, (uint32_t)pwdlen);
    blake2b_update(&ctx, lenbuf, 4);
    blake2b_update(&ctx, pwd, pwdlen);
    store32le(lenbuf, (uint32_t)saltlen);
    blake2b_update(&ctx, lenbuf, 4);
    blake2b_update(&ctx, salt, saltlen);
    store32le(lenbuf, (uint32_t)secretlen);
    blake2b_update(&ctx, lenbuf, 4);
    if (secretlen)
        blake2b_update(&ctx, secret, secretlen);
    store32le(lenbuf, (uint32_t)adlen);
    blake2b_update(&ctx, lenbuf, 4);
    if (adlen)
        blake2b_update(&ctx, ad, adlen);
    blake2b_final(&ctx, h0);

    /* The first two blocks of every lane come straight from H0. */
    for (lane = 0; lane < lanes; lane++) {
        store32le(h0 + 64, 0);
        store32le(h0 + 68, lane);
        argon2_hash_long(
            (unsigned char*)(memory + (size_t)lane * lane_len * ARGON2_WORDS),
            ARGON2_BLOCK_LEN, h0, 72);

        store32le(h0 + 64, 1);
        argon2_hash_long(
            (unsigned char*)(memory +
                             ((size_t)lane * lane_len + 1) * ARGON2_WORDS),
            ARGON2_BLOCK_LEN, h0, 72);
    }

    for (pass = 0; pass < t_cost; pass++) {
        for (slice = 0; slice < ARGON2_SLICES; slice++) {
            for (lane = 0; lane < lanes; lane++) {
                uint64_t pseudo_rand = 0;
                unsigned char zero_block[ARGON2_BLOCK_LEN];
                unsigned char addr_block[ARGON2_BLOCK_LEN];
                unsigned char input_block[ARGON2_BLOCK_LEN];
                /* Argon2id takes its addresses the data-independent way for
                 * the first half of the first pass and the data-dependent way
                 * after: the first protects against an attacker watching
                 * memory access, the second against one trading memory for
                 * time.
                 */
                int independent = (pass == 0 && slice < ARGON2_SLICES / 2);

                memset(zero_block, 0, sizeof(zero_block));
                memset(addr_block, 0, sizeof(addr_block));
                memset(input_block, 0, sizeof(input_block));

                if (independent) {
                    uint64_t* in64 = (uint64_t*)input_block;

                    in64[0] = pass;
                    in64[1] = lane;
                    in64[2] = slice;
                    in64[3] = blocks;
                    in64[4] = t_cost;
                    in64[5] = ARGON2_TYPE_ID;

                    /* The first segment of the first pass starts at index two,
                     * so the "every hundred and twenty-eight" trigger in the
                     * loop never fires for it and its addresses would never be
                     * made.
                     */
                    if (pass == 0 && slice == 0) {
                        in64[6]++;
                        argon2_fill_block((uint64_t*)zero_block,
                                          (uint64_t*)input_block,
                                          (uint64_t*)addr_block, 0);
                        argon2_fill_block((uint64_t*)zero_block,
                                          (uint64_t*)addr_block,
                                          (uint64_t*)addr_block, 0);
                    }
                }

                for (i = (pass == 0 && slice == 0) ? 2 : 0; i < seg_len; i++) {
                    uint32_t index = slice * seg_len + i;
                    uint32_t cur = lane * lane_len + index;
                    uint32_t prev =
                        (index == 0) ? (cur + lane_len - 1) : (cur - 1);
                    uint32_t ref_lane;
                    uint32_t ref_index;
                    uint64_t rand;
                    uint32_t ref_area;
                    uint32_t start;

                    if (independent) {
                        if (i % (ARGON2_BLOCK_LEN / 8) == 0) {
                            uint64_t* in64 = (uint64_t*)input_block;

                            in64[6]++;
                            argon2_fill_block((uint64_t*)zero_block,
                                              (uint64_t*)input_block,
                                              (uint64_t*)addr_block, 0);
                            argon2_fill_block((uint64_t*)zero_block,
                                              (uint64_t*)addr_block,
                                              (uint64_t*)addr_block, 0);
                        }
                        rand = ((
                            uint64_t*)addr_block)[i % (ARGON2_BLOCK_LEN / 8)];
                    }
                    else {
                        rand = memory[(size_t)prev * ARGON2_WORDS];
                    }

                    pseudo_rand = rand;
                    ref_lane = (uint32_t)(pseudo_rand >> 32) % lanes;
                    if (pass == 0 && slice == 0)
                        ref_lane = lane;

                    /* How far back this block may reach: everything already
                     * written in its own lane this pass, plus the finished
                     * slices.
                     */
                    if (pass == 0) {
                        if (ref_lane == lane)
                            ref_area = index - 1;
                        else
                            ref_area = slice * seg_len - ((i == 0) ? 1 : 0);
                        start = 0;
                    }
                    else {
                        if (ref_lane == lane)
                            ref_area = lane_len - seg_len + i - 1;
                        else
                            ref_area = lane_len - seg_len - ((i == 0) ? 1 : 0);
                        start = (slice + 1) * seg_len % lane_len;
                    }

                    {
                        /* The specification's mapping from a random word to an
                         * index, which is quadratic so that recent blocks are
                         * likelier.
                         */
                        uint64_t rel = pseudo_rand & 0xffffffffULL;

                        rel = (rel * rel) >> 32;
                        rel = ref_area - 1 - ((uint64_t)ref_area * rel >> 32);
                        ref_index = (uint32_t)((start + rel) % lane_len);
                    }

                    argon2_fill_block(
                        memory + (size_t)prev * ARGON2_WORDS,
                        memory + ((size_t)ref_lane * lane_len + ref_index) *
                                     ARGON2_WORDS,
                        memory + (size_t)cur * ARGON2_WORDS, pass > 0);
                }

                a2_wipe(addr_block, sizeof(addr_block));
                a2_wipe(input_block, sizeof(input_block));
            }
        }
    }

    /* The tag is the last block of every lane, exclusive-or'd together. */
    {
        uint64_t final[ARGON2_WORDS];

        memcpy(final, memory + ((size_t)lane_len - 1) * ARGON2_WORDS,
               ARGON2_BLOCK_LEN);
        for (lane = 1; lane < lanes; lane++) {
            const uint64_t* last =
                memory +
                ((size_t)lane * lane_len + lane_len - 1) * ARGON2_WORDS;

            for (i = 0; i < ARGON2_WORDS; i++)
                final[i] ^= last[i];
        }

        argon2_hash_long(out, (uint32_t)outlen, (const unsigned char*) final,
                         ARGON2_BLOCK_LEN);
        a2_wipe(final, sizeof(final));
    }

    a2_wipe(memory, (size_t)blocks * ARGON2_BLOCK_LEN);
    free(memory);
    a2_wipe(h0, sizeof(h0));

    return 1;
}

/* ------------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------------ */

/** Compare without leaking where the difference is.  See argon2id.h. */
int a2_equal(const void* a, const void* b, size_t len)
{
    const unsigned char* x = (const unsigned char*)a;
    const unsigned char* y = (const unsigned char*)b;
    unsigned char diff = 0;
    size_t i;

    /* Every byte, every time: the loop must not stop early, which is the
     * whole difference from memcmp().
     */
    for (i = 0; i < len; i++)
        diff |= (unsigned char)(x[i] ^ y[i]);

    return diff == 0;
}

/** Overwrite memory in a way the optimiser may not remove.
 *
 * Through a volatile pointer: a plain memset() on a buffer that is about
 * to go out of scope is a dead store the compiler is entitled to drop.
 */
void a2_wipe(void* p, size_t len)
{
    volatile unsigned char* q = (volatile unsigned char*)p;

    while (len--)
        *q++ = 0;
}
