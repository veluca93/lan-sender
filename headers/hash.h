#ifndef LS_HASH_H
#define LS_HASH_H
#include <stdint.h>
#include <stdlib.h>

#define MULTIPLIER 22695477

typedef struct __attribute__ ((__packed__)) sha224 {
    uint8_t hash[28];
} sha224_t;

typedef struct __attribute__ ((__packed__)) rollh {
    uint8_t hash[4];
} rollh_t;

typedef struct __attribute__ ((__packed__)) hash {
    rollh_t weak_hash;
    sha224_t strong_hash;
} hash_t;

typedef struct sha224_ctx {
    uint32_t hash[8];
    uint8_t buff[64];
    size_t buff_used;
    size_t total_len;
} sha224_ctx_t;

sha224_ctx_t* sha224_init();
void sha224_update(sha224_ctx_t* ctx, void* data, size_t dlen);
void sha224_finalize(sha224_t* hash, sha224_ctx_t* ctx);

typedef struct rollh_ctx {
    uint32_t hash;
    size_t window_lenght;
    size_t window_pos;
    uint8_t* window; 
    size_t mult;
} rollh_ctx_t;

rollh_ctx_t* rollh_init(size_t window_length);
void rollh_update(rollh_ctx_t* ctx, void* data, size_t dlen);
void rollh_finalize(rollh_t* hash, rollh_ctx_t* ctx);

typedef struct hash_ctx {
    rollh_ctx_t* rcontext;
    sha224_ctx_t* scontext;
} hash_ctx_t;

hash_ctx_t* hash_init(size_t window_lenght);
void hash_update(hash_ctx_t* ctx, void* data, size_t dlen);
void hash_finalize(hash_t* hash, hash_ctx_t* ctx);

#endif
