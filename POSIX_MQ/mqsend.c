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
    void *ptr;

    size_t len;
    unsigned int prio;

    if (argc != 4) {
        printf("usage : %s <name> <#bytes> <priority>\n", argv[0]);
        exit(1);
    }
    len = atoi(argv[2]);
    prio = atoi(argv[3]);

    if ((mqd = mq_open(argv[1], O_WRONLY)) < 0) {
        perror("mq_open error");
        exit(1);
    }

    ptr = calloc(len, 1);
    if (mq_send(mqd, ptr, len, prio) < 0) {
        perror("mq_send error");
        exit(1);
    }
    
    return 0;    
}