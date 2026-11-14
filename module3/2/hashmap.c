
#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

static unsigned long hash(const char *key)
{
    unsigned long h = 5381;
    int c;

    while ((c = (unsigned char)*key++))
        h = h * 33 + c;

    return h % HASH_SIZE;
}

HashTable *ht_create(void)
{
    return calloc(1, sizeof(HashTable));
}

int ht_insert(HashTable *ht, const char *key, pid_t pid)
{
    if (!ht || !key || strlen(key) >= MAX_KEY)
        return -1;

    unsigned long index = hash(key);
    HashNode *node = ht->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            pid_t *tmp = realloc(node->value,
                                 (node->count + 1) * sizeof(pid_t));
            if (!tmp)
                return -1;

            node->value = tmp;
            node->value[node->count++] = pid;
            return 0;
        }

        node = node->next;
    }

    node = calloc(1, sizeof(HashNode));
    if (!node)
        return -1;

    strcpy(node->key, key);

    node->value = malloc(sizeof(pid_t));
    if (!node->value) {
        free(node);
        return -1;
    }

    node->value[0] = pid;
    node->count = 1;

    node->next = ht->buckets[index];
    ht->buckets[index] = node;

    return 0;
}

int ht_get(HashTable *ht, const char *key, pid_t **pids, size_t *count)
{
    if (!ht || !key || !pids || !count)
        return -1;

    unsigned long index = hash(key);
    HashNode *node = ht->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            *pids = node->value;
            *count = node->count;
            return 0;
        }

        node = node->next;
    }

    return -1;
}

int ht_remove(HashTable *ht, const char *key)
{
    if (!ht || !key)
        return -1;

    unsigned long index = hash(key);
    HashNode **current = &ht->buckets[index];

    while (*current) {
        HashNode *node = *current;

        if (strcmp(node->key, key) == 0) {
            *current = node->next;
            free(node->value);
            free(node);
            return 0;
        }

        current = &node->next;
    }

    return -1;
}

void ht_destroy(HashTable *ht)
{
    if (!ht)
        return;

    for (size_t i = 0; i < HASH_SIZE; i++) {
        HashNode *node = ht->buckets[i];

        while (node) {
            HashNode *next = node->next;
            free(node->value);
            free(node);
            node = next;
        }
    }

    free(ht);
}
