#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_start(void *arg) {
    printf("thread_start\n");
    pause();
    return (void *)1;
}

void prepare(void) {
    int err;
    printf("preparing locks...\n");
    if ((err = pthread_mutex_lock(&lock1)) != 0) {
        printf("pthread_mutex_lock :%s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_lock(&lock2)) != 0) {
        printf("pthread_mutex_lock :%s\n", strerror(err));
        exit(1);
    }
}

void parent(void) {
    int err;
    printf("parent unlocking locks...\n");
    if ((err = pthread_mutex_unlock(&lock1)) != 0) {
        printf("pthread_mutex_unlock : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_unlock(&lock2)) != 0) {
        printf("pthread_mutex_unlock : %s\n", strerror(err));
        exit(1);
    }
}

void child(void) {
    int err;
    printf("child unlocking locks...\n");
    if ((err = pthread_mutex_unlock(&lock1)) != 0) {
        printf("pthread_mutex_unlock : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_unlock(&lock2)) != 0) {
        printf("pthread_mutex_unlock : %s\n", strerror(err));
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    
    int err;
    pid_t pid;
    pthread_t tid;
    
    if ((err = pthread_atfork(prepare, parent, child)) != 0) {
        printf("pthread_atfork : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_create(&tid, NULL, thread_start, 0)) != 0) {
        printf("pthread_create fail : %s\n", strerror(err));
        exit(1);
    }
    sleep(2);

    //  死锁~
//    pthread_mutex_lock(&lock1);
//    pthread_mutex_lock(&lock2);

    printf("parent about to fork...\n");
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        printf("child returned from fork\n");
    } else {
        printf("parent returned from fork\n");
    }
    
    return 0;
}
