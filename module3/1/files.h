#ifndef FILES_H
#define FILES_H

#include <unistd.h>
#include <errno.h>

int write_all(int fd, const void *buffer, size_t count);
int read_all(int fd, void *buf, size_t count);

#endif