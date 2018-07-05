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
    
    mqd_t mqd;

    struct mq_attr attr;

    if (argc != 2) {
        printf("usage : %s <name>\n", argv[0]);
        exit(1);
    }

    mqd = mq_open(argv[1], O_RDONLY);
    if (mqd < 0) {
        perror("mq_open error");
        exit(1);
    }

    if (mq_getattr(mqd, &attr) < 0) {
        perror("mq_getattr");
        exit(1);
    }

    printf("max #msgs = %ld\n"
           "max #bytes/msg = %ld\n"
           "currently on queue = %ld\n",
           attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    if (mq_close(mqd) < 0) {
        perror("mq_close");
        exit(1);
    }
    
    return 0;    
}