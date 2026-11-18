#include <stdio.h>
#include <unistd.h>
#include "memory.h"

int process_one(Mem *m, int *finished)
{
    int buf[MAX_N];
    int n = 0;
    int r = readarray(m, buf, &n, finished);

    if (r != 1)
        return r;

    int max = buf[0], min = buf[0];
    for (int i = 1; i < n; i++) {
        if (buf[i] > max) max = buf[i];
        if (buf[i] < min) min = buf[i];
    }
    printf("Consumer %d: массив с %d числами обработан, max = %d, min = %d\n",
           getpid(), n, max, min);
    fflush(stdout);
    return 1;
}

int main(void)
{
    Mem m;
    if (mem_open(&m) == -1) {
        fprintf(stderr, "Consumer: запустите производителья.\n");
        return 1;
    }

    while (1)
    {
        int finished = 0;
        int r = process_one(&m, &finished);

        if (r == -1)
            break;
        if (r == 0 && finished)
            break;

        usleep(CONSUMER_SLEEP_US);
    }

    mem_detach(&m);
    printf("Consumer %d: exiting.\n", (int)getpid());
    return 0;
}