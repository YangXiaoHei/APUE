#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int x = 0, y = 1;

pthread_cond_t if_xgey = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *th1(void *arg) {
    pthread_mutex_lock(&lock);
    while (x >= y)
        pthread_cond_wait(&if_xgey, &lock);
    printf("thread ❌ x = %d y = %d\n", x, y);
    x++;
    pthread_mutex_unlock(&lock);
    pthread_exit((void *)1);
}

void *th2(void *arg) {
    pthread_mutex_lock(&lock);
    if (x < y) {
        printf("thread ✅ x = %d y = %d\n", x, y);
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&if_xgey);
    } else {
        y++;
        pthread_mutex_unlock(&lock);
    }
    pthread_exit((void *)2);
}

int main() {

    int err = 0;
    pthread_t t1, t2;
    long i = 1, j = 2;
    if ((err = pthread_create(&t1, NULL, th1, (void *)i)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_create(&t2, NULL, th2, (void *)j)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    
    while (1);
    
    return 0;
}
