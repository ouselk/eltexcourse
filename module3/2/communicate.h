#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <sys/types.h>

#define ACTION_MAX_LEN 16
#define TOPIC_MAX_LEN 64
#define PAYLOAD_MAX_LEN 256

#define ACT_SEND "send"
#define ACT_SUBSCRIBE "subscribe"
#define ACT_UNSUBSCRIBE "unsubscribe"

typedef struct
{
    long type;
    char action[ACTION_MAX_LEN];
    pid_t pid;
    char topic[TOPIC_MAX_LEN];
    char payload[PAYLOAD_MAX_LEN];
} Message;

int sendMessage(int msqid, long type, const Message *msg);
int getMessage(int msqid, long type, Message *msg);

#endif