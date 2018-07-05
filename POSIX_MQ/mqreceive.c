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

int main(int argc, char *argv[]) {
    
    int c, flags = 0;

    mqd_t  mqd;
    ssize_t n;
    unsigned int prio;

    char *buffer;

    flags |= O_RDONLY;

    while ((c = getopt(argc, argv, "n")) != -1) {
        switch (c) {
            case 'n' :
                flags |= O_NONBLOCK;
                break;
        }
    }
    if (optind != argc - 1) {
        printf("usage : %s [-n] <name>\n", argv[0]);
        exit(1);
    }

    if ((mqd = mq_open(argv[optind], flags)) < 0) {
        perror("mq_open error");
        exit(1);
    }
    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) < 0) {
        perror("mq_getattr error");
        exit(1);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        printf("malloc error");
        exit(1);
    }

    n = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    printf("read %ld bytes, priority = %u\n", (long)n, prio);

    return 0;    
}