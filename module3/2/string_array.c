
#include "string_array.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void string_array_init(StringArray *arr)
{
    if (!arr)
        return;

    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

int string_array_push(StringArray *arr, const char *str)
{
    if (!arr || !str)
        return -1;

    if (arr->count == arr->capacity) {
        size_t new_capacity = arr->capacity
            ? arr->capacity * 2
            : 4;

        if (new_capacity < arr->capacity ||
            new_capacity > SIZE_MAX / sizeof(char *))
            return -1;

        char **tmp = realloc(
            arr->data,
            new_capacity * sizeof(char *)
        );

        if (!tmp)
            return -1;

        arr->data = tmp;
        arr->capacity = new_capacity;
    }

    char *copy = strdup(str);

    if (!copy)
        return -1;

    arr->data[arr->count++] = copy;

    return 0;
}

const char *string_array_get(const StringArray *arr, size_t index)
{
    if (!arr || index >= arr->count)
        return NULL;

    return arr->data[index];
}

int string_array_remove(StringArray *arr, const char *str)
{
    if (!arr || !str)
        return -1;

    for (size_t i = 0; i < arr->count; i++) {
        if (strcmp(arr->data[i], str) == 0) {
            free(arr->data[i]);

            for (size_t j = i; j + 1 < arr->count; j++)
                arr->data[j] = arr->data[j + 1];

            arr->count--;

            return 0;
        }
    }

    return -1;
}

void string_array_destroy(StringArray *arr)
{
    if (!arr)
        return;

    for (size_t i = 0; i < arr->count; i++)
        free(arr->data[i]);

    free(arr->data);

    arr->data = NULL;
    arr->count = 0;
    arr->capacity = 0;
}
