/* Declarations for hash tables.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 */

/* This file allows code to define a simple hash table and associated
 * functions using a simple macro.  After including this file, hash tables
 * can be defined with DEFINE_HASH() or DEFINE_HASH_SCALAR(); the former is
 * for string (char *) keys, the latter for scalar (int, etc.) keys.  The
 * format for these macros is:
 *    DEFINE_HASH[_SCALAR](name, type, keyfield [, keytype])
 * where:
 *    `name'     is the name to be used in the hash table functions,
 *    `type'     is the type of the elements to be stored in the hash table
 *                  (e.g. `struct mystruct' or `MyType'); this must be a
 *                  structured type,
 *    `keyfield' is the field in `type' containing the key to be used to
 *                  index the element, and
 *    `keytype'  (only for DEFINE_HASH_SCALAR) is the type of `keyfield'.
 *
 * The default hash function:
 *    - if HASH_SORTED is defined to a nonzero value, converts the first
 *      two bytes of the value of `keyfield' to a pair of five-bit values,
 *      for a total of 1024 slots, with the conversion controlled by the
 *      `hashlookup' array (defined static const by this file);
 *    - if HASH_SORTED is not defined or is defined to 0, treats `keyfield'
 *      as a string and generates a hash key from the entire string,
 *      hashing into a table of 65537 slots.
 * If you want a different hash function, redefine HASHFUNC and HASHSIZE
 * (see below) before defining the hash table; in particular, scalar keys
 * cannot be used with the default hash function, and require a different
 * hash function to be defined.
 *
 * The functions defined by this file are as follows (the strings `<name>',
 * `<type>', and `<keytype>' refer to the corresponding parameters given to
 * the DEFINE_HASH[_SCALAR] macro):
 *
 *    void add_<name>(<type> *node)
 *       Adds the given node to the hash table.
 *
 *    void del_<name>(<type> *node)
 *       Removes the given node from the hash table.
 *
 *    <type> *get_<name>(const char *key)  (for string keys)
 *    <type> *get_<name>(<keytype> key)    (for scalar keys)
 *       If an element with the given key is stored in the hash table,
 *       returns a pointer to that element; otherwise, returns NULL.
 *
 *    <type> *first_<name>(void)
 *    <type> *next_<name>(void)
 *       These functions iterate over all elements in the hash table.
 *       For hashes with string keys, elements are returned in lexical
 *       order by key if HASH_SORTED is defined (see below).
 *       first_<name>() initializes the iterator to the first element in
 *       the hash table and returns it; next_<name>() returns subsequent
 *       elements, one at a time, until all elements have been returned, at
 *       which point it returns NULL until first_<name>() is called again.
 *       If there are no elements in the hash table, first_<name>() will
 *       return NULL (as will next_<name>()).  It is safe to delete
 *       elements, including the current element, while iterating.  If an
 *       element is added while iterating, it is undefined whether that
 *       element will be returned by next_<name>() before the end of the
 *       hash table is reached.
 *
 * This file and the hash tables it generates are controlled by the
 * following #define's:
 *    EXPIRE_CHECK(node): Define to an expression used to check whether a
 *                        given entry has expired.  Initially defined to
 *                        `0' (no expiration) if not otherwise defined when
 *                        this file is included.
 *    HASH_STATIC: Define to either `static' or nothing, to control whether
 *                 hash functions are declared static or not.  Defaults to
 *                 nothing (functions are not static).  The hash table
 *                 structures themselves are always static.
 *    HASHFUNC/ Define these if you want your own hash function.  Defaults
 *    HASHSIZE: to the standard hash function.  If you redefine these once,
 *              you can restore them to the defaults by defining them to
 *              DEFAULT_HASHFUNC(key) and DEFAULT_HASHSIZE.
 *    HASH_SORTED: Define (to a nonzero value) or undefine _before_
 *                 including this file_ to select the type of default hash
 *                 function (see above).
 *
 * Note that the above defines (except HASH_SORTED) take effect when hash
 * tables are actually defined, not when this file is included, so the
 * defines can be changed at will for different hashes.
 */

#ifndef HASH_H
#define HASH_H

/*************************************************************************/

#ifndef EXPIRE_CHECK
# define EXPIRE_CHECK(node) 0
#endif

#ifndef HASH_STATIC
# define HASH_STATIC /*nothing*/
#endif

/*************************************************************************/

/* Generic hash function and associated lookup table. */

#if HASH_SORTED

# define DEFAULT_HASHFUNC(key) \
    (__hashlookup[(uint8)(*(key))]<<5 \
     | (*(key) ? __hashlookup[(uint8)((key)[1])] : 0))
# define DEFAULT_HASHSIZE 1024
static const uint8 __hashlookup[256] = {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
    16,17,18,19,20,21,22,23,24,25,26,27,28,29, 0, 0,
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
    16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,30,

    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,

    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
    31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
};

#else  /* !HASH_SORTED */

# define DEFAULT_HASHFUNC(key) (__default_hashfunc(key))
# define DEFAULT_HASHSIZE 65537
static const uint8 __hashlookup_unsorted[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
    0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,

    0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x5E,0x5F,
    0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,
    0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
    0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
    0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,

    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,
    0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,
    0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,

    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
    0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
    0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
    0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,
    0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
};
static inline int __default_hashfunc(const char *key_) {
    const uint8 *key = (const uint8 *)key_;
    int hash = 0;
    while (*key) {
        hash = (hash*31 + __hashlookup_unsorted[*key]) % DEFAULT_HASHSIZE;
        key++;
    }
    return hash;
}

#endif  /* HASH_SORTED */


#ifndef HASHFUNC
# define HASHFUNC(key) DEFAULT_HASHFUNC(key)
#endif

#ifndef HASHSIZE
# define HASHSIZE DEFAULT_HASHSIZE
#endif

/*************************************************************************/

/* Templates for creating hash table definitions and functions.
 * HASH_STATIC should be defined to either `static' or nothing beforehand.
 */

#undef DEFINE_HASHTABLE
#define DEFINE_HASHTABLE(name, type) \
    static type *hashtable_##name[HASHSIZE];

#undef DEFINE_HASH_ITER
#define DEFINE_HASH_ITER(name,type)                                     \
    static int hashpos_##name;                                          \
    static type *hashiter_##name;                                       \
    static void _next_##name(void)                                      \
    {                                                                   \
        if (hashiter_##name)                                            \
            hashiter_##name = hashiter_##name->next;                    \
        while (hashiter_##name == NULL && ++hashpos_##name < HASHSIZE)  \
            hashiter_##name = hashtable_##name[hashpos_##name];         \
    }                                                                   \
    HASH_STATIC type *next_##name(void)                                 \
    {                                                                   \
        type *retval;                                                   \
        do {                                                            \
            retval = hashiter_##name;                                   \
            if (retval == NULL)                                         \
                return NULL;                                            \
            _next_##name();                                             \
        } while (!noexpire && EXPIRE_CHECK(retval));                    \
        return retval;                                                  \
    }                                                                   \
    HASH_STATIC type *first_##name(void)                                \
    {                                                                   \
        hashpos_##name = -1;                                            \
        hashiter_##name = NULL;                                         \
        _next_##name();                                                 \
        return next_##name();                                           \
    }

#undef DEFINE_HASH_ADD
#define DEFINE_HASH_ADD(name,type,keyfield)                             \
    HASH_STATIC void add_##name(type *node)                             \
    {                                                                   \
        type **listptr = &hashtable_##name[HASHFUNC(node->keyfield)];   \
        LIST_INSERT_ORDERED(node, *listptr, irc_stricmp, keyfield);     \
    }

#undef DEFINE_HASH_ADD_SCALAR
#define DEFINE_HASH_ADD_SCALAR(name,type,keyfield)                      \
    HASH_STATIC void add_##name(type *node)                             \
    {                                                                   \
        type **listptr = &hashtable_##name[HASHFUNC(node->keyfield)];   \
        LIST_INSERT(node, *listptr);                                    \
    }

#undef DEFINE_HASH_DEL
#define DEFINE_HASH_DEL(name,type,keyfield)                             \
    HASH_STATIC void del_##name(type *node)                             \
    {                                                                   \
        if (node == hashiter_##name)                                    \
            _next_##name();                                             \
        LIST_REMOVE(node, hashtable_##name[HASHFUNC(node->keyfield)]);  \
    }

#undef DEFINE_HASH_GET
#define DEFINE_HASH_GET(name,type,keyfield)                             \
    HASH_STATIC type *get_##name(const char *what)                      \
    {                                                                   \
        type *result;                                                   \
        LIST_SEARCH_ORDERED(hashtable_##name[HASHFUNC(what)],           \
                            keyfield, what, irc_stricmp, result);       \
        if (result && !noexpire && EXPIRE_CHECK(result))                \
            result = NULL;                                              \
        return result;                                                  \
    }

#undef DEFINE_HASH_GET_SCALAR
#define DEFINE_HASH_GET_SCALAR(name,type,keyfield,keytype)              \
    HASH_STATIC type *get_##name(keytype what)                          \
    {                                                                   \
        type *result;                                                   \
        LIST_SEARCH_SCALAR(hashtable_##name[HASHFUNC(what)],            \
                           keyfield, what, result);                     \
        if (result && !noexpire && EXPIRE_CHECK(result))                \
            result = NULL;                                              \
        return result;                                                  \
    }

/* Macros to create everything at once for a given type.
 *      name: Name to use in the hash functions (the XXX in add_XXX, etc.)
 *      type: Type of the hash (NickInfo, etc.)
 *      keyfield: Key field in given type
 */

#undef DEFINE_HASH
#define DEFINE_HASH(name,type,keyfield)                 \
    DEFINE_HASHTABLE(name, type)                        \
    DEFINE_HASH_ITER(name, type)                        \
    DEFINE_HASH_ADD(name, type, keyfield)               \
    DEFINE_HASH_DEL(name, type, keyfield)               \
    DEFINE_HASH_GET(name, type, keyfield)

#undef DEFINE_HASH_SCALAR
#define DEFINE_HASH_SCALAR(name,type,keyfield,keytype)  \
    DEFINE_HASHTABLE(name, type)                        \
    DEFINE_HASH_ITER(name, type)                        \
    DEFINE_HASH_ADD_SCALAR(name, type, keyfield)        \
    DEFINE_HASH_DEL(name, type, keyfield)               \
    DEFINE_HASH_GET_SCALAR(name, type, keyfield, keytype)

/*************************************************************************/

#endif  /* HASH_H */

/*
 * Local variables:
 *   c-file-style: "stroustrup"
 *   c-file-offsets: ((case-label . *) (statement-case-intro . *))
 *   indent-tabs-mode: nil
 * End:
 *
 * vim: expandtab shiftwidth=4:
 */
