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
#include <sys/mman.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("usage : %s <name>\n", argv[0]);
        exit(1);
    }

    if (shm_unlink(argv[1]) < 0) {
        perror("shm_unlink error");
        exit(1);
    }
    
    return 0;    
}