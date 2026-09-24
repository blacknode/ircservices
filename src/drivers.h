/* The database and cache drivers built into the core.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * Core-private.  The drivers' own headers (postgres/postgres.h,
 * redis/redis.h) pull in libpq, jansson and hiredis, and are meant for the
 * drivers' files; init() only needs to switch them on.
 */

#ifndef DRIVERS_H
#define DRIVERS_H

/* Register the PostgreSQL driver behind db.h (src/postgres/postgres.c). */
extern int pg_driver_init(void);

/* Register the Redis driver behind cache.h (src/redis/redis.c). */
extern int redis_driver_init(void);

#endif /* DRIVERS_H */
