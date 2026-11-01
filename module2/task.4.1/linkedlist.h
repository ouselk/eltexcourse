#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "phonebook.h"

typedef struct ListNode ListNode;

struct ListNode {
    ListNode *next;
    ListNode *prev;
    Contact contact;
};

typedef struct LinkedList {
    ListNode *tail;
    ListNode *head;
} LinkedList;

void listInit(LinkedList *list);

void listClear(LinkedList *list);

int listSize(const LinkedList *list);

int listGet(const LinkedList *list, int idx, Contact *out);

int listSet(LinkedList *list, int idx, const Contact *contact);

int listInsert(LinkedList *list, const Contact *contact);

int listRemove(LinkedList *list, int idx);

int listFind(const LinkedList *list, const char *family, const char *name);

void printList(const LinkedList *list);

#endif