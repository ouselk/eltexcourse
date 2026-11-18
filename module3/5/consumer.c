#include <stdio.h>
#include <unistd.h>
#include "memory.h"

/* Берёт один необработанный набор, находит в нём максимум и минимум,
 * выводит их. Набор помечается обработанным (count = 0) внутри readarray.
 * Возврат: 1 - набор обработан, 0 - необработанных нет, -1 - ошибка/конец. */
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
    printf("Consumer %d: set of %d numbers, max = %d, min = %d\n",
           (int)getpid(), n, max, min);
    fflush(stdout);
    return 1;
}

int main(void)
{
    Mem m;
    if (mem_open(&m) == -1) {
        fprintf(stderr, "Consumer: shared memory not found, start the producer first\n");
        return 1;
    }

    for (;;) {
        int finished = 0;
        int r = process_one(&m, &finished);

        if (r == -1)                    /* производитель удалил семафор - конец */
            break;
        if (r == 0 && finished)         /* всё обработано, новых не будет */
            break;

        /* после набора (или если наборов пока нет) - спим */
        usleep(CONSUMER_SLEEP_US);
    }

    mem_detach(&m);
    printf("Consumer %d: exiting.\n", (int)getpid());
    return 0;
}