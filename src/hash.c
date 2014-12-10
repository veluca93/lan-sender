#include "hash.h"
#include <string.h>
#define _GNU_SOURCE
#include <endian.h>

#define ROL32(w, shift) ((w<<shift) | (w>>(32-shift)))
#define ROR32(w, shift) ((w>>shift) | (w<<(32-shift)))

static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

sha224_ctx_t* sha224_init() {
    sha224_ctx_t* ctx = malloc(sizeof* ctx);
    ctx->hash[0] = 0xC1059ED8;
    ctx->hash[1] = 0x367CD507;
    ctx->hash[2] = 0x3070DD17;
    ctx->hash[3] = 0xF70E5939;
    ctx->hash[4] = 0xFFC00B31;
    ctx->hash[5] = 0x68581511;
    ctx->hash[6] = 0x64F98FA7;
    ctx->hash[7] = 0xBEFA4FA4;
    ctx->total_len = 0;
    ctx->buff_used = 0;
    return ctx;
}

#define S0(a) (ROR32(a, 7) ^ ROR32(a, 18) ^ (a>>3))
#define S1(a) (ROR32(a, 17) ^ ROR32(a, 19) ^ (a>>10))

#define SS0(a) (ROR32(a, 2) ^ ROR32(a, 13) ^ ROR32(a, 22))
#define SS1(a) (ROR32(a, 6) ^ ROR32(a, 11) ^ ROR32(a, 25))
#define CH(a, b, c) (((a) & (b)) | (~(a) & (c)))
#define MAJ(a, b, c) (((a) & (b)) | ((a) & (c)) | ((b) & (c)))

void sha224_process(sha224_ctx_t* ctx) {
    uint32_t w[64];
    uint32_t temp1, temp2;
    uint32_t work[8];
    size_t i;
    memcpy(w, ctx->buff, 64);
    for (i=0; i<16; i++) w[i] = be32toh(w[i]);
    for (i=16; i<64; i++)
        w[i] = w[i-16] + S0(w[i-15]) + w[i-7] + S1(w[i-2]);
    memcpy(work, ctx->hash, 32);
    for (i=0; i<64; i++) {
        temp1 = work[7] + SS1(work[4]) + CH(work[4], work[5], work[6]) + w[i] + k[i];
        temp2 = SS0(work[0]) + MAJ(work[0], work[1], work[2]);
        work[7] = work[6];
        work[6] = work[5];
        work[5] = work[4];
        work[4] = work[3] + temp1;
        work[3] = work[2];
        work[2] = work[1];
        work[1] = work[0];
        work[0] = temp1 + temp2;
    }
    for (i=0; i<8; i++) ctx->hash[i] += work[i];
}
void sha224_update(sha224_ctx_t* ctx, void* _data, size_t dlen) {
    size_t i;
    uint8_t* data = _data;
    for (i=0; i<dlen; i++) {
        ctx->buff[ctx->buff_used++] = data[i];
        ctx->total_len++;
        if(ctx->buff_used == 64) {
            sha224_process(ctx);
            ctx->buff_used = 0;
        }
    }
}
void sha224_finalize(sha224_t* hash, sha224_ctx_t* ctx) {
    uint64_t msglen = ctx->total_len;
    uint8_t one = 0x80;
    sha224_update(ctx, &one, 1);
    size_t i;
    size_t padding = 0;
    size_t sz = ctx->total_len % 64;
    padding = sz<56 ? (56-sz) : (120-sz);
    uint8_t data[64];
    for (i=0; i<padding; i++) data[i] = 0;
    msglen = htobe64(msglen*8);
    memcpy(data+padding, &msglen, sizeof(uint64_t));
    sha224_update(ctx, data, padding+sizeof(uint64_t));
    for (i=0; i<8; i++) ctx->hash[i] = htobe32(ctx->hash[i]);
    memcpy(hash->hash, ctx->hash, 28);
    free(ctx);
}

rollh_ctx_t* rollh_init(size_t window_lenght) {
    size_t i = 0;
    rollh_ctx_t* ctx = malloc(sizeof* ctx);
    ctx->window_lenght = window_lenght;
    ctx->window_pos = 0;
    ctx->window = calloc(window_lenght, sizeof* ctx->window);
    ctx->mult = 1;
    for (i=0; i<window_lenght; i++) ctx->mult *= MULTIPLIER;
    return ctx;
}
void rollh_update(rollh_ctx_t* ctx, void* _data, size_t dlen) {
    uint8_t* data = _data;
    size_t i = 0;
    for (i=0; i<dlen; i++) {
        ctx->hash -= ctx->mult * ctx->window[ctx->window_pos];
        ctx->hash *= MULTIPLIER;
        ctx->hash += data[i];
        ctx->window[ctx->window_pos++] = data[i];
        if (ctx->window_pos == ctx->window_lenght)
            ctx->window_pos = 0;
    }
}
void rollh_finalize(rollh_t* hash, rollh_ctx_t* ctx) {
    free(ctx->window);
    memcpy(hash, &ctx->hash, sizeof* hash);
    free(ctx);
}

hash_ctx_t* hash_init(size_t window_lenght) {
    hash_ctx_t* ctx = malloc(sizeof* ctx);
    ctx->rcontext = rollh_init(window_lenght);
    ctx->scontext = sha224_init();
    return ctx;
}

void hash_update(hash_ctx_t* ctx, void* data, size_t dlen) {
    rollh_update(ctx->rcontext, data, dlen);
    sha224_update(ctx->scontext, data, dlen);
}

void hash_finalize(hash_t* hash, hash_ctx_t* ctx) {
    rollh_finalize(&hash->weak_hash, ctx->rcontext);
    sha224_finalize(&hash->strong_hash, ctx->scontext);
    free(ctx);
}


