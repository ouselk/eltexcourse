#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdint.h>

typedef struct QueueNode {
    int data;
    uint8_t priority;
    struct QueueNode *prev;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue {
    QueueNode *head;
    QueueNode *tail;
    size_t size;
} Queue;

void qInit(Queue *q);
void qClear(Queue *q);
int  qIsEmpty(const Queue *q);
int qPush(Queue *q, int data, uint8_t priority);
int qPop(Queue *q, QueueNode *out);                              /* первый в очереди */
int qPopPriority(Queue *q, uint8_t priority, QueueNode *out);    /* приоритет == priority */
int qPopNoLowerPriority(Queue *q, uint8_t priority, QueueNode *out); /* приоритет не ниже priority */

#endif