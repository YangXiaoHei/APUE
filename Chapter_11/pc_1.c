
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

#define MIN(_x_, _y_) \
({                              \
    typeof(_x_) __x = (_x_);    \
    typeof(_y_) __y = (_y_);    \
    __x < __y ? __x : __y;      \
})

#define MAX_N_ITEMS 1000000
#define MAX_N_THREADS   100

struct {
    pthread_mutex_t mutex;
    int     nput;
    int     nval;
    int     buf[MAX_N_ITEMS];
} shared = {
    PTHREAD_MUTEX_INITIALIZER 
};

int nitems;

void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&shared.mutex);
        if (shared.nput >= nitems) {
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }
        shared.buf[shared.nput] = shared.nval;
        shared.nput++;
        shared.nval++;
        pthread_mutex_unlock(&shared.mutex);
        *((int *)arg) += 1;
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < nitems; i++) {
        while (1) {
            pthread_mutex_lock(&shared.mutex);
            if (i < shared.nput) {
                pthread_mutex_unlock(&shared.mutex);
                break;
            }
            pthread_mutex_unlock(&shared.mutex);
        }
        printf("consumer get %d \n", shared.buf[i]);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("usage : %s <nthreads><nitems>\n", argv[0]);
        exit(1);
    }
    
    pthread_t tid_producer[MAX_N_THREADS];
    pthread_t tid_consumer;

    int nthreads = MIN(atoi(argv[1]), MAX_N_THREADS);
    nitems = MIN(atoi(argv[2]), MAX_N_ITEMS);

    int err;

    int count[MAX_N_THREADS];
    for (int i = 0; i < nthreads; i++) {
        count[i] = 0;
        if (pthread_create(tid_producer + i, NULL, producer, count + i) != 0) {
            printf("pthread_create error");
            exit(1);
        }
    }
    if (pthread_create(&tid_consumer, NULL, consumer, NULL) != 0) {
        printf("pthread_create error");
        exit(1);
    }

    for (int i = 0; i < nthreads; i++) {
        if ((err = pthread_join(tid_producer[i], NULL)) != 0) {
            printf("pthread_join producer error %s\n", strerror(err));
            exit(1);
        }
    }
    if ((err = pthread_join(tid_consumer, NULL)) != 0) {
        printf("pthread_join consumer error %s\n", strerror(err));
        exit(1);
    }

    return 0;    
}