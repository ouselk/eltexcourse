
#ifndef PIDS_ARRAY_H
#define PIDS_ARRAY_H

#include <sys/types.h>
#include <stddef.h>

typedef struct {
    pid_t *items;
    size_t count;
    size_t capacity;
} PidsArray;

int pids_array_add(PidsArray *array, pid_t pid);
void pids_array_destroy(PidsArray *array);
int  pids_array_contains(const PidsArray *array, pid_t pid);
void pids_array_remove(PidsArray *array, pid_t pid);
#endif
