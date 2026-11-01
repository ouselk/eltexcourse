#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define CONTACT_BOOK_SIZE 100

#define NAME_LEN      128
#define WORK_LEN      128
#define POSITION_LEN  128
#define PHONE_LEN     128
#define EMAIL_LEN     256

typedef struct {
    char family[NAME_LEN];
    char name[NAME_LEN];
    char surname[NAME_LEN];
    char workplace[WORK_LEN];
    char position[POSITION_LEN];
    char phone_number[PHONE_LEN];
    char emails[EMAIL_LEN];
} Contact;

typedef struct {
    Contact contacts[CONTACT_BOOK_SIZE];
    int size;
    int count;
} ContactBook;

void initContactBook(ContactBook *cb);

int getContactCount(const ContactBook *cb);

int isContactBookFull(const ContactBook *cb);

int getContact(const ContactBook *cb, int idx, Contact *out);

int findContact(const ContactBook *cb, const char *family, const char *name);

int addContact(ContactBook *cb, const Contact *contact, int idx);

int delContact(ContactBook *cb, int idx);

int updateContact(ContactBook *cb, int idx, const Contact *new_contact);

#endif