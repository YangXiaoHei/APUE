#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void) {
    int err;
    printf("prepareing locks...\n");
    if ((err = pthread_mutex_lock(&lock1)) != 0) {
        printf("pthread_mutex_lock err in prepare handler : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_lock(&lock2)) != 0) {
        printf("pthread_mutex_lock err in prepare handler : %s\n", strerror(err));
        exit(1);
    }
}

void parent(void)
{
    int err;
    printf("parent unlocking locks...\n");
    if ((err = pthread_mutex_unlock(&lock1)) != 0) {
        printf("pthread_mutex_unlock err in parent handler : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_unlock(&lock2)) != 0) {
        printf("pthread_mutex_unlock err in parent handler : %s\n", strerror(err));
        exit(1);
    }
}

void child(void)
{
    int err;
    printf("parent unlocking locks...\n");
    if ((err = pthread_mutex_unlock(&lock1)) != 0) {
        printf("pthread_mutex_unlock err in child handler : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_unlock(&lock2)) != 0) {
        printf("pthread_mutex_unlock err in child handler : %s\n", strerror(err));
        exit(1);
    }
}

void *thr_fn(void *arg) {
    printf("thread started...\n");
    pause();
    return 0;
}

int main(int argc, char const *argv[])
{
    int err;
    pid_t pid;
    pthread_t tid;

    if ((err = pthread_atfork(prepare, parent, child)) != 0) {
        printf("pthread_atfork err : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0) {
        printf("pthread_create err : %s\n", strerror(err));
        exit(1);
    }

    sleep(2);
    printf("parent about to fork...\n");

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        printf("child returned from fork\n");
    } else {
        printf("parent returned from fork\n");
    }
    exit(0);

    return 0;
}