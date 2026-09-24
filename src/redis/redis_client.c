/* A Redis connection owned by one thread, used synchronously.
 *
 * IRC Services is copyright (c) 1996-2009 Andrew Church.
 *     E-mail: <achurch@achurch.org>
 * Parts written by Andrew Kempe and others.
 * This program is free but copyrighted software; see the file GPL.txt for
 * details.
 *
 * See redis_client.h.  The connection logic is that of redis_conn.c (ported
 * from ircu2): hiredis, synchronously, with the timeout applied both to
 * the connect and to every reply.
 */

#include "redis_client.h"

#include <hiredis/hiredis.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct RedisClient {
    struct RedisClientConf rc_conf;
    redisContext* rc_ctx;
    struct timespec rc_down_until; /* Failing fast until then */
    char rc_error[160];
};

static void now_mono(struct timespec* ts)
{
    clock_gettime(CLOCK_MONOTONIC, ts);
}

static void set_error(struct RedisClient* client, const char* text)
{
    snprintf(client->rc_error, sizeof(client->rc_error), "%s",
             text ? text : "unknown error");
}

static void client_drop(struct RedisClient* client)
{
    if (client->rc_ctx) {
        redisFree(client->rc_ctx);
        client->rc_ctx = NULL;
    }
}

/* A failure: close the connection and stay down for a moment. */
static void client_fail(struct RedisClient* client, const char* text)
{
    set_error(client, text);
    client_drop(client);
    now_mono(&client->rc_down_until);
    client->rc_down_until.tv_nsec += (long)REDIS_CLIENT_RETRY_MS * 1000000L;
    while (client->rc_down_until.tv_nsec >= 1000000000L) {
        client->rc_down_until.tv_sec++;
        client->rc_down_until.tv_nsec -= 1000000000L;
    }
}

/* Run one command during the connection set-up; nonzero on success. */
static int setup_command(struct RedisClient* client, const char* what,
                         redisReply* reply)
{
    if (!reply || reply->type == REDIS_REPLY_ERROR) {
        char text[160];
        snprintf(text, sizeof(text), "%s: %s", what,
                 reply ? reply->str
                       : client->rc_ctx && client->rc_ctx->errstr[0]
                             ? client->rc_ctx->errstr
                             : "no reply");
        client_fail(client, text);
        if (reply)
            freeReplyObject(reply);
        return 0;
    }
    freeReplyObject(reply);
    return 1;
}

int redis_client_connect(struct RedisClient* client)
{
    const struct RedisClientConf* conf = &client->rc_conf;
    struct timespec now;
    struct timeval tv;

    if (client->rc_ctx)
        return 1;
    now_mono(&now);
    if (now.tv_sec < client->rc_down_until.tv_sec ||
        (now.tv_sec == client->rc_down_until.tv_sec &&
         now.tv_nsec < client->rc_down_until.tv_nsec))
        return 0; /* still down; rc_error says why */

    tv.tv_sec = conf->rcc_timeout_ms / 1000;
    tv.tv_usec = (conf->rcc_timeout_ms % 1000) * 1000;
    if (conf->rcc_socket[0])
        client->rc_ctx = redisConnectUnixWithTimeout(conf->rcc_socket, tv);
    else
        client->rc_ctx =
            redisConnectWithTimeout(conf->rcc_host, conf->rcc_port, tv);
    if (!client->rc_ctx || client->rc_ctx->err) {
        client_fail(client,
                    client->rc_ctx ? client->rc_ctx->errstr : "out of memory");
        return 0;
    }
    /* Both directions: a store that accepts the connection and then goes
     * quiet cannot hold the caller for longer than the timeout. */
    redisSetTimeout(client->rc_ctx, tv);
    if (conf->rcc_password[0] &&
        !setup_command(client, "AUTH failed",
                       redisCommand(client->rc_ctx, "AUTH %s",
                                    conf->rcc_password)))
        return 0;
    if (conf->rcc_database &&
        !setup_command(client, "SELECT failed",
                       redisCommand(client->rc_ctx, "SELECT %d",
                                    conf->rcc_database)))
        return 0;
    return 1;
}

struct RedisClient* redis_client_new(const struct RedisClientConf* conf)
{
    struct RedisClient* client = calloc(1, sizeof(*client));

    if (client)
        client->rc_conf = *conf;
    return client;
}

void redis_client_close(struct RedisClient* client)
{
    if (!client)
        return;
    client_drop(client);
    free(client);
}

const char* redis_client_error(const struct RedisClient* client)
{
    return client->rc_error[0] ? client->rc_error : "no error";
}

void redis_client_free(void* value)
{
    free(value);
}

/* A reply that is not an answer: an I/O failure (the connection is gone)
 * or an error from the store.  Returns nonzero if `reply' is usable. */
static int reply_ok(struct RedisClient* client, redisReply* reply)
{
    if (!reply) {
        client_fail(client, client->rc_ctx ? client->rc_ctx->errstr
                                           : "no connection");
        return 0;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        set_error(client, reply->str);
        freeReplyObject(reply);
        return 0;
    }
    return 1;
}

static char* copy_bytes(const char* data, size_t len)
{
    char* copy = malloc(len + 1);

    if (copy) {
        memcpy(copy, data, len);
        copy[len] = 0;
    }
    return copy;
}

int redis_client_get(struct RedisClient* client, const char* key,
                     char** value, size_t* len)
{
    redisReply* reply;
    int res = 0;

    *value = NULL;
    *len = 0;
    if (!redis_client_connect(client))
        return -1;
    reply = redisCommand(client->rc_ctx, "GET %s", key);
    if (!reply_ok(client, reply))
        return -1;
    if (reply->type == REDIS_REPLY_STRING) {
        if ((*value = copy_bytes(reply->str, reply->len)) != NULL) {
            *len = reply->len;
            res = 1;
        }
        else {
            res = -1;
        }
    }
    freeReplyObject(reply);
    return res;
}

int redis_client_mget(struct RedisClient* client, const char** keys, int n,
                      char** values, size_t* lens)
{
    const char** argv;
    size_t* argl;
    redisReply* reply;
    int i;

    for (i = 0; i < n; i++) {
        values[i] = NULL;
        lens[i] = 0;
    }
    if (n <= 0)
        return 0;
    if (!redis_client_connect(client))
        return -1;
    argv = malloc(sizeof(*argv) * (n + 1));
    argl = malloc(sizeof(*argl) * (n + 1));
    if (!argv || !argl) {
        free(argv);
        free(argl);
        return -1;
    }
    argv[0] = "MGET";
    argl[0] = 4;
    for (i = 0; i < n; i++) {
        argv[i + 1] = keys[i];
        argl[i + 1] = strlen(keys[i]);
    }
    reply = redisCommandArgv(client->rc_ctx, n + 1, argv, argl);
    free(argv);
    free(argl);
    if (!reply_ok(client, reply))
        return -1;
    if (reply->type != REDIS_REPLY_ARRAY || (int)reply->elements != n) {
        set_error(client, "unexpected MGET reply");
        freeReplyObject(reply);
        return -1;
    }
    for (i = 0; i < n; i++) {
        redisReply* r = reply->element[i];
        if (r->type == REDIS_REPLY_STRING &&
            (values[i] = copy_bytes(r->str, r->len)) != NULL)
            lens[i] = r->len;
    }
    freeReplyObject(reply);
    return 0;
}

int redis_client_set(struct RedisClient* client, const char* key,
                     const char* value, size_t len, int ttl)
{
    redisReply* reply;

    if (!redis_client_connect(client))
        return -1;
    /* %b: a value is bytes. */
    if (ttl > 0)
        reply = redisCommand(client->rc_ctx, "SET %s %b EX %d", key, value,
                             len, ttl);
    else
        reply = redisCommand(client->rc_ctx, "SET %s %b", key, value, len);
    if (!reply_ok(client, reply))
        return -1;
    freeReplyObject(reply);
    return 0;
}

int redis_client_mset(struct RedisClient* client, const char** keys,
                      const char** values, const size_t* lens, int n, int ttl)
{
    redisReply* reply;
    int i, res = 0;

    if (n <= 0)
        return 0;
    if (!redis_client_connect(client))
        return -1;
    for (i = 0; i < n; i++) {
        if (ttl > 0)
            redisAppendCommand(client->rc_ctx, "SET %s %b EX %d", keys[i],
                               values[i], lens[i], ttl);
        else
            redisAppendCommand(client->rc_ctx, "SET %s %b", keys[i],
                               values[i], lens[i]);
    }
    for (i = 0; i < n; i++) {
        if (redisGetReply(client->rc_ctx, (void**)&reply) != REDIS_OK) {
            client_fail(client, client->rc_ctx ? client->rc_ctx->errstr
                                               : "no connection");
            return -1;
        }
        if (!reply_ok(client, reply))
            res = -1;
        else
            freeReplyObject(reply);
    }
    return res;
}

int redis_client_del(struct RedisClient* client, const char* key)
{
    redisReply* reply;

    if (!redis_client_connect(client))
        return -1;
    reply = redisCommand(client->rc_ctx, "DEL %s", key);
    if (!reply_ok(client, reply))
        return -1;
    freeReplyObject(reply);
    return 0;
}
