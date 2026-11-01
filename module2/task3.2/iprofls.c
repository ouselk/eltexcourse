#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "iprofls.h"

IpAddress genIpAddress(void)
{
    IpAddress address = 0;
    for (int i = 0; i < 4; i++) {
        unsigned char byte = (unsigned char)(rand() %256);
        address |= (byte & 0xFF) << (8 * i);
    }
    return address;
}

bool addressInSubnet(IpAddress address, IpAddress gateway, IpAddress mask)
{
    return (address & mask) == (gateway & mask);
}

int parseIp(const char *s, IpAddress *out)
{
    IpAddress result = 0;
    for (int i = 0; i < 4; i++) {
        if (!isdigit((unsigned char)*s))
            return -1;
        char *end;
        unsigned long v = strtoul(s, &end, 10);
        if (v > 255)
            return -1;
        result = (result << 8) | v;
        s = end;
        if (i < 3) {
            if (*s != '.')
                return -1;
            s++;
        }
    }
    if (*s != '\0')
        return -1;
    *out = result;
    return 0;
}

bool isCorrectMask(IpAddress mask)
{
    IpAddress inv = ~mask;           /* у корректной маски это 0...01...1 */
    return (inv & (inv + 1)) == 0;   /* вида 2^k - 1 */
}

IpAddress prefixToMask(int prefix)
{
    if (prefix <= 0)
        return 0;                    /* сдвиг на 32 — UB, обрабатываем отдельно */
    return 0xFFFFFFFFu << (32 - prefix);
}

int countOnes(IpAddress mask)
{
    int n = 0;
    while (mask) {
        mask &= mask - 1;
        n++;
    }
    return n;
}

void ipToString(IpAddress addr, char *buf)
{
    snprintf(buf, 16, "%u.%u.%u.%u",
             ((addr >> 24) & 0xFF), ((addr >> 16) & 0xFF),
             ((addr >> 8) & 0xFF),  (addr & 0xFF));
}