#include "hashtable.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>

hashtable_t* hashtable;

unsigned long long* keys;
unsigned long long* values;

int main() {
    const size_t num = 1000000;
    keys = malloc(num*sizeof* keys);
    values = malloc(num*sizeof* values);
    hashtable = ht_create(sizeof(unsigned long long), sizeof(unsigned long long));
    keys[0] = 1;
    values[0] = 123;
    for (size_t i=1; i<num; i++) {
        keys[i] = keys[i-1] * 22695477;
        values[i] = values[i-1] * 22695477; 
    }

    for (size_t i=0; i<num; i++)
        ht_put(hashtable, &keys[i], &values[i]);

    for (size_t i=0; i<num; i++) {
        htvalue_t* e = ht_get(hashtable, &keys[i]);
        assert(e);
        assert(*(unsigned long long*) e->value == values[i]);
    }

    return 0;
}
