#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *thread_start1(void *arg) {
    printf("thread 1 return\n");
    sleep(2);
    return (void *)1;
}

void *thread_start2(void *arg) {
    printf("thread 2 exit\n");
    sleep(4);
    return (void *)2;
}


int main() {
    
    int err;
    pthread_t tid1, tid2;
    void *tret;
    
    if ((err = pthread_create(&tid1, NULL, thread_start1, NULL)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_create(&tid2, NULL, thread_start2, NULL)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    
    if ((err = pthread_join(tid1, &tret)) != 0) {
        printf("pthread join fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("thread 1 exit code : %ld\n", (long)tret);
    
    if ((err = pthread_join(tid2, &tret)) != 0) {
        printf("pthread join fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("thread 2 exit code : %ld\n", (long)tret);
    
    exit(0);
}
