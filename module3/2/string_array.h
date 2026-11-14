
#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include <stddef.h>

typedef struct {
    char **data;
    size_t count;
    size_t capacity;
} StringArray;

void string_array_init(StringArray *arr);

int string_array_push(StringArray *arr, const char *str);

const char *string_array_get(const StringArray *arr, size_t index);

int string_array_remove(StringArray *arr, const char *str);

void string_array_destroy(StringArray *arr);

#endif
