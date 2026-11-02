#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#define CONTACT_BOOK_SIZE 100

#define NAME_LEN   64
#define PHONE_LEN  64
#define EMAIL_LEN  128
#define WORK_LEN   128

typedef struct {
    char family[NAME_LEN];
    char name[NAME_LEN];
    char surname[NAME_LEN];
    char workplace[WORK_LEN];
    char position[WORK_LEN];
    char phone_number[PHONE_LEN];
    char emails[EMAIL_LEN];
} Contact;

typedef struct {
    Contact contacts[CONTACT_BOOK_SIZE];
    int size;
} ContactBook;


void initContactBook(ContactBook *cb);
int findContact(const ContactBook *cb, const char *family, const char *name);
int addContact(ContactBook *cb, const Contact *contact);
int delContact(ContactBook *cb, const char *family, const char *name);
int updateContact(ContactBook *cb, const char *family, const char *name,
                  const Contact *new_contact);

void printContacts(const ContactBook *cb);

#endif
