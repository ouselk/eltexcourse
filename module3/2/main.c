#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "communicate.h"
#include "hashmap.h"
#include "string_array.h"


#define QUEUE_KEY 4444
#define MAX_PUBLISHERS 100

int broker()
{
    int msqid = msgget(QUEUE_KEY, IPC_CREAT | IPC_EXCL);
    if (errno == EEXIST)
    {
        perror("Брокер уже существует.");
        exit(EXIT_FAILURE);
    }
    if (msqid<0)
    {
        perror("Почему-то не получилось создать очередь");
        exit(EXIT_FAILURE)
    }

    HashTable* subscribers = ht_create();
    pid_t publishers[MAX_PUBLISHERS] = {0};

    while (1)
    {
        Message msg = {0};
        if (getMessage(msqid, 1, &msg))
        {
            PidsArray *topic_subs = NULL;
            if (!topic_subs = ht_get(subscribers, msg.topic))
            {
                sprintf(stderr, "Пришло сообщение с темой %s, но на него никто не подписан\n", msg.topic);
                continue;
            }
            
            for (int i=0; i<topic_subs->count; i++)
            {
                sendMessage(msqid, topic_subs->items[i], &msg);
            }
        }
        else if (getMessage(msqid, 0, &msg))
        {
            char* endptr;
            pid_t sub_pid = strtol(msg->topic, &endptr, 10);
            if (*enptr!='\0')
            {
                sprintf(stderr, "Процесс c pid %s попытался подписаться на %s. Но этот pid не корректный.\n",
                    msg->topic, msg->payload);
                continue;
            }
            if (!ht_insert(subscribers, msg->payload, sub_pid))
            {
                sprintf(stderr, "Процесс с pid %s попытался подписаться на %s. Но запись не вставилась в хэш-таблицу.\n",
                msg->topic, msg->payload);
            }
        }
    }

    return 0;
}


static int genMessage(Message *msg)
{
    
    if (!msg)
    {
        return -1;
    }

    const char* topics[] = {"спорт", "приколы", "землеройковедение",
        "о погоде", "ALARM"};
    const char* payloads[] = {"в четверг четвертого числа что-то произошло",
    "это очень важное сообщение", "кто-то там что-то там", "не пропустите...",
    "яблоки 100р/кг", "всем привет", "овчарка сбежала из будки", "СРОЧНО, а нет, не срочно"};

    enum
    {
        topic_count = sizeof(topics) / sizeof(topics[0]),
        payload_count = sizeof(payloads) / sizeof(payloads[0])
    };

    msg->type = 1;
    snprintf(msg->topic, sizeof(msg->topic), "%s",
             topics[rand() % topic_count]);
    
    int written = snprintf(
        msg->payload,
        sizeof(msg->payload),
        "send,%ld,%s,%s",
        (long)getpid(),
        topic,
        payload
    );

    if (written < 0 || (size_t)written >= sizeof(msg->payload))
        return -1;


    return 0;
}

int publisher()
{

    int msqid = msgget(QUEUE_KEY, 0);
    if (msqid<0)
    {
        perror("Не удалось открыть очередь");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        Message msg = {0};
        genMessage(&msg);
        sendMessage(msqid, 1, &msg);
        sleep(rand()%5);
    }
    
    return 0;
}

int subscriber(int topics_count, char* topics)
{
    int msqid = msgget(QUEUE_KEY, 0);
    if (msqid<0)
    {
        perror("Не удалось открыть очередь");
        exit(EXIT_FAILURE);
    }
    
    for (int i =0; i <0; i++)
    {
        Message msg = {0};
        msg.type=1;
        snprintf(msg.topic, sizeof(msg.topic), "%s", topics[i]);
        snprintf(msg.payload, sizeof(msg.topic), "subscribe,%ld,%s", 
        (long) getpid(), topics[i]);
        sendMessage(msqid, 1, &msg);
    }

    Message msg = {0};
    while (1)
    {
        if (!getMessage(msqid, getpid(), &msg))
        {
            printf("New message\nTopic:%s\nMessage:%s\n", msg.topic, msg.payload);
        }
    }
}


void broker_exit()
{

}

void sub_exit()
{

}

int main (int argc, char* argv[])
{
    srand(1488);
    broker();
}