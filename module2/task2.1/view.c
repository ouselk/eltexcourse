/*
 * view.c — реализация ИНТЕРФЕЙСА.
 */
#include "view.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Читает строку, обрезает пробелы по краям, не даёт переполнить buf. */
static void read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    fflush(stdout);

    if (!fgets(buf, (int)size, stdin)) {
        buf[0] = '\0';
        return;
    }

    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    } else {
        /* строка длиннее буфера — сбрасываем остаток */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }

    while (len > 0 && isspace((unsigned char)buf[len - 1]))
        buf[--len] = '\0';

    size_t start = 0;
    while (buf[start] && isspace((unsigned char)buf[start]))
        start++;
    if (start > 0)
        memmove(buf, buf + start, strlen(buf + start) + 1);
}

/* Обязательное поле при добавлении: повторяем запрос до непустого значения. */
static void read_required(const char *prompt, char *buf, size_t size)
{
    for (;;) {
        read_line(prompt, buf, size);
        if (!str_is_blank(buf) || feof(stdin))
            return;
        puts("  Поле обязательно для заполнения.");
    }
}

/* Необязательное поле при добавлении. */
static void read_optional(const char *prompt, char *buf, size_t size)
{
    read_line(prompt, buf, size);
}

/* Список значений (телефоны, e-mail и т.п.): пустая строка — конец ввода. */
static void read_list(const char *label, char *base, size_t n, size_t len)
{
    char prompt[96];
    for (size_t i = 0; i < n; i++) {
        char *slot = base + i * len;
        snprintf(prompt, sizeof prompt, "%s %zu из %zu (Enter — пропустить): ",
                 label, i + 1, n);
        read_line(prompt, slot, len);
        if (slot[0] == '\0') {
            /* дальше не спрашиваем, остальные ячейки очищаем */
            for (size_t j = i + 1; j < n; j++)
                base[j * len] = '\0';
            return;
        }
    }
}

/* ---------- редактирование ---------- */

/* Строковое поле: Enter — не менять, "-" — очистить (если разрешено). */
static void edit_field(const char *label, char *field, size_t size, int required)
{
    char prompt[160];
    char tmp[TEXT_LEN > URL_LEN ? TEXT_LEN : URL_LEN];

    snprintf(prompt, sizeof prompt, "%s [%s]: ", label,
             field[0] ? field : "—");

    for (;;) {
        read_line(prompt, tmp, size < sizeof tmp ? size : sizeof tmp);
        if (tmp[0] == '\0')
            return;                          /* оставить как есть */
        if (strcmp(tmp, "-") == 0) {
            if (required) {
                puts("  Это поле обязательно, очистить нельзя.");
                if (feof(stdin)) return;
                continue;
            }
            field[0] = '\0';
            return;
        }
        strncpy(field, tmp, size - 1);
        field[size - 1] = '\0';
        return;
    }
}

/* Ответ «да»: поддерживаем UTF-8 «д/Д» (два байта) и латиницу «y/Y». */
static int is_yes(const char *s)
{
    return strncmp(s, "д", strlen("д")) == 0 ||
           strncmp(s, "Д", strlen("Д")) == 0 ||
           s[0] == 'y' || s[0] == 'Y';
}

/* Список значений: показываем текущие, спрашиваем, нужно ли вводить заново. */
static void edit_list(const char *label, char *base, size_t n, size_t len)
{
    char answer[8];

    printf("%s (текущие):", label);
    int any = 0;
    for (size_t i = 0; i < n; i++) {
        if (base[i * len]) {
            printf(" [%s]", base + i * len);
            any = 1;
        }
    }
    puts(any ? "" : " —");

    read_line("  Ввести заново? (д/н): ", answer, sizeof answer);
    if (is_yes(answer))
        read_list(label, base, n, len);
}

/* ---------- публичный интерфейс ---------- */

int view_menu(void)
{
    char buf[16];

    puts("\n===== ТЕЛЕФОННАЯ КНИГА =====");
    puts("1. Показать все контакты");
    puts("2. Добавить контакт");
    puts("3. Редактировать контакт");
    puts("4. Удалить контакт");
    puts("0. Выход");
    read_line("Выбор: ", buf, sizeof buf);

    if (feof(stdin) && buf[0] == '\0')
        return 0;
    return atoi(buf);
}

void view_message(const char *text)
{
    puts(text);
}

void view_print_contact(const Contact *c)
{
    printf("  ФИО:        %s %s%s%s\n", c->last_name, c->first_name,
           c->middle_name[0] ? " " : "", c->middle_name);
    if (c->workplace[0]) printf("  Работа:     %s\n", c->workplace);
    if (c->position[0])  printf("  Должность:  %s\n", c->position);

    for (size_t i = 0; i < MAX_PHONES; i++)
        if (c->phones[i][0])     printf("  Телефон:    %s\n", c->phones[i]);
    for (size_t i = 0; i < MAX_EMAILS; i++)
        if (c->emails[i][0])     printf("  E-mail:     %s\n", c->emails[i]);
    for (size_t i = 0; i < MAX_SOCIALS; i++)
        if (c->socials[i][0])    printf("  Соцсеть:    %s\n", c->socials[i]);
    for (size_t i = 0; i < MAX_MESSENGERS; i++)
        if (c->messengers[i][0]) printf("  Мессенджер: %s\n", c->messengers[i]);
}

void view_print_list(const PhoneBook *pb)
{
    size_t n = pb_count(pb);
    if (n == 0) {
        puts("Список контактов пуст.");
        return;
    }
    for (size_t i = 0; i < n; i++) {
        printf("\n--- Контакт №%zu ---\n", i + 1);
        view_print_contact(pb_get(pb, i));
    }
}

void view_read_contact(Contact *c)
{
    contact_init(c);
    puts("\n-- Новый контакт (* — обязательное поле) --");
    read_required("Фамилия*: ", c->last_name,  sizeof c->last_name);
    read_required("Имя*: ",     c->first_name, sizeof c->first_name);
    read_optional("Отчество: ",       c->middle_name, sizeof c->middle_name);
    read_optional("Место работы: ",   c->workplace,   sizeof c->workplace);
    read_optional("Должность: ",      c->position,    sizeof c->position);

    read_list("Телефон",           &c->phones[0][0],     MAX_PHONES,     PHONE_LEN);
    read_list("E-mail",            &c->emails[0][0],     MAX_EMAILS,     EMAIL_LEN);
    read_list("Страница в соцсети",&c->socials[0][0],    MAX_SOCIALS,    URL_LEN);
    read_list("Профиль в мессенджере", &c->messengers[0][0], MAX_MESSENGERS, URL_LEN);
}

void view_edit_contact(Contact *c)
{
    puts("\n-- Редактирование (Enter — оставить, \"-\" — очистить поле) --");
    edit_field("Фамилия*",     c->last_name,   sizeof c->last_name,   1);
    edit_field("Имя*",         c->first_name,  sizeof c->first_name,  1);
    edit_field("Отчество",     c->middle_name, sizeof c->middle_name, 0);
    edit_field("Место работы", c->workplace,   sizeof c->workplace,   0);
    edit_field("Должность",    c->position,    sizeof c->position,    0);

    edit_list("Телефон",               &c->phones[0][0],     MAX_PHONES,     PHONE_LEN);
    edit_list("E-mail",                &c->emails[0][0],     MAX_EMAILS,     EMAIL_LEN);
    edit_list("Страница в соцсети",    &c->socials[0][0],    MAX_SOCIALS,    URL_LEN);
    edit_list("Профиль в мессенджере", &c->messengers[0][0], MAX_MESSENGERS, URL_LEN);
}

size_t view_read_index(const char *prompt, size_t count)
{
    char buf[16];
    read_line(prompt, buf, sizeof buf);

    char *end;
    long v = strtol(buf, &end, 10);
    if (buf[0] == '\0' || *end != '\0' || v < 1 || (size_t)v > count)
        return (size_t)-1;
    return (size_t)v - 1;
}

int view_confirm(const char *question)
{
    char buf[8];
    char prompt[128];
    snprintf(prompt, sizeof prompt, "%s (д/н): ", question);
    read_line(prompt, buf, sizeof buf);
    return is_yes(buf);
}
