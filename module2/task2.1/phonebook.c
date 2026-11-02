#include <stdio.h>
#include <string.h>
#include "phonebook.h"

static int isUsed(const Contact *c)
{
    return c->family[0] != '\0';
}

void initContactBook(ContactBook *cb)
{
    memset(cb, 0, sizeof(*cb));
    cb->size = CONTACT_BOOK_SIZE;
}

int findContact(const ContactBook *cb, const char *family, const char *name,
                Contact *out)
{
    if (cb == NULL || family == NULL || name == NULL)
        return -1;

    for (int i = 0; i < cb->size; i++) {
        if (isUsed(&cb->contacts[i]) &&
            strcmp(cb->contacts[i].family, family) == 0 &&
            strcmp(cb->contacts[i].name, name) == 0) {
            if (out != NULL)
                *out = cb->contacts[i];
            return i;
        }
    }
    return -1;
}

int addContact(ContactBook *cb, const Contact *contact)
{
    if (cb == NULL || contact == NULL)
        return -1;

    /* фамилия и имя обязательны */
    if (contact->family[0] == '\0' || contact->name[0] == '\0')
        return -1;

    /* контакт с такими Ф и И уже есть */
    if (findContact(cb, contact->family, contact->name, NULL) != -1)
        return -1;

    for (int i = 0; i < cb->size; i++) {
        if (!isUsed(&cb->contacts[i])) {
            cb->contacts[i] = *contact;
            return 0;
        }
    }
    return -1;
}

int delContact(ContactBook *cb, const char *family, const char *name)
{
    int idx = findContact(cb, family, name, NULL);
    if (idx == -1)
        return -1;

    memset(&cb->contacts[idx], 0, sizeof(Contact));
    return 0;
}

int updateContact(ContactBook *cb, const char *family, const char *name,
                  const Contact *new_contact)
{
    if (cb == NULL || new_contact == NULL)
        return -1;

    Contact old;
    int idx = findContact(cb, family, name, &old);
    if (idx == -1)
        return -1;

    if (delContact(cb, family, name) != 0)
        return -1;

    if (addContact(cb, new_contact) != 0) {
        cb->contacts[idx] = old; /* откат */
        return -1;
    }
    return 0;
}

static void printField(const char *key, const char *value)
{
    if (value[0] != '\0')
        printf("  %s: %s\n", key, value);
}

void printContacts(const ContactBook *cb)
{
    int n = 0;
    for (int i = 0; i < cb->size; i++) {
        const Contact *c = &cb->contacts[i];
        if (!isUsed(c))
            continue;

        n++;
        printf("Контакт №%d:\n", n);
        printField("фамилия", c->family);
        printField("имя", c->name);
        printField("отчество", c->surname);
        printField("место работы", c->workplace);
        printField("должность", c->position);
        printField("телефон", c->phone_number);
        printField("email", c->emails);
    }
    if (n == 0)
        printf("Список контактов пуст.\n");
}