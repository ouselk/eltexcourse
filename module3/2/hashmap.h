
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <sys/types.h>

#define HASH_SIZE 1024
#define MAX_KEY 256

typedef struct HashNode {
    char key[MAX_KEY];
    pid_t *value;
    size_t count;
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_SIZE];
} HashTable;

HashTable *ht_create(void);
int ht_insert(HashTable *ht, const char *key, pid_t pid);
int ht_get(HashTable *ht, const char *key, pid_t **pids, size_t *count);
int ht_remove(HashTable *ht, const char *key);
void ht_destroy(HashTable *ht);

#endif
