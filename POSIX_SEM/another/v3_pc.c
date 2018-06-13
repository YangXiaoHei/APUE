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


#define CIRCULAR_BUFFER_SIZE 2
int circular_queue[CIRCULAR_BUFFER_SIZE];
int tmp;

struct {
    pthread_cond_t full;
    pthread_cond_t empty;

    pthread_mutex_t lock;
    int put_cursor;
    int get_cursor;
    int size;

    int val;

} cond = {
    PTHREAD_COND_INITIALIZER,
    PTHREAD_COND_INITIALIZER,

    PTHREAD_MUTEX_INITIALIZER,
};

void print_queue(void) {
    printf("【");
    for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i++) 
        printf("%3d", circular_queue[i]);
    printf("】\n");
} 

void msleep(unsigned int msec) {
    usleep(msec * 1000);
}

unsigned int yh_rand(unsigned int n, unsigned int m) {
    return rand() % (m - n + 1) + n;
}

void *producer(void *arg) {
    int index = *(int *)arg;
    while (1) 
    {    
        pthread_mutex_lock(&cond.lock);
        while (cond.size >= CIRCULAR_BUFFER_SIZE) {
            printf("👼 producer %d suspend\n", index);
            pthread_cond_wait(&cond.full, &cond.lock);
            printf("🏊 producer %d hold a lock\n", index);
        }

        tmp = cond.put_cursor;
        circular_queue[cond.put_cursor] = cond.val++;
        cond.put_cursor = (cond.put_cursor + 1) % CIRCULAR_BUFFER_SIZE;
        cond.size++;
        printf("✅--------producer %d put %d, now left %d\n", index, circular_queue[tmp], cond.size);

        // msleep(yh_rand(100, 500));

        if (cond.size == 1) {
            printf("🍁🍁🍁🍁🍁🍁🍁🍁  producer notify all 🍁🍁🍁🍁🍁🍁🍁🍁\n");
            pthread_cond_broadcast(&cond.empty);
        }

        pthread_mutex_unlock(&cond.lock);
    }
}

void *consumer(void *arg) {
    int index = *(int *)arg;
    while (1) 
    {
        pthread_mutex_lock(&cond.lock);
        while (cond.size <= 0) {
            printf("👼 consumer %d suspend\n", index);
            pthread_cond_wait(&cond.empty, &cond.lock);
            printf("🏊 consumer %d hold a lock\n", index);
        }

        printf("❌consumer %d get %d, now left %d\n", index, circular_queue[cond.get_cursor], cond.size - 1);

        circular_queue[cond.get_cursor] = -1;
        cond.get_cursor = (cond.get_cursor + 1) % CIRCULAR_BUFFER_SIZE;
        cond.size--;

        // msleep(yh_rand(500, 1000));

        if (cond.size == CIRCULAR_BUFFER_SIZE - 1) {
            printf("🍁🍁🍁🍁🍁🍁🍁🍁  consumer notify all 🍁🍁🍁🍁🍁🍁🍁🍁\n");
            pthread_cond_broadcast(&cond.full);
        }

        pthread_mutex_unlock(&cond.lock);
    }
}
 

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("usage : %s <nproducers><nconsumers>\n", argv[0]);
        exit(1);
    }

    setbuf(stdout, NULL);

    for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i++) 
        circular_queue[i] = -1;
        
    int npros = atoi(argv[1]);
    int ncons = atoi(argv[2]);

    pthread_t tid_producer[npros], tid_consumer[ncons];

    int pros[npros];
    int cons[ncons];

    int err;

    for (int i = 0; i < npros; i++) {
        pros[i] = i;
        if ((err = pthread_create(tid_producer + i, NULL, producer, pros + i)) < 0) {
            printf("pthread_create error : %s", strerror(err));
            exit(1);
        }
    }
    for (int i = 0; i < ncons; i++) {
        cons[i] = i;
        if ((err = pthread_create(tid_consumer + i, NULL, consumer, cons + i)) < 0) {
            printf("pthread_create error : %s", strerror(err));
            exit(1);
        }
    }

    for (int i = 0; i < npros; i++) {
        if ((err = pthread_join(tid_producer[i], NULL)) < 0) {
            printf("pthread_create error : %s", strerror(err));
            exit(1);
        }
    }
        
    for (int i = 0; i < ncons; i++) {
        if ((err = pthread_join(tid_consumer[i], NULL)) < 0) {
            printf("pthread_create error : %s", strerror(err));
            exit(1);
        }
    }
    
    return 0;    
}