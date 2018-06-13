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

#define MAX_N_ITEMS 1000000
#define MAX_N_THREADS 100
int queue[MAX_N_ITEMS];

int nitems;

struct {
    pthread_mutex_t producer_lock;
    int  nput;
    int  nval;
} shared = {
    PTHREAD_MUTEX_INITIALIZER
};

void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&shared.producer_lock);
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.producer_lock);
            return NULL;
        }
        queue[shared.nput++] = shared.nval++;
        pthread_mutex_unlock(&shared.producer_lock);
        *(int *)arg += 1;
    }
    return NULL;
}

void *consumer(void *arg) {
    // int index = *(int *)arg;
    for (int i = 0; i < nitems; i++) {
        while (1) {
            pthread_mutex_lock(&shared.producer_lock);
            if (i < shared.nput) {
                pthread_mutex_unlock(&shared.producer_lock);
                break;
            }
            pthread_mutex_unlock(&shared.producer_lock);
        }

        // printf("consumer get %d\n", queue[i]);

    }
    return NULL;
}

int min(int x, int y) { return x < y ? x : y; }

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("usage : %s <nthreads><nitems>\n", argv[0]);
        exit(1);
    }
        
    int nthreads = min(atoi(argv[1]), MAX_N_THREADS);
    nitems = min(atoi(argv[2]), MAX_N_ITEMS);

    pthread_t tid_producer[nthreads], tid_consumer;

    int count[nthreads];

    for (int i = 0; i < nthreads; i++) {
        count[i] = 0;
        if (pthread_create(tid_producer + i, NULL, producer, count + i) < 0) {
            printf("pthread_create error");
            exit(1);
        }
    }
    if (pthread_create(&tid_consumer, NULL, consumer, NULL) < 0) {
        printf("pthread_create error");
        exit(1);
    }

    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(tid_producer[i], NULL) < 0) {
            printf("pthread_create error");
            exit(1);
        }
        printf("count[%d] = %d\n", i, count[i]);
    }
        
    if (pthread_join(tid_consumer, NULL) < 0) {
        printf("pthread_create error");
        exit(1);
    }
    
    return 0;    
}