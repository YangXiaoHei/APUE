#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *thread_start(void *arg) {
    int err = 0;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if ((err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL)) != 0) {
        printf("pthread_setcanceltype fail : %s\n", strerror(err));
        pthread_exit((void *)-1);
    }
  
    long i = 0;
    while (1)
        i++;
    
    return (void *)1;
}

void *thread_start_1(void *arg) {
    sleep(2);
    printf("begin to cancel\n");
    pthread_cancel((pthread_t)arg);
    printf("after cancel\n");
    return (void *)2;
}

int main(int argc, char *argv[]) {
    
    int err = 0;
    pthread_t t1, t2;
    
    if ((err = pthread_create(&t1, NULL, thread_start, NULL)) != 0) {
        printf("pthread_create fail : %s\n", strerror(err));
        exit(1);
    }
    
    if ((err = pthread_create(&t2, NULL, thread_start_1, t1)) != 0) {
        printf("pthread_create fail : %s\n", strerror(err));
        exit(1);
    }
    
    if ((err = pthread_join(t1, NULL)) != 0) {
        printf("pthread_join fail : %s\n", strerror(err));
        exit(1);
    } else {
        printf("join succ\n");
    }
    
    while (1);
    
    return 0;
}
