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
    
    if (argc != 2) {
        printf("usage : sem_unlink <name>\n");
        exit(1);
    }

    if (sem_unlink(argv[1]) < 0) {
        perror("sem_unlink error");
        exit(1);
    }
    
    return 0;    
}