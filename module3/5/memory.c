#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "memory.h"

int mem_create(Mem *m)
{
    /* убрать остатки прошлого запуска */
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd == -1)
        return -1;
    if (ftruncate(fd, SHM_SIZE) == -1) {    /* новая память заполнена нулями */
        close(fd);
        return -1;
    }
    m->shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (m->shm == MAP_FAILED)
        return -1;

    /* начальное значение 1 = память доступна */
    m->sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    if (m->sem == SEM_FAILED) {
        munmap(m->shm, SHM_SIZE);
        return -1;
    }

    /* инициализация заголовка под блокировкой */
    if (blockmem(m) == -1)
        return -1;
    ShmHeader *h = (ShmHeader *)m->shm;
    h->first = 0;
    h->last = 0;
    h->free_off = sizeof(ShmHeader);
    h->done = 0;
    return unblockmem(m);
}

int mem_open(Mem *m)
{
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1)
        return -1;
    m->shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (m->shm == MAP_FAILED)
        return -1;

    m->sem = sem_open(SEM_NAME, 0);
    if (m->sem == SEM_FAILED) {
        munmap(m->shm, SHM_SIZE);
        return -1;
    }
    return 0;
}

void mem_detach(Mem *m)
{
    munmap(m->shm, SHM_SIZE);
    sem_close(m->sem);
}

void mem_destroy(Mem *m)
{
    mem_detach(m);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
}

int blockmem(Mem *m)
{
    while (sem_wait(m->sem) == -1) {
        if (errno != EINTR)
            return -1;
    }
    return 0;
}

int unblockmem(Mem *m)
{
    return sem_post(m->sem);
}

int writearray(Mem *m, const int *arr, int n)
{
    ShmHeader *h = (ShmHeader *)m->shm;
    int need = (int)sizeof(BlockHeader) + n * (int)sizeof(int);
    int result;

    if (blockmem(m) == -1)
        return -1;

    if (h->free_off + need > SHM_SIZE) {
        h->done = 1;                    /* память исчерпана */
        result = 0;
    } else {
        int off = h->free_off;
        BlockHeader *b = (BlockHeader *)((char *)m->shm + off);

        b->count = n;
        b->next = 0;                    /* пока последний */
        memcpy(b + 1, arr, n * sizeof(int));

        if (h->first == 0)
            h->first = off;
        else
            ((BlockHeader *)((char *)m->shm + h->last))->next = off;

        h->last = off;
        h->free_off = off + need;
        result = 1;
    }

    if (unblockmem(m) == -1)
        return -1;
    return result;
}

int readarray(Mem *m, int *buf, int *n, int *done)
{
    ShmHeader *h = (ShmHeader *)m->shm;
    int result = 0;
    int off;

    if (blockmem(m) == -1)
        return -1;

    for (off = h->first; off != 0; ) {
        BlockHeader *b = (BlockHeader *)((char *)m->shm + off);
        if (b->count > 0) {
            *n = b->count;
            memcpy(buf, b + 1, b->count * sizeof(int));
            b->count = 0;               /* пометили как обработанный */
            result = 1;
            break;
        }
        off = b->next;
    }
    *done = h->done;

    if (unblockmem(m) == -1)
        return -1;
    return result;
}