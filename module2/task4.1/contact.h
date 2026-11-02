#ifndef CONTACT_H
#define CONTACT_H

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

#endif
