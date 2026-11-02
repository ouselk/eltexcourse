#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

int listCompare(const Contact *a, const Contact *b)
{
    int r = strcmp(a->family, b->family);
    if (r != 0)
        return r;
    return strcmp(a->name, b->name);
}

void listInit(LinkedList *list)
{
    list->head = NULL;
    list->tail = NULL;
}

void listClear(LinkedList *list)
{
    ListNode *n = list->head;
    while (n != NULL) {
        ListNode *next = n->next;
        free(n);
        n = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

static ListNode *nodeAt(const LinkedList *list, int idx)
{
    if (list == NULL || idx < 0)
        return NULL;

    ListNode *n = list->head;
    while (n != NULL && idx > 0) {
        n = n->next;
        idx--;
    }
    return n;
}

static void detachNode(LinkedList *list, ListNode *n)
{
    if (n->prev != NULL)
        n->prev->next = n->next;
    else
        list->head = n->next;

    if (n->next != NULL)
        n->next->prev = n->prev;
    else
        list->tail = n->prev;

    n->next = NULL;
    n->prev = NULL;
}

static void attachSorted(LinkedList *list, ListNode *n)
{
    ListNode *cur = list->head;
    while (cur != NULL && listCompare(&cur->contact, &n->contact) <= 0)
        cur = cur->next;

    if (cur == NULL) {
        n->next = NULL;
        n->prev = list->tail;
        if (list->tail != NULL)
            list->tail->next = n;
        else
            list->head = n;
        list->tail = n;
    } else {
        n->next = cur;
        n->prev = cur->prev;
        if (cur->prev != NULL)
            cur->prev->next = n;
        else
            list->head = n;
        cur->prev = n;
    }
}

int listInsert(LinkedList *list, const Contact *contact)
{
    if (list == NULL || contact == NULL)
        return -1;

    ListNode *n = malloc(sizeof(*n));
    if (n == NULL)
        return -1;

    n->contact = *contact;
    n->next = NULL;
    n->prev = NULL;
    attachSorted(list, n);
    return 0;
}

int listIndexOf(const LinkedList *list, const char *family, const char *name)
{
    if (list == NULL || family == NULL || name == NULL)
        return -1;

    int i = 0;
    for (const ListNode *n = list->head; n != NULL; n = n->next, i++) {
        if (strcmp(n->contact.family, family) == 0 &&
            strcmp(n->contact.name, name) == 0)
            return i;
    }
    return -1;
}

int listRemove(LinkedList *list, const char *family, const char *name)
{
    int idx = listIndexOf(list, family, name);
    if (idx == -1)
        return -1;

    ListNode *n = nodeAt(list, idx);
    detachNode(list, n);
    free(n);
    return 0;
}

const Contact *listGet(const LinkedList *list, int idx)
{
    const ListNode *n = nodeAt(list, idx);
    return n != NULL ? &n->contact : NULL;
}

int listSet(LinkedList *list, int idx, const Contact *contact)
{
    if (contact == NULL)
        return -1;

    ListNode *n = nodeAt(list, idx);
    if (n == NULL)
        return -1;

    detachNode(list, n);
    n->contact = *contact;
    attachSorted(list, n);
    return 0;
}

static void printField(const char *key, const char *value)
{
    if (value[0] != '\0')
        printf("  %s: %s\n", key, value);
}

void printList(const LinkedList *list)
{
    int i = 0;
    for (const ListNode *n = list->head; n != NULL; n = n->next) {
        const Contact *c = &n->contact;
        printf("Контакт №%d:\n", ++i);
        printField("фамилия", c->family);
        printField("имя", c->name);
        printField("отчество", c->surname);
        printField("место работы", c->workplace);
        printField("должность", c->position);
        printField("телефон", c->phone_number);
        printField("email", c->emails);
    }
}