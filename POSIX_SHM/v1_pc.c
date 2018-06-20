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

int count;


int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("usage : %s <nloop>\n", argv[0]);
        exit(1);
    }

    int nloop = atoi(argv[1]);

    pid_t pid;

    sem_unlink("mutex");

    sem_t *mutex = sem_open("mutex", O_CREAT | O_EXCL, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        for (int i = 0; i < nloop; i++) {
            sem_wait(mutex);
            printf("parent %d\n", count++);
            sem_post(mutex);
        }
        exit(1);
    }

    for (int i = 0; i < nloop; i++) {
        sem_wait(mutex);
        printf("child %d\n", count++);
        sem_post(mutex);
    }

    
    return 0;    
}