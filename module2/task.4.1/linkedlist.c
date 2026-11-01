#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

static int compareKeys(const Contact *c, const char *family, const char *name)
{
    int cmp = strcmp(c->family, family);
    if (cmp != 0)
        return cmp;
    return strcmp(c->name, name);
}

static void linkSorted(LinkedList *list, ListNode *node)
{
    ListNode *cur = list->head;
    while (cur != NULL
           && compareKeys(&cur->contact, node->contact.family, node->contact.name) <= 0)
        cur = cur->next;

    if (cur == NULL) {
        node->next = NULL;
        node->prev = list->tail;
        if (list->tail != NULL)
            list->tail->next = node;
        else
            list->head = node;
        list->tail = node;
    } else {
        node->next = cur;
        node->prev = cur->prev;
        if (cur->prev != NULL)
            cur->prev->next = node;
        else
            list->head = node;
        cur->prev = node;
    }
}

static void unlinkNode(LinkedList *list, ListNode *node)
{
    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    node->next = NULL;
    node->prev = NULL;
}

static ListNode *nodeAt(const LinkedList *list, int idx)
{
    if (list == NULL || idx < 0)
        return NULL;

    ListNode *node = list->head;
    while (node != NULL && idx > 0) {
        node = node->next;
        idx--;
    }
    return node;
}

void listInit(LinkedList *list)
{
    if (list == NULL)
        return;
    list->head = NULL;
    list->tail = NULL;
}

void listClear(LinkedList *list)
{
    if (list == NULL)
        return;

    ListNode *node = list->head;
    while (node != NULL) {
        ListNode *next = node->next;
        free(node);
        node = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

int listSize(const LinkedList *list)
{
    if (list == NULL)
        return 0;

    int n = 0;
    for (const ListNode *node = list->head; node != NULL; node = node->next)
        n++;
    return n;
}

int listGet(const LinkedList *list, int idx, Contact *out)
{
    if (out == NULL)
        return -1;

    const ListNode *node = nodeAt(list, idx);
    if (node == NULL)
        return -1;

    *out = node->contact;
    return 0;
}

int listSet(LinkedList *list, int idx, const Contact *contact)
{
    if (contact == NULL)
        return -1;

    ListNode *node = nodeAt(list, idx);
    if (node == NULL)
        return -1;

    Contact copy = *contact;
    unlinkNode(list, node);
    node->contact = copy;
    linkSorted(list, node);
    return 0;
}

int listInsert(LinkedList *list, const Contact *contact)
{
    if (list == NULL || contact == NULL)
        return -1;

    ListNode *node = malloc(sizeof(ListNode));
    if (node == NULL)
        return -1;

    node->contact = *contact;
    linkSorted(list, node);
    return 0;
}

int listRemove(LinkedList *list, int idx)
{
    ListNode *node = nodeAt(list, idx);
    if (node == NULL)
        return -1;

    unlinkNode(list, node);
    free(node);
    return 0;
}

int listFind(const LinkedList *list, const char *family, const char *name)
{
    if (list == NULL || family == NULL || name == NULL)
        return -1;

    int i = 0;
    for (const ListNode *node = list->head; node != NULL; node = node->next, i++) {
        int cmp = compareKeys(&node->contact, family, name);
        if (cmp == 0)
            return i;
        if (cmp > 0)
            break;
    }
    return -1;
}

void printList(const LinkedList *list)
{
    if (list == NULL)
        return;

    int n = 0;
    for (const ListNode *node = list->head; node != NULL; node = node->next) {
        const Contact *c = &node->contact;
        printf("Контакт №%d:\n", ++n);
        if (c->family[0] != '\0')       printf("  фамилия: %s\n", c->family);
        if (c->name[0] != '\0')         printf("  имя: %s\n", c->name);
        if (c->surname[0] != '\0')      printf("  отчество: %s\n", c->surname);
        if (c->workplace[0] != '\0')    printf("  место работы: %s\n", c->workplace);
        if (c->position[0] != '\0')     printf("  должность: %s\n", c->position);
        if (c->phone_number[0] != '\0') printf("  телефон: %s\n", c->phone_number);
        if (c->emails[0] != '\0')       printf("  email: %s\n", c->emails);
    }
}