/*
 * view.h — ИНТЕРФЕЙС (консольный ввод/вывод).
 * Здесь нет логики хранения: функции только показывают данные и читают ввод.
 */
#ifndef VIEW_H
#define VIEW_H

#include <stddef.h>
#include "contacts.h"

/* Главное меню. Возвращает номер пункта (0 — выход; также 0 при конце ввода). */
int  view_menu(void);

/* Вывод */
void view_message(const char *text);
void view_print_contact(const Contact *c);
void view_print_list(const PhoneBook *pb);

/* Ввод нового контакта (обязательные поля запрашиваются, пока не введены). */
void view_read_contact(Contact *c);

/* Редактирование: Enter — оставить значение, "-" — очистить необязательное поле. */
void view_edit_contact(Contact *c);

/* Запрос номера контакта (1..count). Возвращает индекс (0..count-1)
 * или (size_t)-1, если ввод отменён/некорректен. */
size_t view_read_index(const char *prompt, size_t count);

/* Подтверждение действия: 1 — да, 0 — нет. */
int  view_confirm(const char *question);

#endif /* VIEW_H */
