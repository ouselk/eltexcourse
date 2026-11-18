#ifndef CHAT_H
#define CHAT_H

#include <mqueue.h>

#define EXIT_PRIORITY    14
#define NORMAL_PRIORITY  0
#define MAX_MESSAGE_LEN  512   /* максимальный размер сообщения вместе с '\0' */
#define QNAME_BUF   256
#define QUEUE_MODE  0666
#define MAX_MSGS    10

typedef enum { FIRST_MEMBER, SECOND_MEMBER } chat_member;

/*
 * Открывает/создаёт очереди "<qname>_1" и "<qname>_2".
 * В *first и *second кладёт дескрипторы очередей _1 и _2 соответственно.
 *
 * FIRST_MEMBER  (очереди создали мы): _1 открыта на чтение, _2 - на запись.
 * SECOND_MEMBER (очереди уже были):   _1 открыта на запись, _2 - на чтение.
 *
 * Возвращает FIRST_MEMBER / SECOND_MEMBER или отрицательный код ошибки (-errno).
 * Обе очереди открываются с O_NONBLOCK.
 */
int open_q(const char *qname, mqd_t *first, mqd_t *second);

/* Отправка обычного сообщения (приоритет NORMAL_PRIORITY). 0 или -errno. */
int send_message(mqd_t q, const char *msg);

/*
 * Приём сообщения. msg - буфер минимум MAX_MESSAGE_LEN + 1 байт.
 * 0 - обычное сообщение, 1 - сообщение с EXIT_PRIORITY,
 * < 0 - ошибка (-EAGAIN - очередь пуста).
 */
int recive_message(mqd_t q, char *msg);

/* Сообщение "exit" с приоритетом EXIT_PRIORITY. 0 или -errno. */
int send_exit(mqd_t q);

/* Ждёт, пока получатель заберёт всё из очереди q (до timeout_ms). 0 или -ETIMEDOUT. */
int wait_drained(mqd_t q, int timeout_ms);

/* Закрывает обе очереди. */
void close_q(mqd_t first, mqd_t second);

/* Удаляет обе очереди из системы (mq_unlink). */
int remove_q(const char *qname);

#endif