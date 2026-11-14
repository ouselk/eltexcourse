
#include "hashtable.h"

#include <stdlib.h>

static unsigned int hash(pid_t key)
{
    return (unsigned int)key % HASH_SIZE;
}

HashTable *ht_create(void)
{
    return calloc(1, sizeof(HashTable));
}

int ht_insert(HashTable *ht, pid_t key, const char *str)
{
    if (!ht || !str)
        return -1;

    unsigned int index = hash(key);

    // Проверяем, существует ли PID
    for (HashNode *node = ht->buckets[index];
         node != NULL;
         node = node->next)
    {
        if (node->key == key) {
            // PID существует — добавляем строку
            return string_array_push(&node->value, str);
        }
    }

    // PID не существует — создаём новый узел
    HashNode *node = malloc(sizeof(HashNode));

    if (!node)
        return -1;

    string_array_init(&node->value);

    // Добавляем первую строку
    if (string_array_push(&node->value, str) != 0) {
        string_array_destroy(&node->value);
        free(node);
        return -1;
    }

    node->key = key;
    node->next = ht->buckets[index];

    ht->buckets[index] = node;

    return 0;
}

const StringArray *ht_get(const HashTable *ht, pid_t key)
{
    if (!ht)
        return NULL;

    unsigned int index = hash(key);

    for (HashNode *node = ht->buckets[index];
         node != NULL;
         node = node->next)
    {
        if (node->key == key)
            return &node->value;
    }

    return NULL;
}

int ht_remove(HashTable *ht, pid_t key, const char *str)
{
    if (!ht || !str)
        return -1;

    unsigned int index = hash(key);
    HashNode **current = &ht->buckets[index];

    while (*current) {
        if ((*current)->key == key) {
            HashNode *node = *current;

            // Удаляем строку из массива
            if (string_array_remove(&node->value, str) != 0)
                return -1;

            // Если массив опустел — удаляем узел
            if (node->value.count == 0) {
                *current = node->next;

                string_array_destroy(&node->value);
                free(node);
            }

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

    for (size_t i = 0; i < HASH_SIZE; i++) {
        HashNode *node = ht->buckets[i];

        while (node) {
            HashNode *next = node->next;

            string_array_destroy(&node->value);
            free(node);

            node = next;
        }
    }

    free(ht);
}
