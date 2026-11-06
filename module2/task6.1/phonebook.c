#include <stdio.h>
#include <string.h>
#include "phonebook.h"

void initContactBook(ContactBook *cb)
{
    listInit(&cb->list);
    cb->size = 0;
}

void freeContactBook(ContactBook *cb)
{
    listClear(&cb->list);
    cb->size = 0;
}

int findContact(const ContactBook *cb, const char *family, const char *name)
{
    if (cb == NULL)
        return -1;
    return listIndexOf(&cb->list, family, name);
}

const Contact *getContactAt(const ContactBook *cb, int idx)
{
    if (cb == NULL)
        return NULL;
    return listGet(&cb->list, idx);
}

int addContact(ContactBook *cb, const Contact *contact)
{
    if (cb == NULL || contact == NULL)
        return -1;

    if (contact->family[0] == '\0' || contact->name[0] == '\0')
        return -1;

    if (cb->size >= CONTACT_BOOK_SIZE)
        return -1;

    if (findContact(cb, contact->family, contact->name) != -1)
        return -1;

    if (listInsert(&cb->list, contact) != 0)
        return -1;

    cb->size++;
    return 0;
}

int delContact(ContactBook *cb, const char *family, const char *name)
{
    if (cb == NULL)
        return -1;

    if (listRemove(&cb->list, family, name) != 0)
        return -1;

    cb->size--;
    return 0;
}

int updateContact(ContactBook *cb, const char *family, const char *name,
                  const Contact *new_contact)
{
    if (cb == NULL || new_contact == NULL)
        return -1;

    if (new_contact->family[0] == '\0' || new_contact->name[0] == '\0')
        return -1;

    int idx = findContact(cb, family, name);
    if (idx == -1)
        return -1;

    int other = findContact(cb, new_contact->family, new_contact->name);
    if (other != -1 && other != idx)
        return -1;

    return listSet(&cb->list, idx, new_contact);
}

void printContacts(const ContactBook *cb)
{
    if (cb == NULL || cb->size == 0) {
        printf("Список контактов пуст.\n");
        return;
    }
    printList(&cb->list);
}