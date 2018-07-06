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

void *buffer;
struct mq_attr attr;
struct sigevent sigev;
mqd_t mq;

void notify_thread(union sigval arg) {

    int n;
    printf("notify_thread started\n");
    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        printf("malloc error");
        exit(1);
    }
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
    free(buffer);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        printf("usage : %s <name>\n", argv[0]);
        exit(1);
    }

    mq = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mq < 0) {
        perror("mq_open error");
        exit(1);
    }

    if (mq_getattr(mq, &attr) < 0) {
        perror("mq_getattr error");
        exit(1);
    }

    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_value.sival_ptr = NULL;
    sigev.sigev_notify_function = notify_thread;
    sigev.sigev_notify_attributes = NULL;
    if (mq_notify(mq, &sigev) < 0) {
        perror("mq_notify error");
        exit(1);
    }

    while (1)
        pause();
    
    return 0;
}