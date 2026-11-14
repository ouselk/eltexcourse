
#include "pids_array.h"

#include <stdlib.h>

int pids_array_add(PidsArray *array, pid_t pid)
{
    if (array->count == array->capacity) {
        size_t new_capacity = array->capacity == 0
            ? 4
            : array->capacity * 2;

        pid_t *tmp = realloc(
            array->items,
            new_capacity * sizeof(pid_t)
        );

        if (!tmp)
            return -1;

        array->items = tmp;
        array->capacity = new_capacity;
    }

    array->items[array->count++] = pid;

    return 0;
}

void pids_array_destroy(PidsArray *array)
{
    if (!array)
        return;

    free(array->items);

    array->items = NULL;
    array->count = 0;
    array->capacity = 0;
}
