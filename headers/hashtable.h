#ifndef LS_HASHTABLE_H
#define LS_HASHTABLE_H

#include <stdlib.h>
#define INV_MAX_LOAD_FACTOR 4

/* An hash table that uses the leftmost bits of the memory
 * representation of the key as an hash. This similar to
 * taking the number modulo 2^k for big-endian architetures.
 */

typedef struct htv {
    void* key;
    void* value;
    struct htv* next;
} htvalue_t;

typedef struct ht {
    size_t size;
    size_t nbits;
    size_t ksize;
    size_t vsize;
    htvalue_t** buckets;
} hashtable_t;

void* ht_get(hashtable_t* ht, void* key);
void ht_put(hashtable_t* ht, void* key, void* value); 
hashtable_t* ht_create(size_t ksize, size_t vsize);
void ht_free(hashtable_t* ht);

#endif
