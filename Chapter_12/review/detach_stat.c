#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

void cleanup(void *arg) {
    printf("%s\n", (char *)arg);
}

void *thread_start(void *arg) {
    pthread_cleanup_push(cleanup, "second handler");
    pthread_cleanup_push(cleanup, "first handler");
    pthread_exit((void *)1);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
}

int main(int argc, char *argv[]) {
    
    int joinable = argc != 1;
    
    int err;
    pthread_t tid;
    
    pthread_attr_t attr;
    if ((err = pthread_attr_init(&attr)) != 0) {
        printf("pthread_attr_init : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_attr_setdetachstate(&attr, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED)) != 0) {
        printf("pthread_attr_setdetachstate fail : %s\n", strerror(err));
        exit(1);
    }
    
    if ((err = pthread_create(&tid, &attr, thread_start, NULL)) != 0) {
        printf("pthread_create fail : %s\n", strerror(err));
        exit(1);
    }
    
    void *ret = NULL;
    if ((err = pthread_join(tid, &ret)) != 0) {
        printf("pthread_join fail : %s\n", strerror(err));
        exit(1);
    } else {
        printf("pthread join exit code : %d\n", (int)ret);
    }
    return 0;
}
