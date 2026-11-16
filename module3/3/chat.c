#include "chat.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#define QNAME_BUF   256
#define QUEUE_MODE  0666
#define MAX_MSGS    10

static void sleep_ms(int ms)
{
    struct timespec ts = { ms / 1000, (long)(ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

static int make_name(char *buf, size_t n, const char *qname, const char *suffix)
{
    const char *slash = (qname[0] == '/') ? "" : "/";
    int len = snprintf(buf, n, "%s%s%s", slash, qname, suffix);
    if (len < 0 || (size_t)len >= n)
        return ENAMETOOLONG;
    return 0;
}

int open_q(const char *qname, mqd_t *first, mqd_t *second)
{
    char n1[QNAME_BUF], n2[QNAME_BUF];
    int rc;

    if (!qname || !*qname || !first || !second)
        return -1;
    if ((rc = make_name(n1, sizeof n1, qname, "_1")) != 0) return -rc;
    if ((rc = make_name(n2, sizeof n2, qname, "_2")) != 0) return -rc;

    struct mq_attr attr;
    memset(&attr, 0, sizeof attr);
    attr.mq_maxmsg  = MAX_MSGS;
    attr.mq_msgsize = MAX_MESSAGE_LEN;

    mqd_t q1 = mq_open(n1, O_CREAT | O_EXCL | O_RDONLY | O_NONBLOCK, QUEUE_MODE, &attr);
    if (q1 != (mqd_t)-1) {
        mqd_t q2 = mq_open(n2, O_CREAT | O_WRONLY | O_NONBLOCK, QUEUE_MODE, &attr);
        if (q2 == (mqd_t)-1) {
            mq_close(q1);
            mq_unlink(n1);
            return -1;
        }
        *first = q1;
        *second = q2;
        return FIRST_MEMBER;
    }
    if (errno != EEXIST)
        return -1;

    q1 = mq_open(n1, O_WRONLY | O_NONBLOCK);
    if (q1 == -1)
        return -1;

    mqd_t q2 = mq_open(n2, O_RDONLY | O_NONBLOCK);
    if (q2 == -1) {
        mq_close(q1);
        return -1;
    }
    *first = q1;
    *second = q2;
    return SECOND_MEMBER;
}

int send_message(mqd_t q, const char *msg)
{
    if (!msg)
        return EINVAL;
    size_t len = strlen(msg) + 1;
    if (len > MAX_MESSAGE_LEN)
        return EMSGSIZE;
    if (mq_send(q, msg, len, NORMAL_PRIORITY) == -1)
        return errno;
    return 0;
}

int recive_message(mqd_t q, char *msg)
{
    unsigned int prio = 0;
    ssize_t n = mq_receive(q, msg, MAX_MESSAGE_LEN, &prio);
    if (n == -1)
        return -errno;
    msg[n] = '\0';
    return (prio == EXIT_PRIORITY) ? 1 : 0;
}

int send_exit(mqd_t q)
{
    static const char text[] = "exit";
    if (mq_send(q, text, sizeof text, EXIT_PRIORITY) == -1)
        return errno;
    return 0;
}

int wait_drained(mqd_t q, int timeout_ms)
{
    struct mq_attr attr;
    for (int waited = 0; waited <= timeout_ms; waited += 50) {
        if (mq_getattr(q, &attr) == -1)
            return errno;
        if (attr.mq_curmsgs == 0)
            return 0;
        sleep_ms(50);
    }
    return ETIMEDOUT;
}

void close_q(mqd_t first, mqd_t second)
{
    mq_close(first);
    mq_close(second);
}

int remove_q(const char *qname)
{
    char n1[QNAME_BUF], n2[QNAME_BUF];
    int rc, result = 0;

    if ((rc = make_name(n1, sizeof n1, qname, "_1")) != 0) return rc;
    if ((rc = make_name(n2, sizeof n2, qname, "_2")) != 0) return rc;

    if (mq_unlink(n1) == -1 && errno != ENOENT) result = errno;
    if (mq_unlink(n2) == -1 && errno != ENOENT) result = errno;
    return result;
}