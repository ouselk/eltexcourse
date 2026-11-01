#include <stddef.h>
#include <string.h>
#include "phonebook.h"

static int isEmptySlot(const Contact *contact)
{
    return contact->family[0] == '\0';
}

static int slotLimit(const ContactBook *cb)
{
    if (cb->size < 0)
        return 0;
    return cb->size > CONTACT_BOOK_SIZE ? CONTACT_BOOK_SIZE : cb->size;
}

static int slotOf(const ContactBook *cb, int idx)
{
    if (cb == NULL || idx < 0)
        return -1;

    int n = 0;
    for (int i = 0; i < slotLimit(cb); i++) {
        if (isEmptySlot(&cb->contacts[i]))
            continue;
        if (n == idx)
            return i;
        n++;
    }
    return -1;
}

void initContactBook(ContactBook *cb)
{
    if (cb == NULL)
        return;

    cb->size = CONTACT_BOOK_SIZE;
    for (int i = 0; i < CONTACT_BOOK_SIZE; i++)
        cb->contacts[i] = (Contact){0};
}

int getContactCount(const ContactBook *cb)
{
    if (cb == NULL)
        return 0;

    int n = 0;
    for (int i = 0; i < slotLimit(cb); i++) {
        if (!isEmptySlot(&cb->contacts[i]))
            n++;
    }
    return n;
}

int isContactBookFull(const ContactBook *cb)
{
    return cb == NULL || getContactCount(cb) >= slotLimit(cb);
}

int getContact(const ContactBook *cb, int idx, Contact *out)
{
    if (out == NULL)
        return -1;

    int slot = slotOf(cb, idx);
    if (slot < 0)
        return -1;

    *out = cb->contacts[slot];
    return 0;
}

int findContact(const ContactBook *cb, const char *family, const char *name)
{
    if (cb == NULL || family == NULL || name == NULL)
        return -1;

    int n = 0;
    for (int i = 0; i < slotLimit(cb); i++) {
        if (isEmptySlot(&cb->contacts[i]))
            continue;
        if (strcmp(cb->contacts[i].family, family) == 0
            && strcmp(cb->contacts[i].name, name) == 0)
            return n;
        n++;
    }
    return -1;
}

int addElemInArray(ContactBook *cb, const Contact *contact, int idx)
{
    if (cb == NULL || contact == NULL)
        return -1;
    if (idx < 0 || idx >= slotLimit(cb))
        return -1;
    if (contact->family[0] == '\0' || contact->name[0] == '\0')
        return -1;
    if (!isEmptySlot(&cb->contacts[idx]))
        return -1;

    cb->contacts[idx] = *contact;
    return 0;
}

int addContact(ContactBook *cb, const Contact *contact)
{
    if (cb == NULL || contact == NULL)
        return -1;

    for (int i = 0; i < slotLimit(cb); i++) {
        if (isEmptySlot(&cb->contacts[i]))
            return addElemInArray(cb, contact, i);
    }
    return -1;
}

int delContact(ContactBook *cb, int idx)
{
    int slot = slotOf(cb, idx);
    if (slot < 0)
        return -1;

    cb->contacts[slot] = (Contact){0};
    return 0;
}

int updateContact(ContactBook *cb, int idx, const Contact *new_contact)
{
    if (cb == NULL || new_contact == NULL)
        return -1;

    int slot = slotOf(cb, idx);
    if (slot < 0)
        return -1;

    Contact fresh = *new_contact;
    Contact backup = cb->contacts[slot];

    if (delContact(cb, idx) != 0)
        return -1;
    if (addElemInArray(cb, &fresh, slot) != 0) {
        cb->contacts[slot] = backup;
        return -1;
    }
    return 0;
}