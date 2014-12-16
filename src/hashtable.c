#include "hashtable.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

size_t ht_hash(hashtable_t* ht, void* key) {
    size_t hash = *(size_t*) ((char*)key+ht->ksize-sizeof(size_t));
    hash %= (1<<ht->nbits);
    return hash;
}

hashtable_t* ht_create(size_t ksize, size_t vsize) {
    hashtable_t* ht = malloc(sizeof* ht);
    ht->size = 0;
    ht->nbits = 1;
    ht->buckets = calloc(1<<ht->nbits, sizeof* ht->buckets);
    ht->ksize = ksize;
    ht->vsize = vsize;
    return ht;
}

void ht_free(hashtable_t* ht) {
    size_t i;
    for (i=0; i<1<<ht->nbits; i++)
        if (ht->buckets[i].key) {
            free(ht->buckets[i].key);
            free(ht->buckets[i].value);
        }
    free(ht->buckets);
    free(ht);
}

size_t ht_find(hashtable_t* ht, void* key) {
    size_t bucket = ht_hash(ht, key);
    while (ht->buckets[bucket].key && memcmp(ht->buckets[bucket].key, key, ht->ksize)) {
        bucket++;
        if (bucket == 1<<ht->nbits) bucket = 0; 
    }
    return bucket;
}

htvalue_t* ht_get(hashtable_t* ht, void* key) {
    size_t bucket = ht_find(ht, key);
    return ht->buckets[bucket].key ? &ht->buckets[bucket] : NULL;
}

void ht_put_(hashtable_t* ht, void* key, void* value, uint8_t become_owner) {
    size_t i;
    size_t bucket = ht_find(ht, key);
    if (ht->buckets[bucket].key) {
        ht->buckets[bucket].value = value;
        return;
    }
    if (ht->size * INV_MAX_LOAD_FACTOR > 1<<ht->nbits) {
        // Rehash
        ht->nbits++;
        ht->buckets = realloc(ht->buckets, (1<<ht->nbits) * (sizeof* ht->buckets));
        for (i=(1<<(ht->nbits-1)); i<(1<<ht->nbits); i++)
            ht->buckets[i].key = NULL;
        for (i=0; i<(1<<(ht->nbits-1)); i++) {
            if (!ht->buckets[i].key) continue;
            size_t npos = ht_find(ht, ht->buckets[i].key);
            if (npos == i) continue;
            ht->buckets[npos].key = ht->buckets[i].key;
            ht->buckets[npos].value = ht->buckets[i].value;
            ht->buckets[i].key = NULL;
        }
        bucket = ht_find(ht, key);
    }
    ht->size++;
    assert(ht->buckets[bucket].key == NULL);
    if (become_owner) {
        ht->buckets[bucket].key = key;
        ht->buckets[bucket].value = value;
    } else {
        ht->buckets[bucket].key = memcpy(malloc(ht->ksize), key, ht->ksize);
        ht->buckets[bucket].value = memcpy(malloc(ht->vsize), value, ht->vsize);
    }
}

void ht_put(hashtable_t* ht, void* key, void* value) {
    ht_put_(ht, key, value, 0);
}

