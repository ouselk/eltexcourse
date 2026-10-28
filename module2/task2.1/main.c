/*
 * main.c — связывает модель (contacts.*) и интерфейс (view.*).
 *
 * Сборка:  gcc -Wall -Wextra -std=c11 -o phonebook main.c contacts.c view.c
 */
#include <stdio.h>

#include "contacts.h"
#include "view.h"

#define NO_INDEX ((size_t)-1)

static void action_add(PhoneBook *pb)
{
    Contact c;
    view_read_contact(&c);
    view_message(pb_status_message(pb_add(pb, &c)));
}

static void action_edit(PhoneBook *pb)
{
    if (pb_count(pb) == 0) {
        view_message("Список контактов пуст.");
        return;
    }
    view_print_list(pb);

    size_t idx = view_read_index("\nНомер контакта для редактирования: ",
                                 pb_count(pb));
    if (idx == NO_INDEX) {
        view_message("Неверный номер.");
        return;
    }

    Contact copy = *pb_get(pb, idx);     /* правим копию, а не оригинал */
    view_edit_contact(&copy);
    view_message(pb_status_message(pb_update(pb, idx, &copy)));
}

static void action_remove(PhoneBook *pb)
{
    if (pb_count(pb) == 0) {
        view_message("Список контактов пуст.");
        return;
    }
    view_print_list(pb);

    size_t idx = view_read_index("\nНомер контакта для удаления: ",
                                 pb_count(pb));
    if (idx == NO_INDEX) {
        view_message("Неверный номер.");
        return;
    }
    if (!view_confirm("Удалить контакт?")) {
        view_message("Отменено.");
        return;
    }
    view_message(pb_status_message(pb_remove(pb, idx)));
}

int main(void)
{
    PhoneBook pb;
    pb_init(&pb);

    for (;;) {
        switch (view_menu()) {
        case 1: view_print_list(&pb); break;
        case 2: action_add(&pb);      break;
        case 3: action_edit(&pb);     break;
        case 4: action_remove(&pb);   break;
        case 0: return 0;
        default: view_message("Неизвестный пункт меню.");
        }
    }
}
