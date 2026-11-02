#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "contact.h"

typedef struct ListNode ListNode;

struct ListNode {
    ListNode *next;
    ListNode *prev;
    Contact contact;
};

typedef struct {
    ListNode *tail;
    ListNode *head;
} LinkedList;

int listCompare(const Contact *a, const Contact *b);

void listInit(LinkedList *list);
void listClear(LinkedList *list);
int listInsert(LinkedList *list, const Contact *contact);
int listRemove(LinkedList *list, const char *family, const char *name);
int listIndexOf(const LinkedList *list, const char *family, const char *name);
const Contact *listGet(const LinkedList *list, int idx);
int listSet(LinkedList *list, int idx, const Contact *contact);
void printList(const LinkedList *list);

#endif