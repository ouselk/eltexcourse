#include <stddef.h>
#include <string.h>
#include "phonebook.h"

void initContactBook(ContactBook *cb)
{
    if (cb == NULL)
        return;
    cb->size = CONTACT_BOOK_SIZE;
    cb->count = 0;
}

int getContactCount(const ContactBook *cb)
{
    return cb != NULL ? cb->count : 0;
}

int isContactBookFull(const ContactBook *cb)
{
    return cb == NULL || cb->count >= cb->size;
}

int getContact(const ContactBook *cb, int idx, Contact *out)
{
    if (cb == NULL || out == NULL || idx < 0 || idx >= cb->count)
        return -1;

    *out = cb->contacts[idx];
    return 0;
}

int findContact(const ContactBook *cb, const char *family, const char *name)
{
    if (cb == NULL || family == NULL || name == NULL)
        return -1;

    for (int i = 0; i < cb->count; i++) {
        if (strcmp(cb->contacts[i].family, family) == 0
            && strcmp(cb->contacts[i].name, name) == 0)
            return i;
    }
    return -1;
}

int addContact(ContactBook *cb, const Contact *contact, int idx)
{
    if (cb == NULL || contact == NULL)
        return -1;
    if (cb->count >= cb->size)
        return -1;
    if (idx < 0 || idx > cb->count)
        return -1;
    if (contact->family[0] == '\0' || contact->name[0] == '\0')
        return -1;

    Contact copy = *contact;

    for (int i = cb->count; i > idx; i--)
        cb->contacts[i] = cb->contacts[i - 1];

    cb->contacts[idx] = copy;
    cb->count++;
    return 0;
}

int delContact(ContactBook *cb, int idx)
{
    if (cb == NULL || idx < 0 || idx >= cb->count)
        return -1;

    for (int i = idx; i < cb->count - 1; i++)
        cb->contacts[i] = cb->contacts[i + 1];

    cb->count--;
    cb->contacts[cb->count] = (Contact){0};
    return 0;
}

int updateContact(ContactBook *cb, int idx, const Contact *new_contact)
{
    if (cb == NULL || new_contact == NULL)
        return -1;

    Contact fresh = *new_contact;
    Contact backup;

    if (getContact(cb, idx, &backup) != 0)
        return -1;
    if (delContact(cb, idx) != 0)
        return -1;
    if (addContact(cb, &fresh, idx) != 0) {
        addContact(cb, &backup, idx);
        return -1;
    }
    return 0;
}