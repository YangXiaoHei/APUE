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

#define MIN(_x_, _y_)           \
({  typeof(_x_) __x = (_x_);    \
    typeof(_y_) __y = (_y_);    \
    __x < __y ? __x : __y;      \
})

#define MAX_N_ITEMS 1000000
#define MAX_N_THREADS 100

struct {
    pthread_mutex_t mutex;
    int     nput;
    int     nval;
    int     nget;
    int     buf[MAX_N_ITEMS];
} shared = {
    PTHREAD_MUTEX_INITIALIZER
};

struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready;
    int nfinish;
} condition = {
    PTHREAD_MUTEX_INITIALIZER, 
    PTHREAD_COND_INITIALIZER
};

int nitems;

void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&shared.mutex);
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }
        shared.buf[shared.nput++] = shared.nval++;
        pthread_mutex_unlock(&shared.mutex);

        pthread_mutex_lock(&condition.mutex);
        if (condition.nready == 0) 
            pthread_cond_broadcast(&condition.cond);
        condition.nready++;
        printf("🍎 ----- producer increment nready to %d\n", condition.nready);
        pthread_mutex_unlock(&condition.mutex);

        *((int *)arg) += 1;
    } 
    return NULL;
}

void *consumer(void *arg) {

    int index = *(int *)arg;

    while (1) {
        pthread_mutex_lock(&condition.mutex);

        if (condition.nfinish >= nitems) {
            printf("consumer %d finished\n", index);
            pthread_mutex_unlock(&condition.mutex);
            return NULL;
        }

        while (condition.nready == 0) {
            printf("consumer %d sleep 🏊\n", index);
            pthread_cond_wait(&condition.cond, &condition.mutex);
            printf("consumer %d wake up 👼\n", index);
        }
        condition.nready--;
        condition.nfinish++;
        printf("consumer %d decrement nready to %d --- 🍇\n", index, condition.nready);
        pthread_mutex_unlock(&condition.mutex);

        pthread_mutex_lock(&shared.mutex);
        if (shared.nget >= nitems) {
            printf("consumer %d finished shared.nget = %d nitems = %d\n", index, shared.nget, nitems);
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }
        printf("consumer %d get %d \n",index, shared.buf[shared.nget++]);
        pthread_mutex_unlock(&shared.mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("usage : %s <nthreads><nitems>\n", argv[0]);
        exit(1);
    }

    int nthreads = MIN(atoi(argv[1]), MAX_N_THREADS);
    nitems = MIN(atoi(argv[2]), MAX_N_ITEMS);

    pthread_t tid_producer[MAX_N_THREADS], tid_consumer[2];
    int count[MAX_N_THREADS];

    int consumer_index[2] = {1, 2};

    for (int i = 0; i < nthreads; i++) {
        count[i] = 0;
        if (pthread_create(tid_producer + i, NULL, producer, count + i) != 0) {
            printf("pthread_create error");
            exit(1);
        }
    }
    for (int i = 0; i < 2; i++) {
        if (pthread_create(tid_consumer + i, NULL, consumer, consumer_index + i) != 0) {
            printf("pthread_create error");
            exit(1);
        }
    }

    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(tid_producer[i], NULL) != 0) {
            printf("pthread_join error");
            exit(1);
        }
        // printf("producer finished\n", i);
    }
    for (int i = 0; i < 2; i++) {
        if (pthread_join(tid_consumer[i], NULL) != 0) {
            printf("pthread_join error");
            exit(1);
        }
    }
    
    return 0;    
}