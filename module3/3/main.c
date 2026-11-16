#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "chat.h"

#define SIG_NEWMSG   SIGUSR1
#define EXIT_WAIT_MS 2000

typedef struct {
    const char *qname;
    chat_member member;
    mqd_t q1, q2;
    mqd_t rq, sq;
    int peer_left;
} chat_ctx;

static chat_ctx ctx;

static void say_fd(int fd, const char *s)
{
    size_t left = strlen(s);
    while (left > 0) {
        ssize_t n = write(fd, s, left);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return;
        }
        s    += n;
        left -= (size_t)n;
    }
}
 
static void say(const char *s)     { say_fd(STDOUT_FILENO, s); }
static void say_err(const char *s) { say_fd(STDERR_FILENO, s); }

static void finish(void)
{
    if (!ctx.peer_left) {
        if (send_exit(ctx.sq) == 0 && ctx.member == FIRST_MEMBER)
            wait_drained(ctx.sq, EXIT_WAIT_MS);
    }
    close_q(ctx.q1, ctx.q2);
    if (ctx.member == FIRST_MEMBER)
        remove_q(ctx.qname);
    say("\nЧат завершён.\n");
    _exit(0);
}

static void on_sigint(int sig)
{
    finish();
}

static void on_newmsg(int sig)
{
    char buf[MAX_MESSAGE_LEN + 1];

    struct sigevent sev;
    memset(&sev, 0, sizeof sev);
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo  = SIG_NEWMSG;
    if (mq_notify(ctx.rq, &sev) == -1) {
        say_err("\nОшибка mq_notify\n");
        finish();
    }

    while (1)
    {
        int rc = recive_message(ctx.rq, buf);
        if (rc == 0) {
            say("\nВам письмо: ");
            say(buf);
            say("\nВведите строку: ");
        } else if (rc == 1) {
            say("\nСобеседник завершил работу.\n");
            ctx.peer_left = 1;
            finish();
        } else {
            if (errno!=EAGAIN)
            say_err("\nОшибка mq_receive\n");
            break;
        }
    }
}

static int set_handler(int sig, void (*handler)(int), const sigset_t *mask)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = handler;
    sa.sa_mask    = *mask;
    sa.sa_flags   = SA_RESTART;
    return sigaction(sig, &sa, NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_очереди>\n", argv[0]);
        return -1;
    }

    int member = open_q(argv[1], &ctx.q1, &ctx.q2);
    if (member < 0) {
        perror("open_q");
        return -1;
    }
    ctx.qname  = argv[1];
    ctx.member = (chat_member)member;
    ctx.rq = (member == FIRST_MEMBER) ? ctx.q1 : ctx.q2;
    ctx.sq = (member == FIRST_MEMBER) ? ctx.q2 : ctx.q1;

    printf("Вы %s участник чата. Ctrl+C или Ctrl+D для выхода.\n",
           member == FIRST_MEMBER ? "первый" : "второй");
    fflush(stdout);
    sigset_t both;
    sigemptyset(&both);
    sigaddset(&both, SIG_NEWMSG);
    sigaddset(&both, SIGINT);

    if (set_handler(SIG_NEWMSG, on_newmsg, &both) == -1 ||
        set_handler(SIGINT, on_sigint, &both) == -1) {
        perror("sigaction");
        close_q(ctx.q1, ctx.q2);
        if (member == FIRST_MEMBER)
            remove_q(ctx.qname);
        return -1;
    }

    on_newmsg(0);
    char line[MAX_MESSAGE_LEN];
    while(1)
    {
        say("Введите строку: ");
        if (!fgets(line, sizeof line, stdin))
            break;
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0')
            continue;
        int rc = send_message(ctx.sq, line);
        if (rc != 0)
            fprintf(stderr, "Не удалось отправить: %s\n", strerror(rc));
    }

    sigprocmask(SIG_BLOCK, &both, NULL);
    finish();
    return 0;
}