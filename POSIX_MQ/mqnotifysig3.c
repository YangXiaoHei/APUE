#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int sig_flag = 0;

void sig_usr1(int signo) {
    sig_flag = 1;
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("usage : %s <name>\n", argv[0]);
        exit(1);
    }

    mqd_t mq = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mq < 0) {
        perror("mq_open error");
        exit(1);
    }
    struct sigevent sigev;
    struct mq_attr attr;
    if (mq_getattr(mq, &attr) < 0) {
        perror("mq_getattr error");
        exit(1);
    }
    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        printf("malloc error\n");
        exit(1);
    }
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if (mq_notify(mq, &sigev) < 0) {
        perror("mq_notify error");
        exit(1);
    }

    sigset_t new_mask, zero_mask;
    sigemptyset(&new_mask);
    sigemptyset(&zero_mask);
    sigaddset(&new_mask, SIGUSR1);

    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    int n;
    while (1) {
        if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) {
            perror("sigprocmask error");
            exit(1);
        }
        while (sig_flag == 0)
            sigsuspend(&zero_mask);
        sig_flag = 0;

        if (mq_notify(mq, &sigev) < 0) {
            perror("mq_notify error");
            exit(1);
        }
        int prio;
        while (n = mq_receive(mq, buffer, attr.mq_msgsize, &prio) >= 0) {
            printf("读到了 %ld bytes 数据，优先级是 %d\n", (long)n, prio);
        }
        if (errno != EAGAIN) {
            perror("mq_receive error");
            exit(1);
        }
        
        if (sigprocmask(SIG_UNBLOCK, &new_mask, NULL) < 0) {
            perror("sigprocmask error");
            exit(1);
        }
    }

    return 0;
}