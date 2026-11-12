#include <unistd.h>
#include <errno.h>
#include "files.h"

int write_all(int fd, const void *buffer, size_t count)
{
    const char *ptr = buffer;
    size_t total = 0;

    while (total < count)
    {
        ssize_t bytes = write(fd, ptr + total, count - total);

        if (bytes == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (bytes == 0)
            return -1;

        total += bytes;
    }

    return 0;
}

int read_all(int fd, void *buf, size_t count)
{
    char *ptr = buf;
    size_t total = 0;

    while (total < count)
    {
        ssize_t bytes = read(fd, ptr + total, count - total);

        if (bytes == -1)
        {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (bytes == 0)
            break;
        total += bytes;
    }

    return total;
}