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

struct node {
    int index;
    struct node *next;
    struct node *prev;
};

struct queue {
    struct node *header;
    struct node *tailer;
    int size;
};

struct queue *q;
int nitems;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int put_cursor;

void init(void) {
    q = malloc(sizeof(struct queue));
    if (q == NULL) {
        printf("malloc error");
        exit(1);
    }
    q->header = malloc(sizeof(struct node));
    q->tailer = malloc(sizeof(struct node));
    if (q->header == NULL || q->tailer == NULL) {
        printf("malloc error");
        exit(1);
    }
    q->header->next = q->tailer;
    q->tailer->prev = q->header;
    q->tailer->next = NULL;
    q->header->prev = NULL;
    q->header->index = -1;
    q->tailer->index = -1;
    q->size = 0;
}

int size(void) {
    pthread_mutex_lock(&lock);
    int s = q->size;
    pthread_mutex_unlock(&lock);
    return s;
}

void enqueue(int value) {
    pthread_mutex_lock(&lock);
    struct node *newn = malloc(sizeof(struct node));
    if (newn == NULL) {
        printf("malloc error");
        exit(1);
    }
    newn->next = q->tailer;
    newn->prev = q->tailer->prev;
    q->tailer->prev->next = newn;
    q->tailer->prev = newn;
    newn->index = value;
    usleep(1000);
    q->size++;
    pthread_mutex_unlock(&lock);
}

int empty(void) {
    pthread_mutex_lock(&lock);
    int isempty = q->size == 0;
    pthread_mutex_unlock(&lock);
    return isempty;
} 

int dequeue(void) {
    pthread_mutex_lock(&lock);
    if (q->size == 0) {
        pthread_mutex_unlock(&lock);
        return -1;
    }
    struct node *del = q->header->next;
    del->prev->next = del->next;
    del->next->prev = del->prev;
    int value = del->index;
    free(del);
    q->size--;
    pthread_mutex_unlock(&lock);
    return value;
}

void *producer(void *arg) {
    int index = *(int *)arg;
    for (int i = 0 ;; i++) {
        enqueue(++put_cursor);
        printf("producer %d put %d, current size %d\n",index, put_cursor, size());
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0 ;; i++) {
        while (empty());
        int value = dequeue();
        printf("consumer get %d, current size %d\n", value, size());
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    init();

    int nthreads = 1;

    pthread_t tid_producer[nthreads], tid_consumer;
    int pros[5];
    for (int i = 0; i < nthreads; i++) {
        pros[i] = i;
        pthread_create(tid_producer + i, NULL, producer, pros + i);
    }
    pthread_create(&tid_consumer, NULL, consumer, NULL);

    alarm(2);
    pause();
    
    return 0;    
}