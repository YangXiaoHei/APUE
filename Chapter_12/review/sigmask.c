#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

void print_thread_mask(const char *msg) {
    printf("%s : ", msg);
    int err = 0;
    sigset_t old_mask;
    sigemptyset(&old_mask);
    if ((err = pthread_sigmask(0, NULL, &old_mask)) != 0) {
        printf("pthread_sigmask fail : %s\n", strerror(err));
        exit(1);
    }
    for (int i = 0; i < NSIG; i++)
        if (sigismember(&old_mask, i))
            printf("【%d】", i);
    printf("\n");
}

void print_process_mask(const char *msg) {
    printf("%s : ", msg);
    int err = 0;
    sigset_t old_mask;
    sigemptyset(&old_mask);
    if ((err = sigprocmask(0, NULL, &old_mask)) != 0) {
        printf("sigprocmask fail : %s\n", strerror(err));
        exit(1);
    }
    for (int i = 0; i < NSIG; i++)
        if (sigismember(&old_mask, i))
            printf("【%d】", i);
    printf("\n");
}

void *run(void *arg) {
    print_thread_mask("new thread step 1");
    
    sigset_t new_mask;
    sigaddset(&new_mask, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &new_mask, NULL);
    
    print_thread_mask("new thread step 2");
    
    return (void *)1;
}

int main() {
    
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    if (sigprocmask(SIG_BLOCK, &new_mask, NULL) != 0) {
        perror("sigprocmask error");
        exit(1);
    }
    
    print_process_mask("main thread step 1");
    
    int err = 0;
    pthread_t t1;
    if ((err = pthread_create(&t1, NULL, run, NULL)) != 0) {
        printf("pthread_create fail : %s\n", strerror(err));
        exit(1);
    }
    
    sleep (3);
    
    print_process_mask("main thread step 2");
    
    while (1);
    
    return 0;
}
