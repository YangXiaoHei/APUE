#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

void *thread_start(void *arg) {
    int fd = 0;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket ");
        exit(1);
    }
    while (1) {
        sleep(1);
    };
    return NULL;
}


int main(int argc, char *argv[]) {
    
    int fd = 0;
    pthread_t tid[300] = { 0 };
    int i = 0;
    for (int i = 0; i < sizeof(tid) / sizeof(tid[0]); i++) {
        int err = 0;
        if ((err = pthread_create(tid + i, NULL, thread_start, NULL)) != 0) {
            printf("pthread_create\n");
        }
    }
    
    while (1) {
        
    };
    
    return 0;
}
