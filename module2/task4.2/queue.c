#include <stdlib.h>
#include "queue.h"

void qInit(Queue *q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

int qIsEmpty(const Queue *q) {
    return q->head == NULL;
}

void qClear(Queue *q) {
    QueueNode *n = q->head;
    while (n) {
        QueueNode *next = n->next;
        free(n);
        n = next;
    }
    qInit(q);
}

int qPush(Queue *q, int data, uint8_t priority) {
    QueueNode *n = malloc(sizeof *n);
    if (!n) return -1;
    n->data = data;
    n->priority = priority;

    QueueNode *pos = q->tail;
    while (pos && pos->priority > priority)
        pos = pos->prev;

    n->prev = pos;
    n->next = pos ? pos->next : q->head;
    if (n->prev) n->prev->next = n; else q->head = n;
    if (n->next) n->next->prev = n; else q->tail = n;
    q->size++;
    return 0;
}

static void takeNode(Queue *q, QueueNode *n, QueueNode *out) {
    if (n->prev) n->prev->next = n->next; else q->head = n->next;
    if (n->next) n->next->prev = n->prev; else q->tail = n->prev;
    q->size--;
    if (out) {
        out->data = n->data;
        out->priority = n->priority;
        out->prev = out->next = NULL;
    }
    free(n);
}

int qPop(Queue *q, QueueNode *out) {
    if (!q->head) return 0;
    takeNode(q, q->head, out);
    return 1;
}

int qPopPriority(Queue *q, uint8_t priority, QueueNode *out) {
    for (QueueNode *n = q->head; n && n->priority <= priority; n = n->next) {
        if (n->priority == priority) {
            takeNode(q, n, out);
            return 1;
        }
    }
    return 0;
}

int qPopNoLowerPriority(Queue *q, uint8_t priority, QueueNode *out)
{
    QueueNode *n = q->tail;
    while (n && n->priority > priority)
        n = n->prev;
    if (!n) return 0;
 
    while (n->prev && n->prev->priority == n->priority)
        n = n->prev;
 
    takeNode(q, n, out);
    return 1;
}