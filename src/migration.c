/* Migrations: checking what a module ships.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Ported from ircu2 (ircd/migration.c).
 *
 * migration_build() takes the array of files the build embedded and either
 * produces an ordered MigrationSet or says which file is wrong.  It runs
 * while a module is being loaded, and its answer decides whether the module
 * loads at all.  Every rule in migration.h is enforced here rather than in
 * the build: a module whose migrations are malformed should be an error
 * whoever loads it reads, not a build failure on somebody else's machine.
 *
 * Running migrations is migration_run.c.
 */

#include "migration.h"
#include "services.h"

/* Buffer behind the errstr every failure path returns. */
static char migration_error[512];

int migration_reserved_name(const char* name)
{
    return name && 0 == strcmp(name, MIGRATION_CORE);
}

/* Pull the version, name and direction out of one file name.
 *
 * filename Name to parse.
 * version Receives the version.
 * name Receives the migration name, NUL-terminated.
 * is_up Receives non-zero for an up file.
 * Returns Non-zero when filename follows the rules.
 */
static int migration_parse(const char* filename, unsigned int* version,
                           char* name, int* is_up)
{
    const char* p = filename;
    const char* start;
    unsigned long number;
    size_t len;

    if (*p++ != 'v')
        return 0;

    /* The version: digits, at least one, and no leading zero -- "v01" and
     * "v1" would be two spellings of one version, and the table can only
     * hold one of them.
     */
    if (*p < '1' || *p > '9')
        return 0;

    start = p;
    while (*p >= '0' && *p <= '9')
        p++;
    if (p - start > 9)
        return 0; /* absurd, and would overflow below */

    number = strtoul(start, 0, 10);

    if (*p++ != '_')
        return 0;

    /* The name: letters, digits and underscores.  Nothing else, because this
     * goes in the table and comes back out into a reply to an operator.
     */
    start = p;
    while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
           (*p >= '0' && *p <= '9') || *p == '_')
        p++;

    len = (size_t)(p - start);
    if (len < 1 || len > MIGRATION_NAME_LEN)
        return 0;

    if (0 == strcmp(p, ".up.sql"))
        *is_up = 1;
    else if (0 == strcmp(p, ".down.sql"))
        *is_up = 0;
    else
        return 0;

    memcpy(name, start, len);
    name[len] = '\0';

    *version = (unsigned int)number;

    return 1;
}

/* Find or make room for version in set.
 *
 * @param[in,out] set Set being built.
 * version Version wanted.
 * Returns The slot, or NULL if there is no room left.
 */
static struct Migration* migration_slot(struct MigrationSet* set,
                                        unsigned int version)
{
    unsigned int i;

    for (i = 0; i < set->ms_count; i++)
        if (set->ms_list[i].mg_version == version)
            return &set->ms_list[i];

    if (set->ms_count >= MIGRATION_MAX)
        return 0;

    return &set->ms_list[set->ms_count++];
}

/* Order two migrations by version, for qsort(). */
static int migration_compare(const void* a, const void* b)
{
    const struct Migration* ma = (const struct Migration*)a;
    const struct Migration* mb = (const struct Migration*)b;

    if (ma->mg_version < mb->mg_version)
        return -1;
    if (ma->mg_version > mb->mg_version)
        return 1;

    return 0;
}

struct MigrationSet* migration_build(const char* module,
                                     const struct MigrationFile* files,
                                     const char** errstr)
{
    struct MigrationSet* set;
    unsigned int i;

    *errstr = 0;

    /* No migrations at all is the ordinary case, and not an error. */
    if (!files || !files[0].mf_name)
        return 0;

    if (!module || !*module || strlen(module) > MIGRATION_MODULE_LEN) {
        *errstr = "the module name is unusable as a migration owner";
        return 0;
    }

    set = scalloc(1, sizeof(*set));
    set->ms_list = scalloc(MIGRATION_MAX, sizeof(struct Migration));
    strscpy(set->ms_module, module, sizeof(set->ms_module));

    for (i = 0; files[i].mf_name; i++) {
        char name[MIGRATION_NAME_LEN + 1];
        struct Migration* migration;
        unsigned int version;
        int is_up;

        if (!migration_parse(files[i].mf_name, &version, name, &is_up)) {
            snprintf(migration_error, sizeof(migration_error),
                     "migrations/%s is not a migration: the name must be "
                     "v<N>_<name>.up.sql or v<N>_<name>.down.sql, with <N> "
                     "starting at 1 and <name> made of letters, digits and "
                     "underscores",
                     files[i].mf_name);
            *errstr = migration_error;
            migration_free(set);
            return 0;
        }

        if (!(migration = migration_slot(set, version))) {
            snprintf(migration_error, sizeof(migration_error),
                     "too many migrations: at most %u are allowed",
                     (unsigned int)MIGRATION_MAX);
            *errstr = migration_error;
            migration_free(set);
            return 0;
        }

        /* A version already seen must agree with itself about its name: two
         * files claiming v3 under different names are two migrations wearing
         * one number, and only one of them could ever be recorded.
         */
        if (migration->mg_version && 0 != strcmp(migration->mg_name, name)) {
            snprintf(migration_error, sizeof(migration_error),
                     "migrations/%s and v%u_%s.* are both version %u; a "
                     "version is one migration and has one name",
                     files[i].mf_name, version, migration->mg_name, version);
            *errstr = migration_error;
            migration_free(set);
            return 0;
        }

        if ((is_up && migration->mg_up) || (!is_up && migration->mg_down)) {
            snprintf(migration_error, sizeof(migration_error),
                     "migrations/%s is a second %s for version %u",
                     files[i].mf_name, is_up ? "up" : "down", version);
            *errstr = migration_error;
            migration_free(set);
            return 0;
        }

        migration->mg_version = version;
        strscpy(migration->mg_name, name, sizeof(migration->mg_name));

        if (is_up)
            migration->mg_up = files[i].mf_sql;
        else
            migration->mg_down = files[i].mf_sql;
    }

    qsort(set->ms_list, set->ms_count, sizeof(struct Migration),
          migration_compare);

    for (i = 0; i < set->ms_count; i++) {
        struct Migration* migration = &set->ms_list[i];

        /* Versions run 1..N.  A gap means a migration was deleted rather than
         * reverted, and the ones after it would apply out of order on a server
         * that had never seen the missing one.
         */
        if (migration->mg_version != i + 1) {
            snprintf(migration_error, sizeof(migration_error),
                     "migration versions must run from v1 with no gaps; "
                     "v%u is missing",
                     i + 1);
            *errstr = migration_error;
            migration_free(set);
            return 0;
        }

        /* The pairing rule.  A migration that cannot be undone cannot honestly
         * be offered to an operator to apply.
         */
        if (!migration->mg_up || !migration->mg_down) {
            snprintf(migration_error, sizeof(migration_error),
                     "migrations/v%u_%s has no %s file; every up needs its "
                     "down and every down needs its up",
                     migration->mg_version, migration->mg_name,
                     migration->mg_up ? "down" : "up");
            *errstr = migration_error;
            migration_free(set);
            return 0;
        }
    }

    return set;
}

void migration_free(struct MigrationSet* set)
{
    if (!set)
        return;

    free(set->ms_list);
    free(set);
}

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
