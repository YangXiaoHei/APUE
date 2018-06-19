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

#define MAX_N_ITEMS 100
int nitems;

struct {
    sem_t *mutex, *nempty, *nstored;
    int buffer[MAX_N_ITEMS];

    int seqno;
    int nput;
    int nget;
} shared;

void *producer(void *arg) {
    int index = *(int *)arg;
    while (1) {
        sem_wait(shared.nempty);
        sem_wait(shared.mutex);

        if (shared.nput >= nitems) {
            sem_post(shared.nstored);
            sem_post(shared.nempty);
            sem_post(shared.mutex);
            return NULL;
        }

        shared.buffer[shared.nput++] = shared.seqno++;
        printf("producer %d put %d\n", index, shared.buffer[shared.nput - 1]);
        sem_post(shared.mutex);
        sem_post(shared.nstored);
    }
    return NULL;
}

void *consumer(void *arg) {
    int index = *(int *)arg;
    while (1) {
        sem_wait(shared.nstored);
        sem_wait(shared.mutex);

        if (shared.nget >= nitems) {
            sem_post(shared.nstored);
            sem_post(shared.mutex);
            return NULL;
        }

        printf("consumer %d get %d\n", index, shared.buffer[shared.nget++]);

        sem_post(shared.mutex);
        sem_post(shared.nempty);
    }
    return NULL;
}

#define SEM_NEMPTY  "nempty"
#define SEM_NSTROED "nstored"
#define SEM_MUTEX   "mutex"

int min(int a, int b) { return a < b ? a : b; }

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
        printf("usage : %s <npros> <ncons> <capacity>\n", argv[0]);
        exit(1);
    }

    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NSTROED);
    sem_unlink(SEM_NEMPTY);

    int npros = atoi(argv[1]);
    int ncons = atoi(argv[2]);
    nitems = min(atoi(argv[3]), MAX_N_ITEMS);

    printf("nitems = %d\n", nitems);

    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    shared.nstored = sem_open(SEM_NSTROED, O_CREAT | O_EXCL, 0644, 0);
    shared.nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL, 0644, nitems);

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

    for (int i = 0; i < npros; i++) {
        pthread_join(tid_producer[i], NULL);
    }
    for (int i = 0; i < ncons; i++) {
        pthread_join(tid_consumer[i], NULL);
    }

    return 0;    
}