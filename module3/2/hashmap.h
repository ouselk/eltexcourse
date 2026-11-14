
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <sys/types.h>

#include "string_array.h"

#define HASH_CAPACITY 1024

typedef struct HashNode {
    pid_t key;
    StringArray value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_CAPACITY];
} HashTable;

HashTable *ht_create(void);

int ht_insert(HashTable *ht, pid_t key, const char *str);

const StringArray *ht_get(const HashTable *ht, pid_t key);

int ht_remove(HashTable *ht, pid_t key, const char *str);

void ht_destroy(HashTable *ht);

#endif
