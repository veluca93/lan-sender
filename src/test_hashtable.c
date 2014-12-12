#include "hashtable.h"
#include <assert.h>
#include <limits.h>

hashtable_t* hashtable;

void nope(long long key) {
    assert(ht_get(hashtable, &key) == NULL);
}

void yeah(long long key) {
    htvalue_t* entry = ht_get(hashtable, &key);
    assert(entry != NULL);
    long long value = *(long long*) entry->value;
    assert(value * value == key);
}

int main() {
    hashtable = ht_create(sizeof(long long), sizeof(long long));
    for (long long i=0; i<INT_MAX; i+=123456) {
        long long ii = i*i;
        ht_put(hashtable, &ii, &i);
    }

    nope(1234567890987654321LL);
    yeah(4609178922992025600LL);
    nope(2323148376507826175LL);
    yeah(2323148376507826176LL);
    nope(2323148376507826177LL);
    yeah(1766907561692565504LL);

    return 0;
}
