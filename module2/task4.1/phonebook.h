#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "linkedlist.h"
#include "contact.h"

#define CONTACT_BOOK_SIZE 100



typedef struct {
    LinkedList list;
    int size;               /* текущее количество контактов */
} ContactBook;

void initContactBook(ContactBook *cb);
void freeContactBook(ContactBook *cb);
int findContact(const ContactBook *cb, const char *family, const char *name);
const Contact *getContactAt(const ContactBook *cb, int idx);
int addContact(ContactBook *cb, const Contact *contact);
int delContact(ContactBook *cb, const char *family, const char *name);
int updateContact(ContactBook *cb, const char *family, const char *name,
                  const Contact *new_contact);

void printContacts(const ContactBook *cb);

#endif