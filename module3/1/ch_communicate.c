#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch_communicate.h"
#include "files.h"

int send_package(const BorkaProtocolPackage* pkg, int fdx)
{
    return write_all(fdx, pkg, sizeof(*pkg));
}

static int send_simple(PackageType type, int fdx)
{
    BorkaProtocolPackage pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.type = type;
    pkg.lenght = 0;
    return send_package(&pkg, fdx);
}

int send_ready_rcv(int fdx)
{
    return send_simple(SLAVE_READY_RECIEVE, fdx);
}

int send_end(int fdx)
{
    return send_simple(MASTER_NOTIFICATE_ABOUT_END, fdx);
}

int send_name_and_size(char name[], long long size, int fdx)
{
    BorkaProtocolPackage pkg;
    memset(&pkg, 0, sizeof(pkg));
    pkg.type = MASTER_SEND_NAME_AND_SIZE;

    int n = snprintf(pkg.data, PACKAGE_SIZE, "%lld %s", size, name);
    if (n < 0 || n >= PACKAGE_SIZE)
        return -1;

    pkg.lenght = n;
    return send_package(&pkg, fdx);
}

int read_package(int fdx, BorkaProtocolPackage* result)
{
    int n = read_all(fdx, result, sizeof(*result));
    if (n != (int)sizeof(*result))
        return -1;

    if (result->lenght < 0 || result->lenght > PACKAGE_SIZE)
        return -1;
    return 0;
}

int parse_name_and_size(const BorkaProtocolPackage* data, char name[], long long* size)
{
    char tmp[PACKAGE_SIZE + 1];
    memcpy(tmp, data->data, PACKAGE_SIZE);
    tmp[PACKAGE_SIZE] = '\0'; 

    char* end = NULL;
    long long value = strtoll(tmp, &end, 10);
    if (end == tmp || *end != ' ')
        return -1;

    *size = value;
    strcpy(name, end + 1);
    return 0;
}