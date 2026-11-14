#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "communicate.h"

int sendMessage(int msqid, long type, const Message *msg)
{
    Message out = *msg;
    out.type = type;

    if (msgsnd(msqid, &out, sizeof(Message) - sizeof(long), IPC_NOWAIT) == -1)
    {
        if (errno == EINVAL || errno == EIDRM)
            return -2;      // очередь недоступна
        return -1;          // например, очередь переполнена
    }
    return 0;
}

// блокирующее чтение: прерывается сигналом (EINTR -> 0)
int getMessage(int msqid, long type, Message *msg)
{
    ssize_t result = msgrcv(msqid, msg, sizeof(Message) - sizeof(long), type, 0);

    if (result == -1)
    {
        if (errno == EINTR)
            return 0;
        if (errno == EINVAL || errno == EIDRM)
            return -2;
        perror("msgrcv");
        return -1;
    }
    return 1;
}