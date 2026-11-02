#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "contact.h"

typedef struct ListNode {
    Contact contact;
    struct ListNode *next;
    struct ListNode *prev;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
} LinkedList;

/* Как искать место для вставки / удаляемый элемент (в обоих режимах один проход с остановкой по порядку). */
typedef enum {
    LIST_SEARCH_HEAD,      /* исходный вариант: всегда идём с головы */
    LIST_SEARCH_WEIGHTED   /* оптимизация: сторону обхода выбираем по таблице весов первых букв */
} ListSearchMode;

#ifdef LIST_COUNT_STEPS
extern unsigned long long g_list_steps;  /* число пройденных узлов (для бенчмарка) */
#endif

int listCompare(const Contact *a, const Contact *b);

void listInit(LinkedList *list);
void listClear(LinkedList *list);

int listIndexOf(const LinkedList *list, const char *family, const char *name);
const Contact *listGet(const LinkedList *list, int idx);
int listSet(LinkedList *list, int idx, const Contact *contact);
void printList(const LinkedList *list);

/* Вставка/удаление с выбором стратегии.
   listInsertEx: 0 - вставлено, 1 - такой контакт уже есть, -1 - ошибка.
   listRemoveEx: 0 - удалено, -1 - не найден / ошибка. */
int listInsertEx(LinkedList *list, const Contact *contact, ListSearchMode mode);
int listRemoveEx(LinkedList *list, const char *family, const char *name, ListSearchMode mode);

/* Добавление в конец без поиска; контакт должен быть строго больше последнего (для загрузки отсортированных данных). */
int listAppendSorted(LinkedList *list, const Contact *contact);

#endif
