#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

int quit_flag;
sigset_t mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_lock = PTHREAD_COND_INITIALIZER;

void *thread_start(void *arg) {
    int err, signo;
    while (1) {
        if ((err = sigwait(&mask, &signo)) != 0)
            printf("sigwait fail : %s\n", strerror(err));
        
        switch (signo) {
            case SIGINT:
                printf("\ninterrupt\n");
                break;
            case SIGQUIT:
                pthread_mutex_lock(&lock);
                quit_flag = 1;
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&wait_lock);
                return 0;
            default:
                printf("unexpected signal %d\n", signo);
                exit(1);
        }
    }
}

int main() {
    
    int err;
    sigset_t old_mask;
    pthread_t tid;
    
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, &old_mask)) != 0) {
        printf("pthread_sigmask fail : %s\n", strerror(err));
        exit(1);
    }
    
    if ((err = pthread_create(&tid, NULL, thread_start, NULL)) != 0) {
        printf("pthread_create fail : %s\n", strerror(err));
        exit(1);
    }
    
    pthread_mutex_lock(&lock);
    while (quit_flag == 0)
        pthread_cond_wait(&wait_lock, &lock);
    pthread_mutex_unlock(&lock);
    
    quit_flag = 0;
    
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask ");
        exit(1);
    }
    return 0;
}
