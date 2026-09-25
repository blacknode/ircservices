/* High-level encryption routines.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Making and checking a password goes to a worker thread, and the command
 * that asked is run again when the answer is in: see include/encrypt.h.
 *
 * What crosses to the worker is a PwJob, allocated with worker_alloc() and
 * holding only bytes: the plaintext, the stored password, the cipher's
 * settings, and the cipher's functions.  pw_work() below is the only code
 * in this file that runs in a worker thread; it calls those functions and
 * memcmp(), and nothing else.  Everything else runs in the main thread.
 */

#include "encrypt.h"
#include "modules.h"
#include "p10.h"
#include "services.h"
#include "worker.h"

/*************************************************************************/

/* List of available ciphers. */
static CipherInfo* cipherlist;

/* The cipher new passwords are made with when options { encryption } is
 * not set. */
#define DEFAULT_CIPHER "argon2"

/* Longest plaintext carried to a worker.  An IRC line is shorter. */
#define PW_PLAIN_MAX 512

/* How many times one command may be run again for its passwords.  A
 * command checks one password, or makes one; the margin is for a record
 * that changed while the first answer was being worked out. */
#define PW_MAX_DEPTH 3

/*************************************************************************/

/* The work, as it crosses to a worker thread and back. */
typedef struct {
    /* Checking (check_password): the stored password and its cipher's
     * check function, or NULL for a password stored as given. */
    int check;
    int (*check_fn)(const CipherParams*, const char*, int, const char*);
    CipherParams check_params;
    char stored[PASSHASHMAX];

    /* Making (encrypt_password, or a password upgraded after a check
     * that matched): the current cipher's functions, or NULL. */
    int (*encrypt_fn)(const CipherParams*, const char*, int, char*, int);
    int (*outdated_fn)(const CipherParams*, const char*);
    CipherParams make_params;
    int upgrade; /* Check: make a new one if the stored one is weak */

    char plaintext[PW_PLAIN_MAX];
    int len;

    /* The answer. */
    int result;             /* check: 1/0/-1; make: 0/-2/-1 */
    int made;               /* Nonzero if `hash' holds a new password */
    char hash[PASSHASHMAX]; /* The new password */
} PwJob;

/* A command waiting for its password work.  Main thread. */
typedef struct pwpending_ PwPending;
struct pwpending_ {
    PwPending *next, *prev;
    Module* mod;     /* The pseudoclient that ran the command */
    char numeric[6]; /* Who ran it */
    uint32 servicestamp;
    PasswordReplayFn replay;
    char* line;         /* The command, to run again */
    int depth;          /* How many times it has been run again */
    int orphaned;       /* Its module was unloaded */
    PwJob* job;         /* The worker task's input and output */
    char* check_cipher; /* Cipher of the stored password (check) */
    char* make_cipher;  /* Cipher of `job->hash', if made */
};

/* The commands waiting. */
static PwPending* pendinglist;

/* The command being run, between password_command_begin() and _end(). */
static struct {
    int active;
    Module* mod;
    char numeric[6];
    uint32 servicestamp;
    PasswordReplayFn replay;
    char* line;
    int depth;
} current;

/* The finished work, while its command is being run again. */
static PwPending* replaying;
static int replay_used;

/*************************************************************************/
/*************************************************************************/

/* Utility routines. */

static CipherInfo* find_cipher(const char* name)
{
    CipherInfo* ci;

    if (!name)
        return NULL;
    LIST_SEARCH(cipherlist, name, name, strcmp, ci);
    return ci;
}

static const char* default_cipher(void)
{
    return EncryptionType && *EncryptionType ? EncryptionType : DEFAULT_CIPHER;
}

/* A stored password, as a string: never longer than the buffer. */
static void stored_string(const Password* password, char* buf)
{
    memcpy(buf, password->password, PASSHASHMAX);
    buf[PASSHASHMAX - 1] = 0;
}

/* Wipe and free what was allocated for a command waiting. */
static void free_pending(PwPending* p)
{
    if (p->line) {
        memset(p->line, 0, strlen(p->line));
        free(p->line);
    }
    free(p->check_cipher);
    free(p->make_cipher);
    free(p);
}

/*************************************************************************/

/* The only function here that runs in a worker thread. */

static void pw_work(struct WorkTask* task)
{
    PwJob* job = task->wt_in;

    job->made = 0;
    if (job->check) {
        if (job->check_fn) {
            job->result = (*job->check_fn)(&job->check_params, job->plaintext,
                                           job->len, job->stored);
        }
        else {
            /* Stored as given, from before a cipher was required: compared
             * as it always was, the first PASSMAX bytes. */
            job->result = strncmp(job->plaintext, job->stored, PASSMAX) == 0;
        }
        if (job->result != 1 || !job->encrypt_fn || !job->upgrade)
            return;
        if (job->check_fn && job->outdated_fn &&
            !(*job->outdated_fn)(&job->make_params, job->stored))
            return;
        /* It matched and it is weak: make it again now, while the
         * plaintext is known.  A failure here costs nothing; the password
         * that matched is still there. */
        job->made =
            (*job->encrypt_fn)(&job->make_params, job->plaintext, job->len,
                               job->hash, sizeof(job->hash)) == 0;
    }
    else {
        int res = (*job->encrypt_fn)(&job->make_params, job->plaintext,
                                     job->len, job->hash, sizeof(job->hash));
        job->result = res == 0 ? 0 : res > 0 ? -2 : -1;
        job->made = res == 0;
    }
}

/*************************************************************************/

/* Back in the main thread: run the command again. */

static void pw_done(struct WorkTask* task)
{
    PwPending* p = task->wt_arg;
    User* u;
    char* line;
    typeof(current) saved;

    LIST_REMOVE(p, pendinglist);
    p->next = p->prev = NULL;
    if (p->orphaned)
        return;
    u = p10_find_user(p->numeric);
    if (!u || u->servicestamp != p->servicestamp)
        return; /* They are gone; so is the question */

    /* worker_wait() delivers too, and it may be called while some other
     * command is running: that command's context is put aside and given
     * back afterwards. */
    saved = current;
    memset(&current, 0, sizeof(current));
    line = sstrdup(p->line);
    replaying = p;
    replay_used = 0;
    (*p->replay)(u, line);
    replaying = NULL;
    memset(line, 0, strlen(line));
    free(line);
    password_command_end();
    current = saved;
}

static void pw_free(struct WorkTask* task)
{
    PwPending* p = task->wt_arg;
    PwJob* job = task->wt_in;

    if (job) {
        memset(job, 0, sizeof(*job));
        worker_free(job);
        task->wt_in = NULL;
    }
    if (p) {
        if (p->next || p->prev || pendinglist == p)
            LIST_REMOVE(p, pendinglist);
        free_pending(p);
    }
}

/*************************************************************************/

/* Hand `job' to a worker, on behalf of the command being run.  `owner' is
 * the module whose code the job calls (the cipher's), or NULL.  Returns
 * PASSWORD_PENDING, or -1 if it could not be handed over (the job is freed
 * either way). */

static int submit(PwJob* job, Module* owner, const char* check_cipher,
                  const char* make_cipher)
{
    struct WorkTask* task;
    PwPending* p;
    int ok;

    if (current.depth >= PW_MAX_DEPTH) {
        log("encrypt: a command asked for a password %d times; giving up",
            current.depth + 1);
        memset(job, 0, sizeof(*job));
        worker_free(job);
        return -1;
    }

    p = scalloc(1, sizeof(*p));
    p->mod = current.mod;
    memcpy(p->numeric, current.numeric, sizeof(p->numeric));
    p->servicestamp = current.servicestamp;
    p->replay = current.replay;
    p->line = sstrdup(current.line);
    p->depth = current.depth + 1;
    p->job = job;
    p->check_cipher = check_cipher ? sstrdup(check_cipher) : NULL;
    p->make_cipher = make_cipher ? sstrdup(make_cipher) : NULL;

    task = worker_task_new(pw_work, pw_done);
    if (!task) {
        memset(job, 0, sizeof(*job));
        worker_free(job);
        free_pending(p);
        return -1;
    }
    task->wt_in = job;
    task->wt_arg = p;
    task->wt_free = pw_free;
    ok = owner ? worker_submit_owned(owner, task) : worker_submit(task);
    if (!ok) {
        log("encrypt: no worker could take the password work (queue full"
            " or no workers)");
        worker_task_free(task); /* runs pw_free() */
        return -1;
    }
    LIST_INSERT(p, pendinglist);
    return PASSWORD_PENDING;
}

/* Nonzero if the user running the current command already has password
 * work waiting. */

static int user_busy(void)
{
    PwPending* p;

    LIST_FOREACH(p, pendinglist)
    {
        if (!p->orphaned && p->servicestamp == current.servicestamp &&
            strcmp(p->numeric, current.numeric) == 0)
            return 1;
    }
    return 0;
}

/* The finished work for this call, if the command is being run again and
 * this is the call that asked for it; NULL otherwise.  `stored' is NULL
 * for encrypt_password(). */

static PwPending* replay_answer(const char* plaintext, int len,
                                const char* stored, const char* cipher)
{
    PwJob* job;

    if (!replaying || replay_used)
        return NULL;
    job = replaying->job;
    if (!job || job->check != (stored != NULL) || job->len != len ||
        memcmp(job->plaintext, plaintext, len) != 0)
        return NULL;
    if (stored) {
        const char* was = replaying->check_cipher;
        if (strcmp(job->stored, stored) != 0 ||
            (was ? !cipher || strcmp(was, cipher) != 0 : cipher != NULL))
            return NULL; /* The stored password changed meanwhile */
    }
    replay_used = 1;
    return replaying;
}

/* Fill in a job's plaintext.  Returns 0 if it is too long to carry. */

static int job_plaintext(PwJob* job, const char* plaintext, int len)
{
    if (len < 0 || len >= (int)sizeof(job->plaintext))
        return 0;
    memcpy(job->plaintext, plaintext, len);
    job->len = len;
    return 1;
}

/*************************************************************************/
/*************************************************************************/

/* High-level password encryption routines. */

/*************************************************************************/

/* Allocate and return a new, empty Password structure.  Always succeeds
 * (smalloc() will throw a signal if memory cannot be allocated).
 */

Password* new_password(void)
{
    Password* password = smalloc(sizeof(*password));
    init_password(password);
    return password;
}

/*************************************************************************/

/* Initialize a preallocated Password structure.  Identical in behavior to
 * new_password(), except that the passed-in structure is used instead of
 * allocating a new one, and the structure pointer is not returned.
 */

void init_password(Password* password)
{
    memset(password->password, 0, sizeof(password->password));
    password->cipher = NULL;
}

/*************************************************************************/

/* Set the contents of a Password structure to the given values.  If
 * cipher is not NULL, a copy of it is made, so the original string may be
 * disposed of after calling set_password().
 */

void set_password(Password* password, const char password_buffer[PASSHASHMAX],
                  const char* cipher)
{
    memcpy(password->password, password_buffer, PASSHASHMAX);
    if (cipher) {
        password->cipher = sstrdup(cipher);
    }
    else {
        password->cipher = NULL;
    }
}

/*************************************************************************/

/* Copy the contents of a Password structure to another Password structure.
 * The destination password comes first, a la memcpy().
 */

void copy_password(Password* to, const Password* from)
{
    clear_password(to);
    memcpy(to->password, from->password, sizeof(to->password));
    if (from->cipher) {
        to->cipher = sstrdup(from->cipher);
    }
    else {
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

void clear_password(Password* password)
{
    memset(password->password, 0, sizeof(password->password));
    free((char*)password->cipher);
    password->cipher = NULL;
}

/*************************************************************************/

/* Free a Password structure allocated with new_password().  Does nothing
 * if NULL is given.
 */

void free_password(Password* password)
{
    if (password) {
        clear_password(password);
        free(password);
    }
}

/*************************************************************************/

/* Store a password the worker made into `password'. */

static void take_hash(Password* password, const PwJob* job, const char* cipher)
{
    clear_password(password);
    memcpy(password->password, job->hash, sizeof(password->password));
    password->password[sizeof(password->password) - 1] = 0;
    password->cipher = sstrdup(cipher);
}

/*************************************************************************/

/* Encrypt string `plaintext' of length `len', placing the result in
 * `password'.  See encrypt.h for the return values.
 */

int encrypt_password(const char* plaintext, int len, Password* password)
{
    const char* name = default_cipher();
    CipherInfo* ci;
    PwPending* p;
    PwJob* job;

    if ((p = replay_answer(plaintext, len, NULL, NULL)) != NULL) {
        if (p->job->result == 0 && p->job->made && p->make_cipher) {
            take_hash(password, p->job, p->make_cipher);
            return 0;
        }
        clear_password(password);
        return p->job->result == -2 ? -2 : -1;
    }

    if (!(ci = find_cipher(name))) {
        log("encrypt_password(): cipher `%s' not available!", name);
        return -1;
    }
    if (!current.active) {
        log("BUG: encrypt_password() called outside a command");
        return -1;
    }
    if (user_busy())
        return PASSWORD_PENDING; /* dropped: one at a time */

    job = worker_alloc(sizeof(*job));
    if (!job)
        return -1;
    if (!job_plaintext(job, plaintext, len)) {
        worker_free(job);
        return -2;
    }
    job->check = 0;
    job->encrypt_fn = ci->encrypt;
    if (ci->params)
        (*ci->params)(&job->make_params);
    return submit(job, ci->owner, NULL, ci->name);
}

/*************************************************************************/

/* Decrypt `password' into buffer `dest' of length `size'.  Returns:
 *     0 on success
 *    +N if the destination buffer is too small; N is the minimum size
 *       buffer required to hold the decrypted password
 *    -2 if the encryption algorithm does not allow decryption
 *    -1 on other error
 */

int decrypt_password(const Password* password, char* dest, int size)
{
    if (password->cipher) {
        CipherInfo* ci = find_cipher(password->cipher);
        if (!ci) {
            log("decrypt_password(): cipher `%s' not available!",
                password->cipher);
            return -1;
        }
        if (!ci->decrypt)
            return -2;
        return (*ci->decrypt)(password->password, dest, size);
    }
    else {
        /* Stored as given. */
        int passlen;
        for (passlen = 0; passlen < PASSMAX; passlen++) {
            if (!password->password[passlen]) {
                break;
            }
        }
        if (size < passlen + 1) {
            return passlen + 1 - size;
        }
        memset(dest, 0, size);
        memcpy(dest, password->password, passlen);
        return 0;
    }
}

/*************************************************************************/

/* Check an input password `plaintext' against a stored password
 * `password'.  See encrypt.h for the return values.
 */

int check_password(const char* plaintext, Password* password)
{
    char stored[PASSHASHMAX];
    CipherInfo *ci = NULL, *make;
    Module* owner;
    PwPending* p;
    PwJob* job;
    int len = strlen(plaintext);

    stored_string(password, stored);

    if ((p = replay_answer(plaintext, len, stored, password->cipher))) {
        PwJob* done = p->job;
        if (done->result == 1 && done->made && p->make_cipher) {
            take_hash(password, done, p->make_cipher);
            log("encrypt: a stored password was made again with %s",
                p->make_cipher);
        }
        memset(stored, 0, sizeof(stored));
        return done->result;
    }

    if (password->cipher && !(ci = find_cipher(password->cipher))) {
        log("check_password(): cipher `%s' not available!", password->cipher);
        memset(stored, 0, sizeof(stored));
        return -1;
    }
    if (!current.active) {
        if (!ci) {
            /* Stored as given: nothing slow to do. */
            int res = strncmp(plaintext, stored, PASSMAX) == 0;
            memset(stored, 0, sizeof(stored));
            return res;
        }
        log("BUG: check_password() called outside a command");
        memset(stored, 0, sizeof(stored));
        return -1;
    }
    if (user_busy()) {
        memset(stored, 0, sizeof(stored));
        return PASSWORD_PENDING; /* dropped: one at a time */
    }

    job = worker_alloc(sizeof(*job));
    if (!job) {
        memset(stored, 0, sizeof(stored));
        return -1;
    }
    if (!job_plaintext(job, plaintext, len)) {
        /* Longer than any password that can have been set. */
        worker_free(job);
        memset(stored, 0, sizeof(stored));
        return 0;
    }
    job->check = 1;
    memcpy(job->stored, stored, sizeof(job->stored));
    memset(stored, 0, sizeof(stored));
    if (ci) {
        job->check_fn = ci->check_password;
        if (ci->params)
            (*ci->params)(&job->check_params);
    }

    /* A password that matches is made again with the current cipher when
     * it was stored without one, or with a weaker setting of the same
     * cipher.  The job may call into one module only, so a password of
     * some other cipher is left as it is. */
    make = find_cipher(default_cipher());
    owner = ci ? ci->owner : make ? make->owner : NULL;
    if (make && (!ci || ci == make)) {
        job->upgrade = 1;
        job->encrypt_fn = make->encrypt;
        job->outdated_fn = make->outdated;
        if (make->params)
            (*make->params)(&job->make_params);
    }
    return submit(job, owner, ci ? ci->name : NULL,
                  job->upgrade ? make->name : NULL);
}

/*************************************************************************/
/*************************************************************************/

/* The command being run. */

/*************************************************************************/

void password_command_begin(Module* mod, User* u, PasswordReplayFn replay,
                            const char* line)
{
    password_command_end();
    current.active = 1;
    current.mod = mod;
    strbcpy(current.numeric, u->numeric);
    current.servicestamp = u->servicestamp;
    current.replay = replay;
    current.line = sstrdup(line);
    current.depth = replaying ? replaying->depth : 0;
}

/*************************************************************************/

void password_command_end(void)
{
    if (current.line) {
        memset(current.line, 0, strlen(current.line));
        free(current.line);
    }
    memset(&current, 0, sizeof(current));
}

/*************************************************************************/

int password_replaying(void)
{
    return replaying != NULL;
}

/*************************************************************************/

void check_encryption(void)
{
    if (!find_cipher(default_cipher())) {
        log("warning: no password can be set: the cipher `%s' (options {"
            " encryption }) is not loaded; load encryption/%s",
            default_cipher(), default_cipher());
    }
}

/*************************************************************************/

void password_drop_module(Module* mod)
{
    PwPending* p;

    /* The work itself goes on (it belongs to the cipher's module, or to
     * nobody); only the command is not run again. */
    LIST_FOREACH(p, pendinglist)
    {
        if (p->mod == mod)
            p->orphaned = 1;
    }
    if (current.active && current.mod == mod)
        password_command_end();
}

/*************************************************************************/
/*************************************************************************/

/* Cipher registration/unregistration. */

/*************************************************************************/

/* Register a new cipher. */

void register_cipher(CipherInfo* ci)
{
    LIST_INSERT(ci, cipherlist);
}

/*************************************************************************/

/* Unregister a cipher.  Does nothing if the cipher was not registered. */

void unregister_cipher(CipherInfo* ci)
{
    CipherInfo* ci2;
    LIST_FOREACH(ci2, cipherlist)
    {
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
