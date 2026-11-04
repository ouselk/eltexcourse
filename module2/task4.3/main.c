#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "phonebook.h"

static int readLine(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, (int)size, stdin) == NULL)
        return -1;

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }
    return 0;
}

static int readRequired(const char *prompt, char *buf, size_t size)
{
    for (;;) {
        if (readLine(prompt, buf, size) != 0)
            return -1;
        if (buf[0] != '\0')
            return 0;
        printf("Это поле обязательно для заполнения.\n");
    }
}

int createContactMenu(Contact *contact)
{
    if (contact == NULL)
        return -1;

    memset(contact, 0, sizeof(*contact));

    if (readRequired("Фамилия (обязательно): ", contact->family, sizeof(contact->family)) != 0)
        return -1;
    if (readRequired("Имя (обязательно): ", contact->name, sizeof(contact->name)) != 0)
        return -1;
    if (readLine("Отчество: ", contact->surname, sizeof(contact->surname)) != 0)
        return -1;
    if (readLine("Место работы: ", contact->workplace, sizeof(contact->workplace)) != 0)
        return -1;
    if (readLine("Должность: ", contact->position, sizeof(contact->position)) != 0)
        return -1;
    if (readLine("Телефон(ы) (через запятую): ", contact->phone_number, sizeof(contact->phone_number)) != 0)
        return -1;
    if (readLine("Email(ы) (через запятую): ", contact->emails, sizeof(contact->emails)) != 0)
        return -1;

    return 0;
}

void addContactMenu(ContactBook *cb)
{
    Contact c;

    if (createContactMenu(&c) != 0) {
        printf("Ошибка: не удалось создать контакт.\n");
        return;
    }

    if (addContact(cb, &c) == 0)
        printf("Контакт успешно добавлен.\n");
    else
        printf("Ошибка: контакт не добавлен (книга заполнена или такой контакт уже существует).\n");
}

void delContactMenu(ContactBook *cb)
{
    char family[NAME_LEN], name[NAME_LEN];

    if (readLine("Фамилия удаляемого контакта: ", family, sizeof(family)) != 0 ||
        readLine("Имя удаляемого контакта: ", name, sizeof(name)) != 0) {
        printf("Ошибка ввода.\n");
        return;
    }

    if (findContact(cb, family, name, NULL) == -1) {
        printf("Такого контакта не существует\n");
        return;
    }

    if (delContact(cb, family, name) != 0) {
        printf("Ошибка: не удалось удалить контакт.\n");
        return;
    }
    printf("Контакт удалён.\n");
}

static void mergeField(char *dst, size_t size, const char *old_val, const char *new_val)
{
    if (strcmp(new_val, "-") == 0)
        dst[0] = '\0';
    else if (new_val[0] == '\0')
        snprintf(dst, size, "%s", old_val);
    else
        snprintf(dst, size, "%s", new_val);
}

void updateContactMenu(ContactBook *cb)
{
    char family[NAME_LEN], name[NAME_LEN];

    if (readLine("Фамилия редактируемого контакта: ", family, sizeof(family)) != 0 ||
        readLine("Имя редактируемого контакта: ", name, sizeof(name)) != 0) {
        printf("Ошибка ввода.\n");
        return;
    }

    Contact old;
    if (findContact(cb, family, name, &old) == -1) {
        printf("Ошибка: такого контакта не существует.\n");
        return;
    }

    printf("\nВведите новые данные контакта.\n"
           "  - Фамилия и имя обязательны (введите их заново, можно прежние).\n"
           "  - Enter: оставить прежнее значение поля.\n"
           "  - '-' : удалить значение поля.\n\n");

    Contact entered;
    if (createContactMenu(&entered) != 0) {
        printf("Ошибка: не удалось создать контакт.\n");
        return;
    }

    Contact merged;
    memset(&merged, 0, sizeof(merged));

    snprintf(merged.family, sizeof(merged.family), "%s", entered.family);
    snprintf(merged.name, sizeof(merged.name), "%s", entered.name);

    mergeField(merged.surname,      sizeof(merged.surname),      old.surname,      entered.surname);
    mergeField(merged.workplace,    sizeof(merged.workplace),    old.workplace,    entered.workplace);
    mergeField(merged.position,     sizeof(merged.position),     old.position,     entered.position);
    mergeField(merged.phone_number, sizeof(merged.phone_number), old.phone_number, entered.phone_number);
    mergeField(merged.emails,       sizeof(merged.emails),       old.emails,       entered.emails);

    if (updateContact(cb, family, name, &merged) == 0)
        printf("Контакт успешно обновлён.\n");
    else
        printf("Ошибка: не удалось обновить контакт (возможно, контакт с такими фамилией и именем уже существует).\n");
}

static void printMenu()
{
    printf("\n=== Телефонная книга ===\n"
           "1. Добавить контакт\n"
           "2. Редактировать контакт\n"
           "3. Удалить контакт\n"
           "4. Показать все контакты\n"
           "0. Выход\n");
}

int main()
{
    static ContactBook book;
    initContactBook(&book);

    char buf[16];
    for (;;) {
        printMenu();
        if (readLine("Выбор: ", buf, sizeof(buf)) != 0)
            break;

        switch (atoi(buf)) {
        case 1: addContactMenu(&book);    break;
        case 2: updateContactMenu(&book); break;
        case 3: delContactMenu(&book);    break;
        case 4: printContacts(&book);     break;
        case 0: return 0;
        default: printf("Неверный пункт меню.\n");
        }
    }
    return 0;
}