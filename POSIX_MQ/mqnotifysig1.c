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

void *buffer;
mqd_t mq;
struct mq_attr attr;
struct sigevent sigev;

void sig_usr1(int signo) {
    ssize_t n;
    if (mq_notify(mq,  &sigev) < 0) {
        perror("mq_notify error");
        exit(1);
    }
    printf("mq_msgsize = %ld\n", (long)attr.mq_msgsize);
    if (n = mq_receive(mq, buffer, attr.mq_msgsize, NULL) < 0) {
        perror("mq_receive error");
        exit(1);
    }
    printf("读到 %ld bytes 字节\n", (long)n);
    return;
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("usage : %s <name>\n", argv[0]);
        exit(1);
    }

    mq = mq_open(argv[1], O_RDONLY);
    if (mq < 0) {
        perror("mq_open error");
        exit(1);
    }
    if (mq_getattr(mq, &attr) < 0) {
        perror("mq_getattr error");
        exit(1);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc error");
        exit(1);
    }

    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if (mq_notify(mq, &sigev) < 0) {
        perror("mq_notify error");
        exit(1);
    }

    while(1)
        pause();

    return 0;
}