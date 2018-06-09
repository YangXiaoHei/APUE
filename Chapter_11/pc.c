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
#include <math.h>

long min(long x, long y) {
    return x < y ? x : y;
}


#define MAX_ITEMS_NUM 1000000
#define MAX_THREADS_NUM 100

int nitems;

struct {
    pthread_mutex_t mutex;
    int     buff[MAX_ITEMS_NUM];
    int     nput;
    int     nval;
} shared = {
    PTHREAD_MUTEX_INITIALIZER
};

void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&shared.mutex);
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }
        shared.buff[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        pthread_mutex_unlock(&shared.mutex);
        *((int *)arg) += 1;
    }
}

void *consumer(void *arg) {
    for (int i = 0; i < nitems; i++) {
        if (shared.buff[i] != i) {
            printf("buff[%d] = %d\n", i, shared.buff[i]);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("usage : %s <nthreads><nitems>\n", argv[0]);
        exit(1);
    }

    int nthreads = min(atoi(argv[1]), MAX_THREADS_NUM);
    nitems = min(atoi(argv[2]), MAX_ITEMS_NUM);

    pthread_t tid_producer[MAX_THREADS_NUM];
    int count[MAX_THREADS_NUM];
    for (int i = 0; i < nthreads; i++) {
        count[i] = 0;
        if (pthread_create(tid_producer + i, NULL, producer, count + i) != 0) {
            printf("pthread create error\n");
            exit(1);
        }
    }

    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(tid_producer[i], NULL) != 0) {
            printf("pthread join error");
            exit(1);
        }
        printf("count[%d] = %d\n", i, count[i]);
    }

    pthread_t tid_consumer;
    if (pthread_create(&tid_consumer, NULL, consumer, NULL) != 0) {
        printf("pthread create error\n");
        exit(1);
    }
    if (pthread_join(tid_consumer, NULL) != 0) {
        printf("pthread join error");
        exit(1);
    }

    return 0;    
}