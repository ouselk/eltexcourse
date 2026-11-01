#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define CONTACT_BOOK_SIZE 100

/* Размеры полей указаны в байтах (UTF-8: кириллическая буква = 2 байта),
   включая завершающий '\0'. */
#define NAME_LEN      128
#define WORK_LEN      128
#define POSITION_LEN  128
#define PHONE_LEN     128
#define EMAIL_LEN     256

typedef struct {
    char family[NAME_LEN];          /* фамилия (обязательно) */
    char name[NAME_LEN];            /* имя (обязательно) */
    char surname[NAME_LEN];         /* отчество */
    char workplace[WORK_LEN];       /* место работы */
    char position[POSITION_LEN];    /* должность */
    char phone_number[PHONE_LEN];   /* телефон(ы) */
    char emails[EMAIL_LEN];         /* e-mail(ы) */
} Contact;

typedef struct {
    Contact contacts[CONTACT_BOOK_SIZE];
    int size;                       /* число ячеек массива, = CONTACT_BOOK_SIZE */
} ContactBook;

/* Ячейка считается пустой, если в ней не заполнена фамилия.
   Возвращает 1, если пустая, иначе 0. */
int isContactEmpty(const Contact *contact);

/* Добавляет контакт в ячейку idx. Ячейка должна быть пустой,
   у контакта должны быть заполнены фамилия и имя.
   0 - успех, -1 - ошибка. */
int addContact(ContactBook *cb, const Contact *contact, int idx);

/* Удаляет контакт из ячейки idx. 0 - успех, -1 - ошибка. */
int delContact(ContactBook *cb, int idx);

/* Заменяет контакт в ячейке idx на new_contact (delContact + addContact).
   При ошибке старый контакт остаётся на месте. 0 - успех, -1 - ошибка. */
int updateContact(ContactBook *cb, int idx, const Contact *new_contact);

#endif /* PHONEBOOK_H */