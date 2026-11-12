#ifndef CH_COMMINICATE_H
#define CH_COMMINICATE_H

#define PACKAGE_SIZE 256

typedef enum
{
    UNKNOWN = -1,
    SLAVE_READY_RECIEVE,
    MASTER_SEND_NAME_AND_SIZE,
    MASTER_NOTIFICATE_ABOUT_END,
    DATA_RECIVE
} PackageType;

typedef struct
{
    PackageType type;
    char data[PACKAGE_SIZE];
    int lenght;
} BorkaProtocolPackage;

int send_package(const BorkaProtocolPackage* pkg, int fdx);
//int send_message(char msg[], int fdx);
int send_ready_rcv(int fdx);
int send_name_and_size(char name[], long long size, int fdx);
int send_end(int fdx);

int read_package(int fdx, BorkaProtocolPackage* result);
int parse_name_and_size(const BorkaProtocolPackage* data, char name[], long long* size);

#endif