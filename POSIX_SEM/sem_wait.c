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

int main(int argc, char *argv[]) {
    
    sem_t *sem;
    int val;

    if (argc != 2) {
        printf("usage : sem_wait <name>\n");
        exit(1);
    }

    if ((sem = sem_open(argv[1], 0)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    if (sem_wait(sem) < 0) {
        perror("sem_wait error");
        exit(1);
    }

    if (sem_getvalue(sem, &val) < 0) {
        perror("sem_getvalue");
        exit(1);
    }

    printf("pid %ld has semaphore, value = %d\n", (long)getpid(), val);

    pause();
    
    return 0;    
}