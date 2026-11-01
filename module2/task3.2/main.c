#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "iprofls.h"

#define MAX_N 10000000

static void printUsage(const char *prog)
{
    fprintf(stderr,
            "Использование:\n"
            "  %s 192.168.0.1/24 N\n"
            "  %s 192.168.0.1 255.255.255.0 N\n"
            "где N - количество пакетов (1..%d)\n",
            prog, prog, MAX_N);
}

static int parseCidr(const char *arg, IpAddress *gateway, IpAddress *mask)
{
    char buf[64];
    const char *slash = strchr(arg, '/');
    if (!slash || (slash - arg) >= sizeof buf)
        return -1;
    memcpy(buf, arg, slash - arg);
    buf[slash - arg] = '\0';
    if (parseIp(buf, gateway)!=0)
        return -1;

    const char *p = slash + 1;
    if (!isdigit((unsigned char)*p))
        return -1;
    char *end;
    unsigned long prefix = strtoul(p, &end, 10);
    if (*end != '\0' || prefix > 32)
        return -1;
    *mask = prefixToMask((int)prefix);
    return 0;
}

/* Разбор N. Возвращает 0 - ок, 1 - не число / 0, 2 - больше MAX_N */
static int parseCount(const char *s, unsigned long *n)
{
    if (!isdigit((unsigned char)*s))
        return 1;
    char *end;
    unsigned long v = strtoull(s, &end, 10);
    if (*end != '\0' || v == 0)
        return 1;
    if (v > MAX_N)
        return 2;
    *n = (unsigned long)v;
    return 0;
}

int main(int argc, char *argv[])
{
    IpAddress gateway, mask;
    const char *nArg;

    if (argc == 3) {
        if (parseCidr(argv[1], &gateway, &mask)!=0) {
            fprintf(stderr, "Ошибка: некорректный шлюз или префикс '%s'\n", argv[1]);
            printUsage(argv[0]);
            return 1;
        }
        nArg = argv[2];
    } else if (argc == 4) {
        if (parseIp(argv[1], &gateway)!=0) {
            fprintf(stderr, "Ошибка: некорректный IP-адрес шлюза '%s'\n", argv[1]);
            printUsage(argv[0]);
            return 1;
        }
        if (parseIp(argv[2], &mask)!=0 || !isCorrectMask(mask)) {
            fprintf(stderr, "Ошибка: некорректная маска подсети '%s'\n", argv[2]);
            printUsage(argv[0]);
            return 1;
        }
        nArg = argv[3];
    } else {
        fprintf(stderr, "Ошибка: неверное число аргументов\n");
        printUsage(argv[0]);
        return 1;
    }

    unsigned long n = 0;
    int rc = parseCount(nArg, &n);
    if (rc == 1) {
        fprintf(stderr, "Ошибка: N должно быть положительным целым числом\n");
        printUsage(argv[0]);
        return 1;
    }
    if (rc == 2) {
        fprintf(stderr, "Ошибка: N превышает максимально допустимое значение %d\n", MAX_N);
        return 1;
    }

    srand(time(NULL));

    unsigned long inside = 0;
    for (unsigned long i = 0; i < n; i++) {
        IpAddress dst = genIpAddress();
        if (addressInSubnet(dst, gateway, mask))
            inside++;
    }
    unsigned long outside = n - inside;

    int k = countOnes(mask);
    double theory = ldexp(1.0, -k);

    char gwStr[16], maskStr[16];
    ipToString(gateway, gwStr);
    ipToString(mask, maskStr);

    printf("Шлюз: %s, маска: %s (/%d)\n", gwStr, maskStr, k);
    printf("Обработано пакетов: %lu\n", n);
    printf("Своя подсеть:  %lu шт. (%.6f%%)\n", inside, 100.0 * (double)inside / (double)n);
    printf("Другие сети:   %lu шт. (%.6f%%)\n", outside, 100.0 * (double)outside / (double)n);
    printf("Теоретическая доля попадания в свою подсеть: %.10f%%\n", 100.0 * theory);

    return 0;
}