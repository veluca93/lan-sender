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
    size_t i;
    hashtable_t* ht = malloc(sizeof* ht);
    ht->size = 0;
    ht->nbits = 1;
    ht->buckets = malloc(1<<ht->nbits * sizeof* ht->buckets);
    for (i=0; i<(1<<ht->nbits); i++)
        ht->buckets[i] = calloc(1, sizeof** ht->buckets);
    ht->ksize = ksize;
    ht->vsize = vsize;
    return ht;
}

void ht_free(hashtable_t* ht) {
    free(ht->buckets);
    free(ht);
}

size_t ht_find(hashtable_t* ht, void* key, size_t* pos) {
    size_t bucket = ht_hash(ht, key);
    htvalue_t* bkt = ht->buckets[bucket];
    *pos = 0;
    while (bkt[*pos].key && memcmp(bkt[*pos].key, key, ht->ksize)) {
        (*pos)++;
    }
    return bucket;
}

htvalue_t* ht_get(hashtable_t* ht, void* key) {
    size_t pos;
    size_t bucket = ht_find(ht, key, &pos);
    return ht->buckets[bucket][pos].key ? &ht->buckets[bucket][pos] : NULL;
}

void ht_put_(hashtable_t* ht, void* key, void* value, uint8_t become_owner) {
    size_t i;
    size_t pos;
    size_t bucket = ht_find(ht, key, &pos);
    if (ht->buckets[bucket][pos].key) {
        ht->buckets[bucket][pos].value = value;
        return;
    }
    if (ht->size * INV_MAX_LOAD_FACTOR > 1<<ht->nbits) {
        // Rehash
        ht->nbits++;
        htvalue_t** old = ht->buckets;
        ht->buckets = malloc((1<<ht->nbits) * (sizeof* ht->buckets));
        for (i=0; i<(1<<ht->nbits); i++)
            ht->buckets[i] = calloc(1, sizeof** ht->buckets);
        ht->size = 0;
        for (i=0; i<(1<<(ht->nbits-1)); i++) {
            htvalue_t* cur;
            for (cur = old[i]; cur->key; cur++)
                ht_put_(ht, cur->key, cur->value, 1);
            free(old[i]);
        }
        free(old);
        bucket = ht_find(ht, key, &pos);
    }
    ht->size++;
    assert(ht->buckets[bucket][pos].key == NULL);
    size_t test = 1;
    while (test < pos+1) test <<= 1;
    if (pos+1 == test) {
        ht->buckets[bucket] = realloc(ht->buckets[bucket], (sizeof** ht->buckets) * (pos*2+2));
    }
    if (become_owner) {
        ht->buckets[bucket][pos].key = key;
        ht->buckets[bucket][pos].value = value;
    } else {
        ht->buckets[bucket][pos].key = memcpy(malloc(ht->ksize), key, ht->ksize);
        ht->buckets[bucket][pos].value = memcpy(malloc(ht->vsize), value, ht->vsize);
    }
    ht->buckets[bucket][pos+1].key = NULL;
}

void ht_put(hashtable_t* ht, void* key, void* value) {
    ht_put_(ht, key, value, 0);
}

