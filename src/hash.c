#include "hash.h"
#include <string.h>
#define _GNU_SOURCE
#include <endian.h>
#include <stdio.h>

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

#define ROR32(w, shift) ((w>>shift) | (w<<(32-shift)))

#define S0(a) (ROR32(a, 7) ^ ROR32(a, 18) ^ (a>>3))
#define S1(a) (ROR32(a, 17) ^ ROR32(a, 19) ^ (a>>10))

#define W(I) ( tm = S1(w[(I-2)&0x0f]) + w[(I-7)&0x0f]  \
                  + S0(w[(I-15)&0x0f]) + w[I&0x0f]     \
                , w[I&0x0f] = tm )

#define SS0(a) (ROR32(a, 2) ^ ROR32(a, 13) ^ ROR32(a, 22))
#define SS1(a) (ROR32(a, 6) ^ ROR32(a, 11) ^ ROR32(a, 25))
#define CH(a, b, c) ((c) ^ ((a) & ((b) ^ (c))))
#define MAJ(a, b, c) (((a) & (b)) | ((c) & ((a) | (b))))
#define R(A, B, C, D, E, F, G, H, V, K) { \
    temp1 = H + SS1(E) + CH(E, F, G) + V + K; \
    temp2 = SS0(A) + MAJ(A, B, C); \
    D += temp1; \
    H = temp1 + temp2; \
}

void sha224_process(uint8_t* buff, size_t len, sha224_ctx_t* ctx) {
    register uint32_t a, b, c, d, e, f, g, h;
    uint8_t* bend = buff + len;
    uint32_t temp1, temp2, tm;
    uint32_t w[16];
    size_t i;
    a = ctx->hash[0];
    b = ctx->hash[1];
    c = ctx->hash[2];
    d = ctx->hash[3];
    e = ctx->hash[4];
    f = ctx->hash[5];
    g = ctx->hash[6];
    h = ctx->hash[7];
    ctx->total_len += len;
    while (buff < bend) {
        for (i=0; i<16; i++) {
            w[i] = buff[0]<<24 | buff[1]<<16 | buff[2]<<8 | buff[3];
            buff += 4;
        }
        R(a, b, c, d, e, f, g, h, w[ 0], 0x428a2f98);
        R(h, a, b, c, d, e, f, g, w[ 1], 0x71374491);
        R(g, h, a, b, c, d, e, f, w[ 2], 0xb5c0fbcf);
        R(f, g, h, a, b, c, d, e, w[ 3], 0xe9b5dba5);
        R(e, f, g, h, a, b, c, d, w[ 4], 0x3956c25b);
        R(d, e, f, g, h, a, b, c, w[ 5], 0x59f111f1);
        R(c, d, e, f, g, h, a, b, w[ 6], 0x923f82a4);
        R(b, c, d, e, f, g, h, a, w[ 7], 0xab1c5ed5);
        R(a, b, c, d, e, f, g, h, w[ 8], 0xd807aa98);
        R(h, a, b, c, d, e, f, g, w[ 9], 0x12835b01);
        R(g, h, a, b, c, d, e, f, w[10], 0x243185be);
        R(f, g, h, a, b, c, d, e, w[11], 0x550c7dc3);
        R(e, f, g, h, a, b, c, d, w[12], 0x72be5d74);
        R(d, e, f, g, h, a, b, c, w[13], 0x80deb1fe);
        R(c, d, e, f, g, h, a, b, w[14], 0x9bdc06a7);
        R(b, c, d, e, f, g, h, a, w[15], 0xc19bf174);
        R(a, b, c, d, e, f, g, h, W(16), 0xe49b69c1);
        R(h, a, b, c, d, e, f, g, W(17), 0xefbe4786);
        R(g, h, a, b, c, d, e, f, W(18), 0x0fc19dc6);
        R(f, g, h, a, b, c, d, e, W(19), 0x240ca1cc);
        R(e, f, g, h, a, b, c, d, W(20), 0x2de92c6f);
        R(d, e, f, g, h, a, b, c, W(21), 0x4a7484aa);
        R(c, d, e, f, g, h, a, b, W(22), 0x5cb0a9dc);
        R(b, c, d, e, f, g, h, a, W(23), 0x76f988da);
        R(a, b, c, d, e, f, g, h, W(24), 0x983e5152);
        R(h, a, b, c, d, e, f, g, W(25), 0xa831c66d);
        R(g, h, a, b, c, d, e, f, W(26), 0xb00327c8);
        R(f, g, h, a, b, c, d, e, W(27), 0xbf597fc7);
        R(e, f, g, h, a, b, c, d, W(28), 0xc6e00bf3);
        R(d, e, f, g, h, a, b, c, W(29), 0xd5a79147);
        R(c, d, e, f, g, h, a, b, W(30), 0x06ca6351);
        R(b, c, d, e, f, g, h, a, W(31), 0x14292967);
        R(a, b, c, d, e, f, g, h, W(32), 0x27b70a85);
        R(h, a, b, c, d, e, f, g, W(33), 0x2e1b2138);
        R(g, h, a, b, c, d, e, f, W(34), 0x4d2c6dfc);
        R(f, g, h, a, b, c, d, e, W(35), 0x53380d13);
        R(e, f, g, h, a, b, c, d, W(36), 0x650a7354);
        R(d, e, f, g, h, a, b, c, W(37), 0x766a0abb);
        R(c, d, e, f, g, h, a, b, W(38), 0x81c2c92e);
        R(b, c, d, e, f, g, h, a, W(39), 0x92722c85);
        R(a, b, c, d, e, f, g, h, W(40), 0xa2bfe8a1);
        R(h, a, b, c, d, e, f, g, W(41), 0xa81a664b);
        R(g, h, a, b, c, d, e, f, W(42), 0xc24b8b70);
        R(f, g, h, a, b, c, d, e, W(43), 0xc76c51a3);
        R(e, f, g, h, a, b, c, d, W(44), 0xd192e819);
        R(d, e, f, g, h, a, b, c, W(45), 0xd6990624);
        R(c, d, e, f, g, h, a, b, W(46), 0xf40e3585);
        R(b, c, d, e, f, g, h, a, W(47), 0x106aa070);
        R(a, b, c, d, e, f, g, h, W(48), 0x19a4c116);
        R(h, a, b, c, d, e, f, g, W(49), 0x1e376c08);
        R(g, h, a, b, c, d, e, f, W(50), 0x2748774c);
        R(f, g, h, a, b, c, d, e, W(51), 0x34b0bcb5);
        R(e, f, g, h, a, b, c, d, W(52), 0x391c0cb3);
        R(d, e, f, g, h, a, b, c, W(53), 0x4ed8aa4a);
        R(c, d, e, f, g, h, a, b, W(54), 0x5b9cca4f);
        R(b, c, d, e, f, g, h, a, W(55), 0x682e6ff3);
        R(a, b, c, d, e, f, g, h, W(56), 0x748f82ee);
        R(h, a, b, c, d, e, f, g, W(57), 0x78a5636f);
        R(g, h, a, b, c, d, e, f, W(58), 0x84c87814);
        R(f, g, h, a, b, c, d, e, W(59), 0x8cc70208);
        R(e, f, g, h, a, b, c, d, W(60), 0x90befffa);
        R(d, e, f, g, h, a, b, c, W(61), 0xa4506ceb);
        R(c, d, e, f, g, h, a, b, W(62), 0xbef9a3f7);
        R(b, c, d, e, f, g, h, a, W(63), 0xc67178f2);
        a = ctx->hash[0] += a;
        b = ctx->hash[1] += b;
        c = ctx->hash[2] += c;
        d = ctx->hash[3] += d;
        e = ctx->hash[4] += e;
        f = ctx->hash[5] += f;
        g = ctx->hash[6] += g;
        h = ctx->hash[7] += h;
    }
}
void sha224_update(sha224_ctx_t* ctx, void* _data, size_t dlen) {
    uint8_t* data = _data;
    if (ctx->buff_used) {
        size_t to_copy = 64 - ctx->buff_used;
        if (to_copy > dlen) to_copy = dlen;
        memcpy(ctx->buff + ctx->buff_used, data, to_copy);
        ctx->buff_used += to_copy;
        if (ctx->buff_used != 64) return;
        sha224_process(ctx->buff, 64, ctx);
        ctx->buff_used = 0;
        dlen -= to_copy;
        data += to_copy;
    }
    if (!dlen) return;
    size_t nblocks = dlen / 64;
    sha224_process(data, nblocks * 64, ctx);
    memcpy(ctx->buff, data + nblocks * 64, dlen - nblocks * 64);
    ctx->buff_used = dlen - nblocks * 64;
}
void sha224_finalize(sha224_t* hash, sha224_ctx_t* ctx) {
    uint64_t msglen = ctx->total_len + ctx->buff_used;
    uint8_t one = 0x80;
    sha224_update(ctx, &one, 1);
    size_t i;
    size_t padding = 0;
    size_t sz = ctx->buff_used;
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


