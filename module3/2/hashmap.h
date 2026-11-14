
#ifndef HASHMAP_H
#define HASHMAP_H

#include <sys/types.h>
#include "communicate.h"
#define HASH_CAPACITY 1024
#define MAX_KEY TOPIC_MAX_LEN

typedef struct HashNode {
    char key[MAX_KEY];
    pid_t value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_CAPACITY];
} HashTable;

HashTable *ht_create(void);
int ht_insert(HashTable *ht, const char *key, pid_t value);
int ht_get(const HashTable *ht, const char *key, pid_t *value);
int ht_remove(HashTable *ht, const char *key);
void ht_destroy(HashTable *ht);

#endif
