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

#define MAX_N_ITEMS 10
int nitems;

struct {
    sem_t *mutex, *nempty, *nstored;
    int buffer[MAX_N_ITEMS];
    int seqno;
    int nput;
    int nget;

    int stored_cnt;
} shared;

pthread_cond_t empty    = PTHREAD_COND_INITIALIZER;
pthread_cond_t full     = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock    = PTHREAD_MUTEX_INITIALIZER;

void *producer(void *arg) {
    int index = *(int *)arg;
    while (1) {
        // sem_wait(shared.nempty);
        // sem_wait(shared.mutex);
        // shared.buffer[shared.nput++ % MAX_N_ITEMS] = shared.seqno++;
        // printf("producer %d put %d\n", index, shared.buffer[(shared.nput - 1) % MAX_N_ITEMS]);
        // usleep(100000);
        // sem_post(shared.mutex);
        // sem_post(shared.nstored);

        pthread_mutex_lock(&lock);
        while (shared.stored_cnt >= nitems)
            pthread_cond_wait(&full, &lock);
        shared.buffer[shared.nput++ % MAX_N_ITEMS] = shared.seqno++;
        shared.stored_cnt++;
        printf("producer %d put %d\n", index, shared.buffer[(shared.nput - 1) % MAX_N_ITEMS]);
        usleep(rand() % 1000000 + 100);
        if (shared.stored_cnt == 1) 
            pthread_cond_broadcast(&empty);
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

void *consumer(void *arg) {
    int index = *(int *)arg;
    while (1) {
        // sem_wait(shared.nstored);
        // sem_wait(shared.mutex);
        // printf("consumer %d get %d\n", index, shared.buffer[shared.nget++ % MAX_N_ITEMS]);
        // sem_post(shared.mutex);
        // sem_post(shared.nempty);

        pthread_mutex_lock(&lock);
        while (shared.stored_cnt == 0)
            pthread_cond_wait(&empty, &lock);
        printf("consumer %d get %d\n", index, shared.buffer[shared.nget++ % MAX_N_ITEMS]);
        shared.stored_cnt--;
        if (shared.stored_cnt == nitems - 1) 
            pthread_cond_broadcast(&full);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

#define SEM_NEMPTY  "nempty"
#define SEM_NSTROED "nstored"
#define SEM_MUTEX   "mutex"

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
        printf("usage : %s <npros> <ncons> <capacity>\n", argv[0]);
        exit(1);
    }

    // sem_unlink(SEM_MUTEX);
    // sem_unlink(SEM_NSTROED);
    // sem_unlink(SEM_NEMPTY);

    int npros = atoi(argv[1]);
    int ncons = atoi(argv[2]);
    nitems = atoi(argv[3]);

    // shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    // shared.nstored = sem_open(SEM_NSTROED, O_CREAT | O_EXCL, 0644, 0);
    // shared.nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL, 0644, nitems);

    int cons[ncons];
    int pros[npros];

    pthread_t tid_producer[npros], tid_consumer[ncons];
    
    for (int i = 0; i < npros; i++) {
        pros[i] = i;
        pthread_create(tid_producer + i, NULL, producer, pros + i);
    }
    for (int i = 0; i < ncons; i++) {
        cons[i] = i;
        pthread_create(tid_consumer + i, NULL, consumer, cons + i);
    }
    

    alarm(5);
    pause();
    
    return 0;    
}