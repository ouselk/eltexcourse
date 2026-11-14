
#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

static unsigned int hash(const char *key)
{
    unsigned int hash = 5381;
    int c;

    while ((c = (unsigned char)*key++))
        hash = ((hash << 5) + hash) + c;

    return hash % HASH_SIZE;
}

HashTable *ht_create(void)
{
    return calloc(1, sizeof(HashTable));
}

int ht_insert(HashTable *ht, const char *key, pid_t value)
{
    if (!ht || !key || strlen(key) >= MAX_KEY)
        return -1;

    unsigned int index = hash(key);

    for (HashNode *node = ht->buckets[index];
         node != NULL;
         node = node->next)
    {
        if (strcmp(node->key, key) == 0) {
            node->value = value;
            return 0;
        }
    }
    HashNode *node = malloc(sizeof(HashNode));

    if (!node)
        return -1;

    strcpy(node->key, key);
    node->value = value;

    node->next = ht->buckets[index];
    ht->buckets[index] = node;

    return 0;
}

int ht_get(const HashTable *ht, const char *key, pid_t *value)
{
    if (!ht || !key || !value)
        return -1;

    unsigned int index = hash(key);

    for (HashNode *node = ht->buckets[index];
         node != NULL;
         node = node->next)
    {
        if (strcmp(node->key, key) == 0) {
            *value = node->value;
            return 0;
        }
    }

    return -1;
}

int ht_remove(HashTable *ht, const char *key)
{
    if (!ht || !key)
        return -1;

    unsigned int index = hash(key);
    HashNode **current = &ht->buckets[index];

    while (*current) {
        if (strcmp((*current)->key, key) == 0) {
            HashNode *tmp = *current;

            *current = tmp->next;

            free(tmp);

            return 0;
        }

        current = &(*current)->next;
    }

    return -1;
}

void ht_destroy(HashTable *ht)
{
    if (!ht)
        return;

    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *node = ht->buckets[i];

        while (node) {
            HashNode *next = node->next;

            free(node);
            node = next;
        }
    }

    free(ht);
}
