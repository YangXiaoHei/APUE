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

struct mq_attr attr;

int main(int argc, char *argv[]) {
    
    int flags, c;

    flags = O_CREAT | O_RDWR;
    while ( (c = getopt(argc, argv, "em:z:")) != -1) {
        switch(c) {
            case 'e' :
                flags |= O_EXCL;
                break;

            case 'm':
                attr.mq_maxmsg = atol(optarg);
                break;

            case 'z' :
                attr.mq_msgsize = atol(optarg);
                break;
        }
    }

    if (optind != argc - 1) {
        printf("usage : %s [-e] [ -m maxmsg -z msgsize] <name>\n", argv[0]);
        exit(1);
    }

    if ((attr.mq_maxmsg != 0 && attr.mq_msgsize == 0) ||
        (attr.mq_maxmsg == 0 && attr.mq_msgsize != 0)) {
        printf("must specify both -m maxmsg and -z msgsize\n");
        exit(1);
    }

    mqd_t mqd;

    printf("attr.mq_flags = %ld\n"
           "attr.mq_maxmsg = %ld\n"
           "attr.mq_msgsize = %ld\n"
           "attr.mq_curmsgs = %ld\n",
           attr.mq_flags, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    if ((mqd = mq_open(argv[optind], flags, 0644, attr.mq_maxmsg != 0 ? &attr : NULL)) < 0) {
        perror("mq_open error");
        printf("argv[optind]=%s\n", argv[optind]);
        exit(1);
    }

    if (mq_close(mqd) < 0) {
        perror("mq_close");
        exit(1);
    }
    
    return 0;    
}