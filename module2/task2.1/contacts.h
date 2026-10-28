/*
 * contacts.h — МОДЕЛЬ телефонной книги.
 * Только данные и логика. Никакого ввода/вывода (printf/scanf) здесь нет.
 */
#ifndef CONTACTS_H
#define CONTACTS_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_CONTACTS    100   /* максимум контактов в книге */

#define NAME_LEN        64    /* фамилия, имя, отчество (в байтах, UTF-8) */
#define TEXT_LEN        128   /* место работы, должность */
#define PHONE_LEN       24
#define EMAIL_LEN       64
#define URL_LEN         128   /* ссылки на соцсети и профили мессенджеров */

#define MAX_PHONES      3
#define MAX_EMAILS      3
#define MAX_SOCIALS     3
#define MAX_MESSENGERS  3

/* Один контакт. Пустая строка ("") означает «поле не заполнено». */
typedef struct {
    char last_name[NAME_LEN];                    /* обязательное */
    char first_name[NAME_LEN];                   /* обязательное */
    char middle_name[NAME_LEN];
    char workplace[TEXT_LEN];
    char position[TEXT_LEN];
    char phones[MAX_PHONES][PHONE_LEN];
    char emails[MAX_EMAILS][EMAIL_LEN];
    char socials[MAX_SOCIALS][URL_LEN];          /* страницы в соцсетях */
    char messengers[MAX_MESSENGERS][URL_LEN];    /* профили в мессенджерах */
} Contact;

/* Телефонная книга: массив структур + счётчик занятых элементов. */
typedef struct {
    Contact items[MAX_CONTACTS];
    size_t  count;
} PhoneBook;

typedef enum {
    PB_OK = 0,
    PB_ERR_FULL,      /* книга заполнена */
    PB_ERR_INDEX,     /* неверный номер контакта */
    PB_ERR_NO_NAME    /* не заполнены фамилия и/или имя */
} PbStatus;

/* Вспомогательные функции */
bool        str_is_blank(const char *s);           /* пусто или только пробелы */
void        contact_init(Contact *c);              /* обнулить все поля */
PbStatus    contact_validate(const Contact *c);    /* проверка обязательных полей */

/* Операции над книгой */
void        pb_init(PhoneBook *pb);
size_t      pb_count(const PhoneBook *pb);
const Contact *pb_get(const PhoneBook *pb, size_t index);
PbStatus    pb_add(PhoneBook *pb, const Contact *c);
PbStatus    pb_update(PhoneBook *pb, size_t index, const Contact *c);
PbStatus    pb_remove(PhoneBook *pb, size_t index);

const char *pb_status_message(PbStatus st);

#endif /* CONTACTS_H */
