#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "linkedlist.h"
#include "contact.h"

#define CONTACT_BOOK_SIZE 200000

typedef struct {
    LinkedList list;
    int size;               /* текущее количество контактов */
} ContactBook;

void initContactBook(ContactBook *cb);
void freeContactBook(ContactBook *cb);
int findContact(const ContactBook *cb, const char *family, const char *name);
const Contact *getContactAt(const ContactBook *cb, int idx);

/* addContact / delContact используют оптимизированный поиск (LIST_SEARCH_WEIGHTED). */
int addContact(ContactBook *cb, const Contact *contact);
int delContact(ContactBook *cb, const char *family, const char *name);

/* То же с явным выбором стратегии (LIST_SEARCH_BASELINE - исходная реализация). */
int addContactEx(ContactBook *cb, const Contact *contact, ListSearchMode mode);
int delContactEx(ContactBook *cb, const char *family, const char *name, ListSearchMode mode);

/* Быстрая загрузка уже отсортированных данных (добавление в конец). */
int appendContactSorted(ContactBook *cb, const Contact *contact);

int updateContact(ContactBook *cb, const char *family, const char *name,
                  const Contact *new_contact);

void printContacts(const ContactBook *cb);

#endif
