#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "phonebook.h"

static double nowMs(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

static Contact *loadPairs(const char *path, int *count)
{
    FILE *f = fopen(path, "r");
    if (!f) { perror(path); exit(1); }

    int cap = 1024, n = 0;
    Contact *arr = malloc(cap * sizeof(*arr));
    char line[512];

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        char *sep = strchr(line, ';');
        if (!sep) continue;
        *sep = '\0';
        if (n == cap) { cap *= 2; arr = realloc(arr, cap * sizeof(*arr)); }
        memset(&arr[n], 0, sizeof(arr[n]));
        snprintf(arr[n].family, sizeof(arr[n].family), "%s", line);
        snprintf(arr[n].name, sizeof(arr[n].name), "%s", sep + 1);
        n++;
    }
    fclose(f);
    *count = n;
    return arr;
}

/* Проверка структуры: порядок, связи prev/next, размер. Возвращает хэш содержимого или 0 при ошибке. */
static unsigned long long checkBook(const ContactBook *cb)
{
    unsigned long long h = 1469598103934665603ULL;
    int cnt = 0;
    const ListNode *prev = NULL;

    for (const ListNode *n = cb->list.head; n; prev = n, n = n->next) {
        if (n->prev != prev) return 0;
        if (prev && listCompare(&prev->contact, &n->contact) >= 0) return 0;
        for (const char *p = n->contact.family; *p; p++) h = (h ^ (unsigned char)*p) * 1099511628211ULL;
        h = (h ^ ';') * 1099511628211ULL;
        for (const char *p = n->contact.name; *p; p++) h = (h ^ (unsigned char)*p) * 1099511628211ULL;
        h = (h ^ '\n') * 1099511628211ULL;
        cnt++;
    }
    if (cb->list.tail != prev || cnt != cb->size) return 0;
    return h ? h : 1;
}

static const char *modeName(ListSearchMode m)
{
    switch (m) {
    case LIST_SEARCH_BASELINE: return "исходный (без оптимизации)";
    case LIST_SEARCH_HEAD:     return "только с головы + ранний выход";
    default:                   return "по весам букв (оптимизация)";
    }
}

typedef struct {
    double ins_ms, del_ms;
    double ins_steps, del_steps;  /* среднее число пройденных узлов на операцию */
    unsigned long long hash_after_insert;
} Result;

static int cmpDouble(const void *a, const void *b)
{
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

static int runOnce(ContactBook *cb, const Contact *items, int n, ListSearchMode mode,
                   unsigned long long base_hash, Result *r)
{
#ifdef LIST_COUNT_STEPS
    g_list_steps = 0;
#endif
    double t0 = nowMs();
    for (int i = 0; i < n; i++)
        if (addContactEx(cb, &items[i], mode) != 0) { printf("ошибка вставки #%d\n", i); return -1; }
    double t1 = nowMs();
#ifdef LIST_COUNT_STEPS
    r->ins_steps = (double)g_list_steps / n;
    g_list_steps = 0;
#else
    r->ins_steps = 0;
#endif

    r->hash_after_insert = checkBook(cb);
    if (!r->hash_after_insert) { printf("список повреждён после вставки\n"); return -1; }

    double t2 = nowMs();
    for (int i = 0; i < n; i++)
        if (delContactEx(cb, items[i].family, items[i].name, mode) != 0) { printf("ошибка удаления #%d\n", i); return -1; }
    double t3 = nowMs();
#ifdef LIST_COUNT_STEPS
    r->del_steps = (double)g_list_steps / n;
#else
    r->del_steps = 0;
#endif

    if (checkBook(cb) != base_hash) { printf("список после удаления не совпал с исходным\n"); return -1; }

    r->ins_ms = t1 - t0;
    r->del_ms = t3 - t2;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage: %s base.csv test_real.csv test_uniform.csv [reps]\n", argv[0]);
        return 1;
    }
    int reps = (argc > 4) ? atoi(argv[4]) : 3;

    int nbase;
    Contact *base = loadPairs(argv[1], &nbase);

    static ContactBook book;
    initContactBook(&book);

    double t0 = nowMs();
    for (int i = 0; i < nbase; i++)
        if (appendContactSorted(&book, &base[i]) != 0) { printf("загрузка: ошибка на записи %d\n", i); return 1; }
    printf("Загружено %d контактов за %.1f мс (размер Contact = %zu байт, узла = %zu байт)\n",
           book.size, nowMs() - t0, sizeof(Contact), sizeof(ListNode));

    unsigned long long base_hash = checkBook(&book);
    if (!base_hash) { printf("исходный список не отсортирован\n"); return 1; }

    /* быстрые проверки граничных случаев для всех режимов */
    ListSearchMode modes[3] = { LIST_SEARCH_BASELINE, LIST_SEARCH_HEAD, LIST_SEARCH_WEIGHTED };
    for (int m = 0; m < 3; m++) {
        Contact dup = base[nbase / 2];
        Contact first, last;
        memset(&first, 0, sizeof(first)); memset(&last, 0, sizeof(last));
        snprintf(first.family, sizeof(first.family), "Аа"); snprintf(first.name, sizeof(first.name), "Ааа");
        snprintf(last.family, sizeof(last.family), "Яяяя"); snprintf(last.name, sizeof(last.name), "Яяя");
        int ok = 1;
        ok &= addContactEx(&book, &dup, modes[m]) != 0;                    /* дубликат отклонён */
        ok &= delContactEx(&book, "Несуществующий", "Никто", modes[m]) != 0;  /* нет такого */
        ok &= addContactEx(&book, &first, modes[m]) == 0;
        ok &= addContactEx(&book, &last, modes[m]) == 0;
        ok &= delContactEx(&book, first.family, first.name, modes[m]) == 0;
        ok &= delContactEx(&book, last.family, last.name, modes[m]) == 0;
        ok &= checkBook(&book) == base_hash;
        printf("Проверка граничных случаев [%s]: %s\n", modeName(modes[m]), ok ? "OK" : "ОШИБКА");
        if (!ok) return 1;
    }

    const char *wl_names[2] = { "реальное распределение первых букв", "равномерно по буквам алфавита" };

    for (int w = 0; w < 2; w++) {
        int n;
        Contact *items = loadPairs(argv[2 + w], &n);
        printf("\n=== Набор %d: %s, %d записей (вставка, затем удаление тех же записей) ===\n", w + 1, wl_names[w], n);
        printf("%-34s %12s %12s %14s %14s\n", "режим", "вставка, мс", "удаление, мс", "шагов/вставка", "шагов/удал.");

        unsigned long long ref_hash = 0;
        double med_ins[3], med_del[3];

        for (int m = 0; m < 3; m++) {
            double ti[16], td[16];
            Result r = {0};
            for (int k = 0; k < reps; k++) {
                if (runOnce(&book, items, n, modes[m], base_hash, &r) != 0) return 1;
                ti[k] = r.ins_ms; td[k] = r.del_ms;
            }
            if (m == 0) ref_hash = r.hash_after_insert;
            else if (r.hash_after_insert != ref_hash) { printf("результат вставки отличается от исходного режима!\n"); return 1; }

            qsort(ti, reps, sizeof(double), cmpDouble);
            qsort(td, reps, sizeof(double), cmpDouble);
            med_ins[m] = ti[reps / 2];
            med_del[m] = td[reps / 2];
            printf("%-34s %12.1f %12.1f %14.0f %14.0f\n", modeName(modes[m]),
                   med_ins[m], med_del[m], r.ins_steps, r.del_steps);
        }
        printf("Ускорение оптимизации относительно исходного: вставка x%.2f, удаление x%.2f\n",
               med_ins[0] / med_ins[2], med_del[0] / med_del[2]);
        printf("Ускорение относительно варианта «только с головы»: вставка x%.2f, удаление x%.2f\n",
               med_ins[1] / med_ins[2], med_del[1] / med_del[2]);
        free(items);
    }

    freeContactBook(&book);
    free(base);
    return 0;
}
