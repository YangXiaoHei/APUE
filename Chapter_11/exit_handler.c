#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void cleanup(void *arg) {
    printf("cleanup : %s\n", (char *)arg);
}


void *th1(void *arg) {
    printf("thread 1 start\n");
    pthread_cleanup_push(cleanup, "thread 1 first handler");
    pthread_cleanup_push(cleanup, "thread 1 second handler");
    printf("thread 1 push complete\n");
    if (arg)
        pthread_exit((void *)1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void *)1;
}

void *th2(void *arg) {
    printf("thread 2 start\n");
    pthread_cleanup_push(cleanup, "thread 2 first handler");
    pthread_cleanup_push(cleanup, "thread 2 second handler");
    printf("thread 2 push complete\n");
    if (arg)
        pthread_exit((void *)2);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    pthread_exit((void *)1);
}

int main(int argc, char *argv[]) {
    
    int err;
    pthread_t t1, t2;
    if ((err = pthread_create(&t1, NULL, th1, (void *)1)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_create(&t2, NULL, th2, (void *)2)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    
    void *ret = NULL;
    if ((err = pthread_join(t1, &ret)) != 0) {
        printf("pthread join 1 fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("thread 1 exit with code : %ld\n", (long)ret);
    
    if ((err = pthread_join(t2, &ret)) != 0) {
        printf("pthread join 2 fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("thread 2 exit with code : %ld\n", (long)ret);
    
    return 0;
}
