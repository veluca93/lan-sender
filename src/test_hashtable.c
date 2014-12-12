#include "hashtable.h"
#include <assert.h>
#include <limits.h>

hashtable_t* hashtable;

void nope(long long key) {
    assert(ht_get(hashtable, &key) != NULL);
}

void yeah(long long key) {
    htvalue_t* entry = ht_get(hashtable, &key);
    assert(entry != NULL);
    long long value = *(long long*) entry->value;
    assert(value * value == key);
}

int main() {
    hashtable = ht_create(sizeof(long long));
    for (long long i=0; i*i<LLONG_MAX; i++) {
        long long ii = i*i;
        ht_put(hashtable, &ii, &i);
    }

    nope(1234567890987654321LL);
    yeah(1206364825614169LL);
    nope(21734908741114320LL);
    yeah(21734908741114321LL);
    nope(21734908741114322LL);
    yeah(2173490882957090449LL);

    return 0;
}
