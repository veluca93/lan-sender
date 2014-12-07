#ifndef LC_HASH_H
#define LC_HASH_H
#include <stdint.h>

typedef struct __attribute__ ((__packed__)) sha224 {
    uint32_t hash[7];
} sha224_t;

typedef struct __attribute__ ((__packed__)) rollh {
    uint32_t hash;
} rollh_t;

typedef struct __attribute__ ((__packed__)) hash {
    rollh_t weak_hash;
    sha224_t strong_hash;
} hash_t;

#endif
