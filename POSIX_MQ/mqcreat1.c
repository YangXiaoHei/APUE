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
#ifdef __gnu_linux__
#include <mqueue.h>
#endif


int main(int argc, char *argv[]) {
    
    int c, flags;
    mqd_t mqd;

    flags = O_RDWR | O_CREAT;

    while ((c = getopt(argc, argv, "e")) != -1) {
        switch(c) {
            case 'e' : {
                flags |= O_EXCL;
            } break;
        }
    }
    if (optind != argc - 1) {
        printf("usage : %s [-e] <name>\n", argv[0]);
        exit(1);
    }

    mqd = mq_open(argv[optind], flags, 0644, NULL);
    if (mqd < 0) {
        perror("mq_open error");
        exit(1);
    }

    mq_close(mqd);
    
    return 0;    
}