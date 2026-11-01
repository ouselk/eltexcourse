#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "phonebook.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* ---------- ввод ---------- */

/* Читает строку из stdin, обрезая пробелы по краям.
   0 - ок, 1 - строка не поместилась в буфер, -1 - конец ввода/ошибка. */
static int readLine(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buf, (int)size, stdin) == NULL)
        return -1;

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    } else if (len == size - 1) {
        int ch = getchar();
        if (ch != '\n' && ch != EOF) {          /* в строке есть ещё символы */
            while (ch != '\n' && ch != EOF)
                ch = getchar();
            buf[0] = '\0';
            return 1;
        }
    }

    /* обрезаем пробелы справа */
    while (len > 0 && isspace((unsigned char)buf[len - 1]))
        buf[--len] = '\0';

    /* обрезаем пробелы слева: сдвигаем строку к началу буфера */
    size_t start = 0;
    while (buf[start] != '\0' && isspace((unsigned char)buf[start]))
        start++;
    if (start > 0) {
        size_t i = 0;
        while (buf[start + i] != '\0') {
            buf[i] = buf[start + i];
            i++;
        }
        buf[i] = '\0';
    }

    return 0;
}

/* ---------- вспомогательные ---------- */

/* Индекс контакта с такими фамилией и именем или -1. */
static int findContact(const ContactBook *cb, const char *family, const char *name)
{
    for (int i = 0; i < cb->size; i++) {
        if (!isContactEmpty(&cb->contacts[i])
            && strcmp(cb->contacts[i].family, family) == 0
            && strcmp(cb->contacts[i].name, name) == 0)
            return i;
    }
    return -1;
}

/* ---------- вывод ---------- */

void printContacts(ContactBook *cb)
{
    int n = 0;
    for (int i = 0; i < cb->size; i++) {
        const Contact *c = &cb->contacts[i];
        if (isContactEmpty(c))
            continue;
        printf("Контакт №%d:\n", ++n);
        if (c->family[0] != '\0')       printf("  фамилия: %s\n", c->family);
        if (c->name[0] != '\0')         printf("  имя: %s\n", c->name);
        if (c->surname[0] != '\0')      printf("  отчество: %s\n", c->surname);
        if (c->workplace[0] != '\0')    printf("  место работы: %s\n", c->workplace);
        if (c->position[0] != '\0')     printf("  должность: %s\n", c->position);
        if (c->phone_number[0] != '\0') printf("  телефон: %s\n", c->phone_number);
        if (c->emails[0] != '\0')       printf("  email: %s\n", c->emails);
    }
    if (n == 0)
        printf("Контактная книга пуста.\n");
}

/* ---------- меню ---------- */

/* Запрашивает данные контакта. Значения, уже лежащие в contact, считаются
   текущими: Enter оставляет текущее значение поля, "-" очищает необязательное
   поле. Для нового контакта вызывающий передаёт обнулённую структуру,
   для редактирования - копию старого контакта.
   Фамилия и имя (первые два поля) не могут остаться пустыми.
   0 - успех, -1 - ошибка. */
int createContactMenu(Contact *contact)
{
    if (contact == NULL)
        return -1;

    struct {
        const char *prompt;
        char *buf;
        size_t size;
    } fields[] = {
        {"Фамилия: ",      contact->family,       sizeof(contact->family)},
        {"Имя: ",          contact->name,         sizeof(contact->name)},
        {"Отчество: ",     contact->surname,      sizeof(contact->surname)},
        {"Место работы: ", contact->workplace,    sizeof(contact->workplace)},
        {"Должность: ",    contact->position,     sizeof(contact->position)},
        {"Телефон: ",      contact->phone_number, sizeof(contact->phone_number)},
        {"Email: ",        contact->emails,       sizeof(contact->emails)},
    };
    const int count = (int)(sizeof(fields) / sizeof(fields[0]));
    const int required = 2;                 /* фамилия и имя */
    char input[EMAIL_LEN];                  /* не меньше самого большого поля */

    if (contact->family[0] == '\0')
        printf("Обязательные поля: фамилия и имя. Остальные можно пропустить (Enter).\n");

    for (int i = 0; i < count; i++) {
        for (;;) {
            int rc = readLine(fields[i].prompt, input, fields[i].size);
            if (rc < 0)
                return -1;
            if (rc == 1) {
                printf("Слишком длинное значение (максимум %zu байт). Повторите ввод.\n",
                       fields[i].size - 1);
                continue;
            }
            if (input[0] == '\0') {
                if (i < required && fields[i].buf[0] == '\0') {
                    printf("Это поле обязательно для заполнения. Повторите ввод.\n");
                    continue;
                }
                break;                      /* оставляем текущее значение */
            }
            if (strcmp(input, "-") == 0) {
                if (i < required) {
                    printf("Фамилию и имя очистить нельзя. Повторите ввод.\n");
                    continue;
                }
                fields[i].buf[0] = '\0';
                break;
            }
            snprintf(fields[i].buf, fields[i].size, "%s", input);
            break;
        }
    }
    return 0;
}

void addContactMenu(ContactBook *cb)
{
    Contact c = {0};
    if (createContactMenu(&c) != 0) {
        printf("Ошибка: не удалось создать контакт.\n");
        return;
    }

    if (findContact(cb, c.family, c.name) != -1) {
        printf("Ошибка: контакт с такими фамилией и именем уже существует.\n");
        return;
    }

    int idx = -1;
    for (int i = 0; i < cb->size; i++) {
        if (isContactEmpty(&cb->contacts[i])) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        printf("Контактная книга переполнена\n");
        return;
    }

    if (addContact(cb, &c, idx) != 0) {
        printf("Ошибка: не удалось добавить контакт.\n");
        return;
    }
    printf("Контакт добавлен.\n");
}

void delContactMenu(ContactBook *cb)
{
    Contact key = {0};
    int rc;

    do {
        rc = readLine("Фамилия: ", key.family, sizeof(key.family));
        if (rc == 1)
            printf("Слишком длинное значение (максимум %zu байт). Повторите ввод.\n",
                   sizeof(key.family) - 1);
        else if (rc == 0 && key.family[0] == '\0')
            printf("Это поле обязательно для заполнения. Повторите ввод.\n");
    } while (rc == 1 || (rc == 0 && key.family[0] == '\0'));
    if (rc < 0) {
        printf("Ошибка ввода.\n");
        return;
    }

    do {
        rc = readLine("Имя: ", key.name, sizeof(key.name));
        if (rc == 1)
            printf("Слишком длинное значение (максимум %zu байт). Повторите ввод.\n",
                   sizeof(key.name) - 1);
        else if (rc == 0 && key.name[0] == '\0')
            printf("Это поле обязательно для заполнения. Повторите ввод.\n");
    } while (rc == 1 || (rc == 0 && key.name[0] == '\0'));
    if (rc < 0) {
        printf("Ошибка ввода.\n");
        return;
    }

    int idx = findContact(cb, key.family, key.name);
    if (idx == -1) {
        printf("Такого контакта не существует\n");
        return;
    }
    if (delContact(cb, idx) != 0) {
        printf("Ошибка: не удалось удалить контакт.\n");
        return;
    }
    printf("Контакт удалён.\n");
}

void updateContactMenu(ContactBook *cb)
{
    Contact key = {0};
    int rc;

    printf("Введите фамилию и имя контакта, который нужно изменить.\n");

    do {
        rc = readLine("Фамилия: ", key.family, sizeof(key.family));
        if (rc == 1)
            printf("Слишком длинное значение (максимум %zu байт). Повторите ввод.\n",
                   sizeof(key.family) - 1);
        else if (rc == 0 && key.family[0] == '\0')
            printf("Это поле обязательно для заполнения. Повторите ввод.\n");
    } while (rc == 1 || (rc == 0 && key.family[0] == '\0'));
    if (rc < 0) {
        printf("Ошибка ввода.\n");
        return;
    }

    do {
        rc = readLine("Имя: ", key.name, sizeof(key.name));
        if (rc == 1)
            printf("Слишком длинное значение (максимум %zu байт). Повторите ввод.\n",
                   sizeof(key.name) - 1);
        else if (rc == 0 && key.name[0] == '\0')
            printf("Это поле обязательно для заполнения. Повторите ввод.\n");
    } while (rc == 1 || (rc == 0 && key.name[0] == '\0'));
    if (rc < 0) {
        printf("Ошибка ввода.\n");
        return;
    }

    int idx = findContact(cb, key.family, key.name);
    if (idx == -1) {
        printf("Ошибка: такого контакта не существует.\n");
        return;
    }

    printf("Введите новые данные контакта.\n"
           "  Enter - оставить прежнее значение поля,\n"
           "  '-'   - удалить значение поля (кроме фамилии и имени).\n");

    /* Пропущенные поля остаются такими же, как в старом контакте. */
    Contact nc = cb->contacts[idx];
    if (createContactMenu(&nc) != 0) {
        printf("Ошибка: не удалось создать контакт.\n");
        return;
    }

    int other = findContact(cb, nc.family, nc.name);
    if (other != -1 && other != idx) {
        printf("Ошибка: контакт с такими фамилией и именем уже существует.\n");
        return;
    }

    if (updateContact(cb, idx, &nc) != 0) {
        printf("Ошибка: не удалось обновить контакт.\n");
        return;
    }
    printf("Контакт обновлён.\n");
}

/* ---------- main ---------- */

static void printMenu(void)
{
    printf("\n=== Телефонная книга ===\n"
           "1 - Показать контакты\n"
           "2 - Добавить контакт\n"
           "3 - Редактировать контакт\n"
           "4 - Удалить контакт\n"
           "0 - Выход\n");
}

int main(void)
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    static ContactBook cb;                  /* static: обнулена целиком */
    cb.size = CONTACT_BOOK_SIZE;

    for (;;) {
        char line[16];
        printMenu();
        if (readLine("Выбор: ", line, sizeof(line)) < 0)
            break;                          /* конец ввода */

        if (strcmp(line, "1") == 0)      printContacts(&cb);
        else if (strcmp(line, "2") == 0) addContactMenu(&cb);
        else if (strcmp(line, "3") == 0) updateContactMenu(&cb);
        else if (strcmp(line, "4") == 0) delContactMenu(&cb);
        else if (strcmp(line, "0") == 0) break;
        else printf("Неизвестная команда.\n");
    }

    printf("\nДо свидания!\n");
    return 0;
}