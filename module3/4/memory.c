#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "memory.h"

union semun {
    int val;
};

static struct sembuf lock_op      = {0, -1, 0};
static struct sembuf unlock_op[2] = {{0, 0, 0}, {0, 1, 0}};

static int do_semop(int semid, struct sembuf *ops, size_t n)
{
    while (semop(semid, ops, n) == -1) {
        if (errno != EINTR)
            return -1;
    }
    return 0;
}

int mem_create(Mem *m)
{
    key_t key = ftok(KEY_PATH, KEY_ID);
    if (key == -1)
        return -1;

    // убрать остатки прошлого запуска
 //   int old = shmget(key, 0, 0666);
  //  if (old != -1)
  //      shmctl(old, IPC_RMID, NULL);
  //  old = semget(key, 1, 0666);
  //  if (old != -1)
  //      semctl(old, 0, IPC_RMID);

    m->shmid = shmget(key, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (m->shmid == -1)
        return -1;
    m->semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (m->semid == -1)
        return -1;

    m->shm = shmat(m->shmid, NULL, 0);      /* новая память заполнена нулями */
    if (m->shm == (void *)-1)
        return -1;

    union semun arg = { .val = 0 };
    if (semctl(m->semid, 0, SETVAL, arg) == -1)
        return -1;

    ShmHeader *h = (ShmHeader *)m->shm;
    h->first = 0;
    h->last = 0;
    h->free_off = sizeof(ShmHeader);
    h->done = 0;

    return unblockmem(m);                   /* 0 -> 1, память открыта */
}

int mem_open(Mem *m)
{
    key_t key = ftok(KEY_PATH, KEY_ID);
    if (key == -1)
        return -1;

    m->shmid = shmget(key, 0, 0666);
    m->semid = semget(key, 1, 0666);
    if (m->shmid == -1 || m->semid == -1)
        return -1;

    m->shm = shmat(m->shmid, NULL, 0);
    if (m->shm == (void *)-1)
        return -1;
    return 0;
}

void mem_detach(Mem *m)
{
    shmdt(m->shm);
}

void mem_destroy(Mem *m)
{
    shmdt(m->shm);
    shmctl(m->shmid, IPC_RMID, NULL);
    semctl(m->semid, 0, IPC_RMID);
}

int blockmem(Mem *m)
{
    return do_semop(m->semid, &lock_op, 1);
}

int unblockmem(Mem *m)
{
    return do_semop(m->semid, unlock_op, 2);
}

int writearray(Mem *m, const int *arr, int n)
{
    ShmHeader *h = (ShmHeader *)m->shm;
    int need = sizeof(BlockHeader) + n * sizeof(int);
    int result=0;

    if (blockmem(m) == -1)
        return -1;

    if (h->free_off + need > SHM_SIZE) {
        h->done = 1;
        result = 0;
    } else {
        int off = h->free_off;
        BlockHeader *b = (BlockHeader *)((char *)m->shm + off);

        b->count = n;
        b->next = 0;
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