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
    pthread_mutex_t lock;
    pthread_cond_t cond;
};


int total;

struct queue *q;
int nitems;

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
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
}

int size(void) {
    pthread_mutex_lock(&q->lock);
    int s = q->size;
    pthread_mutex_unlock(&q->lock);
    return s;
}

void enqueue(int *value, int *size) {
    pthread_mutex_lock(&q->lock);
    struct node *newn = malloc(sizeof(struct node));
    if (newn == NULL) {
        printf("malloc error");
        exit(1);
    }
    newn->next = q->tailer;
    newn->prev = q->tailer->prev;
    q->tailer->prev->next = newn;
    q->tailer->prev = newn;
    newn->index = ++total;
    if (value != NULL) 
        *value = total;
    // usleep(1000);
    q->size++;
    if (size != NULL) 
        *size = q->size;
    pthread_mutex_unlock(&q->lock);
}

int empty(void) {
    pthread_mutex_lock(&q->lock);
    int isempty = q->size == 0;
    pthread_mutex_unlock(&q->lock);
    return isempty;
} 

int dequeue(int *size) {
    pthread_mutex_lock(&q->lock);
    if (q->size == 0) {
        pthread_mutex_unlock(&q->lock);
        return -1;
    }
    struct node *del = q->header->next;
    del->prev->next = del->next;
    del->next->prev = del->prev;
    int value = del->index;
    free(del);
    q->size--;
    if (size != NULL) *size = q->size;
    pthread_mutex_unlock(&q->lock);
    return value;
}

void *producer(void *arg) {
    int index = *(int *)arg;
    for (int i = 0 ;; i++) {
        int size, value;
        enqueue(&value, &size);
        printf("producer %d put %d, current size %d\n",index, value, size);
    }
    return NULL;
}

void *consumer(void *arg) {
    int index = *(int *)arg;
    for (int i = 0 ;; i++) {
        // while (empty());
        int size;
        int value = dequeue(&size);
        printf("consumer %d get %d, current size %d\n",index, value, size);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    
    init();

    int npros = 1;
    int ncons = 3;

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
    
    alarm(4);
    pause();
    
    return 0;    
}