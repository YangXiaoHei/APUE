#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

void *thr1(void *arg) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    int count = 0;
    sigwait(&mask, &count);
    printf("thread 1 count = %d\n", count);
    return (void *)1;
}

void *thr2(void *arg) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    int count = 0;
    sigwait(&mask, &count);
    printf("thread 2 count = %d\n", count);
    return (void *)1;
}

void *thr3(void *arg) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    int count = 0;
    sigwait(&mask, &count);
    printf("thread 3 count = %d\n", count);
    return (void *)1;
}

void sig_int(int signo) {
    
    printf("sig_init\n");
    
}

int main() {
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    
    signal(SIGINT, sig_int);
    
    int err;
    pthread_t t1, t2, t3;
    
    pthread_create(&t2, NULL, thr2, NULL);
    pthread_create(&t1, NULL, thr1, NULL);
    pthread_create(&t3, NULL, thr3, NULL);
    
    sigset_t empty_mask;
    empty_mask = 0;
    sigsuspend(&empty_mask);
    
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t1, NULL);
    
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    
    while (1);
    
    return 0;
}
