#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "communicate.h"
#include "hashmap.h"
#include "pids_array.h"

#define QUEUE_KEY 4444
#define BROKER_WORK_TIME 120

typedef struct
{
    int msqid;
    HashTable *subscribers;
    PidsArray publishers;
} BrokerState;

typedef struct
{
    int msqid;
    int topics_count;
    char **topics;
} SubState;

void on_signal(int sig)
{
    exit(EXIT_SUCCESS);
}
int sendCommand(int msqid, const char *action, const char *topic)
{
    Message msg = {0};
    msg.type = 1;
    snprintf(msg.action, sizeof(msg.action), "%s", action);
    msg.pid = getpid();
    snprintf(msg.topic, sizeof(msg.topic), "%s", topic);
    return sendMessage(msqid, 1, &msg);
}

void broker_exit(int status, void *arg)
{
  //  printf("LALALA\n");
  //  fflush(stdout);
    (void)status;
    BrokerState *st = arg;

    signal(SIGINT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    for (size_t i = 0; i < st->publishers.count; i++)
        kill(st->publishers.items[i], SIGINT);

    PidsArray killed = {0};     // чтобы не слать SIGINT дважды одному pid
    for (size_t i = 0; i < HASH_SIZE; i++)
    {
        for (HashNode *node = st->subscribers->buckets[i]; node; node = node->next)
        {
            for (size_t j = 0; j < node->value.count; j++)
            {
                pid_t pid = node->value.items[j];
                if (!pids_array_contains(&killed, pid))
                {
                    kill(pid, SIGINT);
                    pids_array_add(&killed, pid);
                }
            }
        }
    }
    pids_array_destroy(&killed);

    for (int i = 0; i < 20; i++)
    {
        Message tmp;
        while (msgrcv(st->msqid, &tmp, sizeof(Message) - sizeof(long), 0, IPC_NOWAIT) != -1)
            ;
        struct msqid_ds ds;
        if (i > 0 && msgctl(st->msqid, IPC_STAT, &ds) == 0 && ds.msg_qnum == 0)
            break;
        struct timespec ts = {0, 100000000};   // 100 мс
        nanosleep(&ts, NULL);
    }

    msgctl(st->msqid, IPC_RMID, NULL);

    ht_destroy(st->subscribers);
    pids_array_destroy(&st->publishers);
    free(st);
}

int broker(void)
{
    BrokerState *st = calloc(1, sizeof(*st));
    if (!st)
    {
        perror("Не получилось выделить память в брокере");
        exit(EXIT_FAILURE);
    }

    st->subscribers = ht_create();
    if (!st->subscribers)
    {
        perror("ht_create");
        free(st);
        exit(EXIT_FAILURE);
    }

    st->msqid = msgget(QUEUE_KEY, IPC_CREAT | IPC_EXCL | 0666);
    if (st->msqid < 0)
    {
        if (errno == EEXIST)
            fprintf(stderr, "Брокер уже существует.\n");
        else
            perror("Не получилось создать очередь");
        ht_destroy(st->subscribers);
        free(st);
        exit(EXIT_FAILURE);
    }

    on_exit(broker_exit, st);
    signal(SIGALRM, on_signal);
    alarm(BROKER_WORK_TIME);

    while (1)
    {
        Message msg = {0};
        int code = getMessage(st->msqid, 1, &msg);
        if (code == -2)
            break;
        if (code != 1)
            continue;

        if (msg.pid <= 0)
        {
            fprintf(stderr, "Некорректный pid в сообщении\n");
            continue;
        }

        if (strcmp(msg.action, ACT_SEND) == 0)
        {
            if (!pids_array_contains(&st->publishers, msg.pid))
                pids_array_add(&st->publishers, msg.pid);

            PidsArray *topic_subs = ht_get(st->subscribers, msg.topic);
            if (!topic_subs)
                continue;

            for (size_t i = 0; i < topic_subs->count; i++)
                sendMessage(st->msqid, topic_subs->items[i], &msg);
        }
        else if (strcmp(msg.action, ACT_SUBSCRIBE) == 0)
        {
            if (ht_insert(st->subscribers, msg.topic, msg.pid) != 0)
                fprintf(stderr, "Процессу %d: не удалось подписаться на %s\n",
                        msg.pid, msg.topic);
        }
        else if (strcmp(msg.action, ACT_UNSUBSCRIBE) == 0)
        {
            ht_remove_pid(st->subscribers, msg.topic, msg.pid);
        }
        else
        {
            fprintf(stderr, "Неизвестное действие: %s\n", msg.action);
        }
    }

    return 0;
}

int genMessage(Message *msg, const char *topic)
{
    if (!msg || !topic)
    {
        return -1;
    }

    const char* payloads[] = {"в четверг четвертого числа что-то произошло",
    "это очень важное сообщение", "кто-то там что-то там", "не пропустите...",
    "яблоки 100р/кг", "всем привет", "овчарка сбежала из будки", "СРОЧНО, а нет, не срочно"};

    enum
    {
        payload_count = sizeof(payloads) / sizeof(payloads[0])
    };

    msg->type = 1;
    snprintf(msg->action, sizeof(msg->action), "%s", ACT_SEND);
    msg->pid = getpid();
    snprintf(msg->topic, sizeof(msg->topic), "%s", topic);
    snprintf(msg->payload, sizeof(msg->payload), "%s",
             payloads[rand() % payload_count]);

    return 0;
}

int publisher(const char *topic)
{
    int msqid = msgget(QUEUE_KEY, 0);
    if (msqid < 0)
    {
        perror("Не удалось открыть очередь");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        Message msg = {0};
        if (genMessage(&msg, topic) == 0)
        {
            if (sendMessage(msqid, 1, &msg) == -2)
                break;      // очередь удалена
        }
        sleep(1 + rand() % 5);
    }

    return 0;
}

void sub_exit(int status, void *arg)
{
    SubState *st = arg;

    signal(SIGINT, SIG_IGN);
    signal(SIGALRM, SIG_IGN);
    
    for (int i = 0; i < st->topics_count; i++)
    {
        if (sendCommand(st->msqid, ACT_UNSUBSCRIBE, st->topics[i]) == -2)
            break;      // очередь уже удалена
    }

    free(st);
}

int subscriber(int topics_count, char* topics[])
{
    int msqid = msgget(QUEUE_KEY, 0);
    if (msqid < 0)
    {
        perror("Не удалось открыть очередь");
        exit(EXIT_FAILURE);
    }

    SubState *st = calloc(1, sizeof(*st));
    if (!st)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    st->msqid = msqid;
    st->topics_count = topics_count;
    st->topics = topics;

    on_exit(sub_exit, st);

    for (int i = 0; i < topics_count; i++)
    {
        if (sendCommand(msqid, ACT_SUBSCRIBE, topics[i]) == -2)
            return 0;
    }

    while (1)
    {
        Message msg = {0};
        int code = getMessage(msqid, getpid(), &msg);
        if (code == 1)
            printf("New message\nFrom: %d\nTopic: %s\nMessage: %s\n",
                   (int)msg.pid, msg.topic, msg.payload);
        else if (code == -2)
            return 0;
    }
}



void printHelpMessage(const char *prog)
{
    fprintf(stderr,
            "Использование:\n"
            "  %s -b                      брокер\n"
            "  %s -p <тема>               издатель\n"
            "  %s -s <тема> [тема ...]    подписчик\n",
            prog, prog, prog);
}
 
int main(int argc, char* argv[])
{
    if (argc < 2 || argv[1][0] != '-' || argv[1][1] == '\0' || argv[1][2] != '\0')
    {
        printHelpMessage(argv[0]);
        return EXIT_FAILURE;
    }

    signal(SIGINT, on_signal);

    switch (argv[1][1])
    {
    case 'b':
        if (argc != 2)
        {
            printHelpMessage(argv[0]);
            return EXIT_FAILURE;
        }
        return broker();

    case 'p':
        if (argc != 3)
        {
            printHelpMessage(argv[0]);
            return EXIT_FAILURE;
        }
        srand((unsigned)time(NULL) ^ (unsigned)getpid());
        return publisher(argv[2]);

    case 's':
        if (argc < 3)
        {
            printHelpMessage(argv[0]);
            return EXIT_FAILURE;
        }
        return subscriber(argc - 2, &argv[2]);

    default:
        printHelpMessage(argv[0]);
        return EXIT_FAILURE;
    }
}