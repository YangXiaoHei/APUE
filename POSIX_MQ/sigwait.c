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

int main(int argc, char const *argv[])
{
    mqd_t mq = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mq < 0) {
        perror("mq_open error");
        exit(1);
    }

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

    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if (mq_notify(mq, &sigev) < 0) {
        perror("mq_notify error");
        exit(1);
    }

    int signo = 0;

    while (1) {
        if (sigwait(&new_mask, &signo) < 0) {
            perror("sigwait error");
            exit(1);
        }
        if (signo == SIGUSR1) {
            int n;

            if (mq_notify(mq, &sigev) < 0) {
                perror("mq_notify error");
                exit(1);
            }

            while ((n = mq_receive(mq, buffer, attr.mq_msgsize, NULL)) >= 0) {
                printf("read %d bytes\n", n);
            }
            if (errno != EAGAIN) {
                perror("mq_receive error");
                exit(1);
            }
        }
    }






    return 0;
}