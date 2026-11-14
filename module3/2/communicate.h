#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#define TOPIC_MAX_LEN 64
#define PAYLOAD_MAX_LEN 1024


typedef struct
{
    long type;
    char topic[TOPIC_MAX_LEN];
    char payload[PAYLOAD_MAX_LEN];
} Message;

int sendMessage(int msqid, long type, const Message *msg);
int getMessage(int msqid, long type, Message *msg);

#endif