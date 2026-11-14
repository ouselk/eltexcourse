
#include <errno.h>
#include "communicate.h"

int sendMessage(int msqid, long type, const Message *msg)
{
    Message msg = {0};

    msg.type = 1;
    strcpy(msg.topic, "news");
    strcpy(msg.payload, "Hello!");

    if (msgsnd(msqid, &msg, sizeof(Message) - sizeof(long), IPC_NOWAIT) == -1)
    {
        if (errno==EINVAL)
            return -2;
        return -1;
    }
    return 0;
}

int getMessage(int msqid, long type, Message *msg)
{
    ssize_t result = msgrcv(
        msqid,
        msg,
        sizeof(Message) - sizeof(long),
        type,
        IPC_NOWAIT
    );

    if (result == -1)
    {
        if (errno == ENOMSG)
            return 0;
        if (errno==EINVAL)
            return -2;
        perror("msgrcv");
        return -1;
    }

    return 1;
}