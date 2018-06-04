#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

#define LOCK(...)                   \
do {                                \
    pthread_mutex_lock(&lock);      \
    __VA_ARGS__;                    \
    pthread_mutex_unlock(&lock);    \
} while (0);

int count;

void *thr1(void *arg) {
    for (int i = 0; i < 100; i++) 
        LOCK(count++; printf("thread 1 --- count %d\n", count);)
    return (void *)1;
}

void *thr2(void *arg) {
    for (int i = 0; i < 100; i++) 
        LOCK(count++; printf("thread 2 --- count %d\n", count);)
    return (void *)1;
}


int main(int argc, char *argv[]) {
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thr1, NULL);
    pthread_create(&t2, NULL, thr2, NULL);
    
    pause();
    
    return 0;    
}