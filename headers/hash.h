#ifndef LC_HASH_H
#define LC_HASH_H
#include <stdint.h>

typedef struct __attribute__ ((__packed__)) sha224 {
    uint8_t hash[28];
} sha224_t;

typedef struct __attribute__ ((__packed__)) bshift {
    uint8_t hash[4];
} bshift_t;

typedef struct __attribute__ ((__packed__)) hash {
    sha224_t strong_hash;
    bshift_t weak_hash;
} hash_t;

#endif
