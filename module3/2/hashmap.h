
#ifndef HASHMAP_H
#define HASHMAP_H

#include "pids_array.h"
#include "communicate.h"
#define HASH_SIZE 1024
#define MAX_KEY TOPIC_MAX_LEN

typedef struct HashNode {
    char key[MAX_KEY];
    PidsArray value;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_SIZE];
} HashTable;

HashTable *ht_create(void);
int ht_insert(HashTable *ht, const char *key, pid_t pid);
PidsArray *ht_get(HashTable *ht, const char *key);
int ht_remove(HashTable *ht, const char *key);
void ht_destroy(HashTable *ht);

#endif
