#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    
    int err;
    pthread_mutexattr_t mutex_attr;
    if ((err = pthread_mutexattr_init(&mutex_attr)) != 0) {
        printf("pthread_mutexattr_init fail\n");
        exit(1);
    }
    if ((err = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE)) != 0) {
        printf("phtread_mutexattr_settype fail\n");
        exit(1);
    }
    
    pthread_mutex_t lock;
    if ((err = pthread_mutex_init(&lock, &mutex_attr)) != 0) {
        printf("pthread_mutex_init fail\n");
        exit(1);
    }
    
    printf("before lock\n");
    pthread_mutex_lock(&lock);
    printf("after lock\n");
    
    printf("before lock\n");
    pthread_mutex_lock(&lock);
    printf("after lock\n");
    
    return 0;
}

