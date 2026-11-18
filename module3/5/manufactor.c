#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "memory.h"

int genArray(int *arr)
{
    int n = rand() % MAX_N + 1;
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 1000;
    return n;
}

void check_and_wait(Mem *m)
{
    ShmHeader *h = (ShmHeader *)m->shm;

    while (1)
    {
        int all_done = 1;

        if (blockmem(m) == -1) {
            perror("blockmem");
            break;
        }
        for (int off = h->first; off != 0; ) {
            BlockHeader *b = (BlockHeader *)((char *)m->shm + off);
            if (b->count != 0) {
                all_done = 0;
                break;
            }
            off = b->next;
        }
        unblockmem(m);

        if (all_done)
            break;
        printf("Producer: not all sets processed yet, sleeping...\n");
        sleep(CHECK_SLEEP_S);
    }

    mem_destroy(m);
    printf("Producer: all sets processed, memory detached, exiting.\n");
}

int main(void)
{
    Mem m;
    if (mem_create(&m) == -1) {
        perror("mem_create");
        return 1;
    }

    srand((unsigned)time(NULL) ^ (unsigned)getpid());

    int arr[MAX_N];
    int count = 0;
    while (1)
    {
        int n = genArray(arr);
        int r = writearray(&m, arr, n);
        if (r == -1) {
            perror("writearray");
            return 1;
        }
        if (r == 0)
            break;                      /* память исчерпана */
        count++;
        printf("Producer: wrote set #%d (%d numbers)\n", count, n);
        usleep(PRODUCER_SLEEP_US);
    }
    printf("Producer: shared memory is full, %d sets written.\n", count);

    check_and_wait(&m);
    return 0;
}