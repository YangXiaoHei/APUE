#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_start(void *arg) {
    printf("pid : %ld, new thread : %lu\n",(long)getpid(), (unsigned long)pthread_self());
    return (void *)1;
}


int main(int argc, char *argv[]) {
    
    pthread_t tid;
    int err = 0;
    if ((err = pthread_create(&tid, NULL, thread_start, NULL)) != 0) {
        perror("pthread create fail");
        exit(1);
    }
    printf("pid : %ld, main thread : %lu  new thread : %lu\n",(long)getpid(), (unsigned long)pthread_self(), (unsigned long)tid);
    
    sleep(1);
    
    return 0;
}
