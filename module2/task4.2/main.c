#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"

static void qPrint(const Queue *q) {
    printf("[size=%zu] ", q->size);
    for (const QueueNode *n = q->head; n; n = n->next)
        printf("(msg=%d, prio=%u) ", n->data, (unsigned)n->priority);
    printf("\n");
}

static void report(const char *what, int found, const QueueNode *n) {
    if (found)
        printf("%-34s -> msg=%d, prio=%u\n", what, n->data, (unsigned)n->priority);
    else
        printf("%-34s -> нет подходящего элемента\n", what);
}

int main(void) {
    Queue q;
    QueueNode out;
    qInit(&q);

    /* Имитация генерации сообщений с разными приоритетами. */
    srand(12345);
    const uint8_t prios[] = {0, 10, 50, 100, 255};
    for (int i = 1; i <= 15; i++) {
        uint8_t p = (rand() % 3 == 0) ? (uint8_t)(rand() % 256)
                                      : prios[rand() % 5];
        qPush(&q, i, p);
        printf("push msg=%-2d prio=%u\n", i, (unsigned)p);
    }
    printf("\nОчередь после генерации:\n");
    qPrint(&q);

    printf("\n--- Выборки ---\n");
    report("qPop (первый)", qPop(&q, &out), &out);
    report("qPopPriority(50)", qPopPriority(&q, 50, &out), &out);
    report("qPopPriority(50) ещё раз", qPopPriority(&q, 50, &out), &out);
    report("qPopPriority(77)", qPopPriority(&q, 77, &out), &out);
    report("qPopNoLowerPriority(10)", qPopNoLowerPriority(&q, 10, &out), &out);
    report("qPopNoLowerPriority(0)", qPopNoLowerPriority(&q, 0, &out), &out);
    report("qPopNoLowerPriority(255)", qPopNoLowerPriority(&q, 255, &out), &out);
    report("qPopPriority(14)", qPopPriority(&q, 14, &out), &out);
    report("qPopNoLowerPriority(14)", qPopNoLowerPriority(&q, 14, &out), &out);
    report("qPopPriority(14)", qPopPriority(&q, 14, &out), &out);


    printf("\nОчередь после выборок:\n");
    qPrint(&q);

    /* Проверки корректности. */
    printf("\n--- Автопроверки ---\n");
    Queue t;
    qInit(&t);
    assert(!qPop(&t, &out));
    assert(!qPopPriority(&t, 0, &out));
    assert(!qPopNoLowerPriority(&t, 255, &out));

    qPush(&t, 1, 5);
    qPush(&t, 2, 3);
    qPush(&t, 3, 5);   /* FIFO внутри одного приоритета */
    qPush(&t, 4, 0);
    qPush(&t, 5, 255);
    assert(qPopNoLowerPriority(&t, 0, &out) && out.data == 4);
    assert(!qPopNoLowerPriority(&t, 2, &out));
    assert(qPopPriority(&t, 5, &out) && out.data == 1);
    assert(qPopPriority(&t, 5, &out) && out.data == 3);
    assert(!qPopPriority(&t, 5, &out));
    assert(qPop(&t, &out) && out.data == 2);
    assert(qPop(&t, &out) && out.data == 5 && out.priority == 255);
    assert(qIsEmpty(&t) && t.size == 0 && t.head == NULL && t.tail == NULL);
    printf("Все проверки пройдены.\n");

    /* Выгрузка остатка в порядке приоритета. */
    printf("\nОстаток очереди по порядку:\n");
    while (qPop(&q, &out))
        printf("(msg=%d, prio=%u) ", out.data, (unsigned)out.priority);
    printf("\n");

    qClear(&q);
    qClear(&t);
    return 0;
}