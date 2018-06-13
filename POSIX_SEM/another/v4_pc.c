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

#define SEM_MUTEX "/tmp/mutex.sem"
#define SEM_NSTORED "/tmp/nstored.sem"
#define SEM_NEMPTY "/tmp/nempty.sem"

sem_t *mutex, *nempty, *nstored;

#define MAX_N_ITEMS 10
int circular_buffer[MAX_N_ITEMS];
int nitems;

void *producer(void *arg) {
    for (int i = 0; i < nitems; i++) {
        sem_wait(nempty);
        sem_wait(mutex);
        circular_buffer[i % MAX_N_ITEMS] = i;
        printf("👼 ------- producer put %d\n", circular_buffer[i % MAX_N_ITEMS]);
        sem_post(mutex);
        sem_post(nstored);
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < nitems; i++) {
        sem_wait(nstored);
        sem_wait(mutex);
        printf("consumer get %d\n", circular_buffer[i %  MAX_N_ITEMS]);
        sem_post(mutex);
        sem_post(nempty);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage : %s <nitems>\n", argv[0]);
        exit(1);
    }

    nitems = atoi(argv[1]);

    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NEMPTY);
    sem_unlink(SEM_NSTORED);

    if ((mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
    if ((nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL, 0644, MAX_N_ITEMS)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
    if ((nstored = sem_open(SEM_NSTORED, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    pthread_t tid_producer, tid_consumer;
    int err;
    if ((err = pthread_create(&tid_producer, NULL, producer, NULL)) < 0) {
        printf("pthread_create error : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_create(&tid_consumer, NULL, consumer, NULL)) < 0) {
        printf("pthread_create error : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_join(tid_consumer, NULL)) < 0) {
        printf("pthread_join error : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_join(tid_producer, NULL)) < 0) {
        printf("pthread_join error : %s\n", strerror(err));
        exit(1);
    }


    
    return 0;    
}