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

void enqueue(int value) {
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
    q->size++;
}

int empty(void) {
    return q->size == 0;
} 

int dequeue(void) {
    if (empty()) return -1;
    struct node *del = q->header->next;
    del->prev->next = del->next;
    del->next->prev = del->prev;
    int value = del->index;
    free(del);
    q->size--;
    return value;
}



int nitems;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int put_cursor;

void *producer(void *arg) {
    int index = *(int *)arg;
    for (int i = 0 ;; i++) {
        pthread_mutex_lock(&lock);
        enqueue(++put_cursor);
        usleep(1000 * 10);
        printf("producer %d put %d, current size %d\n",index, put_cursor, q->size);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}


void *consumer(void *arg) {
    int size;
    for (int i = 0 ;; i++) {
        while (1) 
        {
            pthread_mutex_lock(&lock);
            if (i < put_cursor) {
                size = q->size - 1;
                pthread_mutex_unlock(&lock);
                break;
            }
            pthread_mutex_unlock(&lock);
        }
        int value = dequeue();
        printf("consumer get %d, current size %d\n", value, size);
        
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