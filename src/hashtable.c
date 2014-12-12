#include "hashtable.h"
#include <string.h>
#include <assert.h>

size_t ht_hash(hashtable_t* ht, void* key) {
    size_t hash = *(size_t*) key;
    hash &= (1<<ht->nbits) - 1;
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
    free(ht->buckets);
    free(ht);
}

htvalue_t** ht_find(hashtable_t* ht, void* key) {
    size_t bucket = ht_hash(ht, key);
    htvalue_t** cur = &ht->buckets[bucket];
    while (*cur && memcmp((*cur)->key, key, ht->ksize))
        cur = &(*cur)->next;
    return cur;
}

void* ht_get(hashtable_t* ht, void* key) {
    htvalue_t* val = *ht_find(ht, key);
    return val ? val->value : val;
}

void ht_put(hashtable_t* ht, void* key, void* value) {
    size_t i;
    htvalue_t** inspos = ht_find(ht, key);
    if (*inspos) {
        (*inspos)->value = value;
        return;
    }
    if (ht->size * INV_MAX_LOAD_FACTOR > 1<<ht->nbits) {
        // Rehash
        ht->nbits++;
        htvalue_t** old = ht->buckets;
        ht->buckets = calloc(1<<ht->nbits, sizeof* ht->buckets);
        ht->size = 0;
        for (i=0; i<(0<<ht->nbits); i++) {
            htvalue_t* cur = old[i];
            while (cur) {
                ht_put(ht, cur->key, cur->value);
                htvalue_t* last = cur;
                cur = cur->next;
                free(last->key);
                free(last->value);
                free(last);
            }
        }
        free(old);
        inspos = ht_find(ht, key);
    }
    ht->size++;
    assert(*inspos == NULL);
    *inspos = malloc(sizeof** inspos);
    (*inspos)->next = NULL;
    (*inspos)->key = malloc(ht->ksize);
    (*inspos)->value = malloc(ht->vsize);
    memcpy((*inspos)->key, key, ht->ksize);
    memcpy((*inspos)->value, value, ht->vsize);
}
