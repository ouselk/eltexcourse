/*
 * contacts.c — реализация МОДЕЛИ. Ввода/вывода нет.
 */
#include "contacts.h"

#include <ctype.h>
#include <string.h>

bool str_is_blank(const char *s)
{
    for (; *s; ++s) {
        if (!isspace((unsigned char)*s))
            return false;
    }
    return true;
}

void contact_init(Contact *c)
{
    memset(c, 0, sizeof *c);
}

PbStatus contact_validate(const Contact *c)
{
    if (str_is_blank(c->last_name) || str_is_blank(c->first_name))
        return PB_ERR_NO_NAME;
    return PB_OK;
}

void pb_init(PhoneBook *pb)
{
    memset(pb, 0, sizeof *pb);
}

size_t pb_count(const PhoneBook *pb)
{
    return pb->count;
}

const Contact *pb_get(const PhoneBook *pb, size_t index)
{
    if (index >= pb->count)
        return NULL;
    return &pb->items[index];
}

PbStatus pb_add(PhoneBook *pb, const Contact *c)
{
    PbStatus st = contact_validate(c);
    if (st != PB_OK)
        return st;
    if (pb->count >= MAX_CONTACTS)
        return PB_ERR_FULL;

    pb->items[pb->count++] = *c;
    return PB_OK;
}

PbStatus pb_update(PhoneBook *pb, size_t index, const Contact *c)
{
    if (index >= pb->count)
        return PB_ERR_INDEX;

    PbStatus st = contact_validate(c);
    if (st != PB_OK)
        return st;

    pb->items[index] = *c;
    return PB_OK;
}

PbStatus pb_remove(PhoneBook *pb, size_t index)
{
    if (index >= pb->count)
        return PB_ERR_INDEX;

    /* сдвигаем хвост массива влево на одну позицию */
    memmove(&pb->items[index], &pb->items[index + 1],
            (pb->count - index - 1) * sizeof pb->items[0]);
    pb->count--;
    memset(&pb->items[pb->count], 0, sizeof pb->items[0]);
    return PB_OK;
}

const char *pb_status_message(PbStatus st)
{
    switch (st) {
    case PB_OK:          return "Готово.";
    case PB_ERR_FULL:    return "Ошибка: телефонная книга заполнена.";
    case PB_ERR_INDEX:   return "Ошибка: контакта с таким номером нет.";
    case PB_ERR_NO_NAME: return "Ошибка: фамилия и имя обязательны.";
    }
    return "Неизвестная ошибка.";
}
