#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

int broker()
{
    int msqid = msgget(4444, IPC_CREAT | IPC_EXCL);
    if (errno == EEXIST)
    {
        perror("ОШибка рыбка брат");
        exit(EXIT_FAILURE);
    }
    while (1)
    {}
}

int main (int argc, char* argv[])
{
    broker();
}