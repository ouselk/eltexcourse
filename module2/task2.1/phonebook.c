#include <stddef.h>
#include "phonebook.h"

/* Пустой контакт: все символы всех строк равны '\0'. */
static const Contact EMPTY_CONTACT = {0};

int isContactEmpty(const Contact *contact)
{
    return contact->family[0] == '\0';
}

static int validIndex(const ContactBook *cb, int idx)
{
    return cb != NULL
        && cb->size > 0
        && cb->size <= CONTACT_BOOK_SIZE
        && idx >= 0
        && idx < cb->size;
}

int addContact(ContactBook *cb, const Contact *contact, int idx)
{
    if (contact == NULL || !validIndex(cb, idx))
        return -1;
    if (contact->family[0] == '\0' || contact->name[0] == '\0')
        return -1;                              /* обязательные поля */
    if (!isContactEmpty(&cb->contacts[idx]))
        return -1;                              /* ячейка занята */

    cb->contacts[idx] = *contact;
    return 0;
}

int delContact(ContactBook *cb, int idx)
{
    if (!validIndex(cb, idx))
        return -1;
    if (isContactEmpty(&cb->contacts[idx]))
        return -1;                              /* удалять нечего */

    cb->contacts[idx] = EMPTY_CONTACT;
    return 0;
}

int updateContact(ContactBook *cb, int idx, const Contact *new_contact)
{
    if (new_contact == NULL || !validIndex(cb, idx))
        return -1;

    /* Копии нужны на случай, если new_contact указывает внутрь cb,
       и чтобы откатиться, если добавление не удастся. */
    Contact fresh  = *new_contact;
    Contact backup = cb->contacts[idx];

    if (delContact(cb, idx) != 0)
        return -1;
    if (addContact(cb, &fresh, idx) != 0) {
        cb->contacts[idx] = backup;
        return -1;
    }
    return 0;
}