#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"
#include "weights.h"

#ifdef LIST_COUNT_STEPS
unsigned long long g_list_steps = 0;
#define STEP() (g_list_steps++)
#else
#define STEP() ((void)0)
#endif

int listCompare(const Contact *a, const Contact *b)
{
    int r = strcmp(a->family, b->family);
    if (r != 0)
        return r;
    return strcmp(a->name, b->name);
}

static int keyCompare(const Contact *c, const char *family, const char *name)
{
    int r = strcmp(c->family, family);
    if (r != 0)
        return r;
    return strcmp(c->name, name);
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
        STEP();
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

/* Вставляет n сразу после prev (prev == NULL - в начало списка). */
static void linkAfter(LinkedList *list, ListNode *prev, ListNode *n)
{
    n->prev = prev;
    n->next = (prev != NULL) ? prev->next : list->head;

    if (n->next != NULL)
        n->next->prev = n;
    else
        list->tail = n;

    if (prev != NULL)
        prev->next = n;
    else
        list->head = n;
}

/* ---------- исходная (неоптимизированная) версия ---------- */

static void attachSorted(LinkedList *list, ListNode *n)
{
    ListNode *cur = list->head;
    while (cur != NULL && listCompare(&cur->contact, &n->contact) <= 0) {
        STEP();
        cur = cur->next;
    }

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
        STEP();
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

/* ---------- оптимизация: выбор стороны обхода по весам первых букв ---------- */

/* Индекс первой буквы фамилии в таблице весов (порядок байтов UTF-8: Ё, А..Я) или -1. */
static int letterIndex(const char *s)
{
    unsigned char b0 = (unsigned char)s[0];
    unsigned char b1 = (b0 != 0) ? (unsigned char)s[1] : 0;

    if (b0 != 0xD0)
        return -1;
    if (b1 == 0x81)                    /* Ё */
        return 0;
    if (b1 >= 0x90 && b1 <= 0xAF)      /* А..Я */
        return 1 + (b1 - 0x90);
    return -1;
}

/* 1 - обход с конца, 0 - с начала.
   Суммируем веса букв до первой буквы фамилии и после неё; если "до" больше,
   то перед нашим элементом больше записей, значит с конца идти короче.
   При равенстве (и для нестандартной первой буквы) идём с начала. */
static int preferTail(const char *family)
{
    static double prefix[LETTER_COUNT + 1];  /* prefix[i] = сумма весов букв с индексами < i */
    static int ready = 0;

    if (!ready) {
        prefix[0] = 0.0;
        for (int i = 0; i < LETTER_COUNT; i++)
            prefix[i + 1] = prefix[i] + LETTER_WEIGHTS[i];
        ready = 1;
    }

    int idx = letterIndex(family);
    if (idx < 0)
        return 0;

    double before = prefix[idx];
    double after = prefix[LETTER_COUNT] - prefix[idx + 1];
    return before > after;
}

int listInsertEx(LinkedList *list, const Contact *contact, ListSearchMode mode)
{
    if (list == NULL || contact == NULL)
        return -1;

    if (mode == LIST_SEARCH_BASELINE)
        return listInsert(list, contact);   /* проверка дубликата делается снаружи, как раньше */

    int fromTail = (mode == LIST_SEARCH_WEIGHTED) && preferTail(contact->family);
    ListNode *prev;  /* после какого узла вставляем (NULL - в начало) */

    if (fromTail) {
        ListNode *cur = list->tail;
        while (cur != NULL && listCompare(&cur->contact, contact) > 0) {
            STEP();
            cur = cur->prev;
        }
        if (cur != NULL && listCompare(&cur->contact, contact) == 0)
            return 1;
        prev = cur;
    } else {
        ListNode *cur = list->head;
        while (cur != NULL && listCompare(&cur->contact, contact) < 0) {
            STEP();
            cur = cur->next;
        }
        if (cur != NULL && listCompare(&cur->contact, contact) == 0)
            return 1;
        prev = (cur != NULL) ? cur->prev : list->tail;
    }

    ListNode *n = malloc(sizeof(*n));
    if (n == NULL)
        return -1;
    n->contact = *contact;
    linkAfter(list, prev, n);
    return 0;
}

int listRemoveEx(LinkedList *list, const char *family, const char *name, ListSearchMode mode)
{
    if (list == NULL || family == NULL || name == NULL)
        return -1;

    if (mode == LIST_SEARCH_BASELINE)
        return listRemove(list, family, name);

    int fromTail = (mode == LIST_SEARCH_WEIGHTED) && preferTail(family);
    ListNode *cur;

    if (fromTail) {
        cur = list->tail;
        while (cur != NULL && keyCompare(&cur->contact, family, name) > 0) {
            STEP();
            cur = cur->prev;
        }
    } else {
        cur = list->head;
        while (cur != NULL && keyCompare(&cur->contact, family, name) < 0) {
            STEP();
            cur = cur->next;
        }
    }

    if (cur == NULL || keyCompare(&cur->contact, family, name) != 0)
        return -1;

    detachNode(list, cur);
    free(cur);
    return 0;
}

int listAppendSorted(LinkedList *list, const Contact *contact)
{
    if (list == NULL || contact == NULL)
        return -1;
    if (list->tail != NULL && listCompare(&list->tail->contact, contact) >= 0)
        return -1;

    ListNode *n = malloc(sizeof(*n));
    if (n == NULL)
        return -1;
    n->contact = *contact;
    linkAfter(list, list->tail, n);
    return 0;
}

/* ---------- вывод ---------- */

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
