/* Storing a password, and checking one.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (include/ircd_pwhash.h).
 *
 * A stored password is one string that carries everything needed to check
 * it again: the algorithm, its cost, the salt and the tag.
 *
 *     $argon2id$v=19$m=65536,t=3,p=1$c29tZSBzYWx0$Vi1lIGhhc2ggZ29lcyBoZXJl
 *
 * The costs travel with the hash and are not read from the configuration
 * when checking, which is what lets them be raised later: an old password
 * keeps verifying under the cost it was made with, and a2_pwhash_outdated()
 * says when it is worth making again.
 *
 * ALL THREE CALLS HERE ARE PURE.  They read no Services state, allocate
 * nothing that outlives them, and touch no User.  That is deliberate and
 * it is the point: hashing a password takes between fifty and two hundred
 * and fifty milliseconds on purpose.  They are called from a worker thread
 * (see worker.h), with the password copied in and the result copied out.
 */

#ifndef PWHASH_H
#define PWHASH_H

#include <stddef.h>

/* Longest encoded password hash, not counting the terminator: what fits
 * in a Password (PASSHASHMAX in defs.h, which counts it). */
#define PWHASH_MAX 255

/* Default cost: memory in kibibytes, passes, lanes.  OWASP's recommended
 * minimum for Argon2id (19 MiB, 2 passes): about a tenth of a second with
 * this portable implementation.  ircu2 uses 64 MiB and 3 passes, which
 * here takes over half a second -- a worker thread busy that long for
 * every IDENTIFY; raise them in the module block if the network can
 * afford it. */
#define PWHASH_DEFAULT_MEMORY 19456
#define PWHASH_DEFAULT_TIME   2
#define PWHASH_DEFAULT_LANES  1

/* Make a stored password into `out' (PWHASH_MAX+1 bytes) from `pwdlen'
 * bytes of `pwd', with `saltlen' random bytes of `salt' (at least 8,
 * different for every password: the caller supplies them), the pepper
 * `secret' (NULL for none) and the costs `m_cost' (kibibytes) and `t_cost'
 * (passes), 0 for the defaults.  Returns nonzero on success. */
extern int a2_pwhash_make(char* out, const void* pwd, size_t pwdlen,
                          const void* salt, size_t saltlen, const void* secret,
                          size_t secretlen, unsigned int m_cost,
                          unsigned int t_cost);

/* Check `pwdlen' bytes of `pwd' against the stored hash `stored', with
 * the pepper `secret'.  Returns nonzero if it matches.  A stored hash this
 * module cannot parse never matches, rather than matching everything. */
extern int a2_pwhash_verify(const char* stored, const void* pwd, size_t pwdlen,
                            const void* secret, size_t secretlen);

/* Nonzero if `stored' was made with a cost below `m_cost'/`t_cost' (0 for
 * the defaults), so that it is worth making again the next time the
 * password is known.  An unreadable hash is outdated. */
extern int a2_pwhash_outdated(const char* stored, unsigned int m_cost,
                              unsigned int t_cost);

#endif /* PWHASH_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
