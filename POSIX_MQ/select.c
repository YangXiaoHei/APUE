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
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int fds[2];

void sig_usr1(int signo) {
    if (write(fds[1], "a", 1) != 1) {
        perror("write error");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{

    if (argc != 2) {
        printf("usage : %s <name>\n", argv[0]);
        exit(1);
    }

    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    // mqd_t mq = mq_open(argv[1], O_RDONLY);
    mqd_t mq = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mq < 0) {
        perror("open error");
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

    struct sigevent sigev;
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGUSR1;
    if (mq_notify(mq, &sigev) < 0) {
        perror("mq_notify error");
        exit(1);
    }

    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    fd_set rset;
    FD_ZERO(&rset);

    int nfd;
    int n;

    while (1) {
        FD_ZERO(&rset);
        FD_SET(fds[0], &rset);
try_again:
        nfd = select(fds[0] + 1, &rset, NULL, NULL, NULL);
        if (nfd < 0) {
            if (errno == EINTR)
                goto try_again;
            perror("select error");
            exit(1);
        }

        int c;
        if (FD_ISSET(fds[0], &rset)) {
            if (read(fds[0], &c, 1) < 0) {
                perror("read error");
                exit(1);
            }

            if (mq_notify(mq, &sigev) < 0) {
                perror("mq_notify error");
                exit(1);
            }
            // n = mq_receive(mq, buffer, attr.mq_msgsize, NULL);
            // if (n < 0) {
            //     perror("mq_receive error");
            //     exit(1);
            // }
            // printf("read %d bytes\n", n);
            
            while (n = mq_receive(mq, buffer, attr.mq_msgsize, NULL) >= 0) {
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