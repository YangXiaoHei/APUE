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

    if (argc != 4) {
        printf("usage : %s <shmname><semname><nloop>\n", argv[0]);
        exit(1);
    }
    
    sem_t *mutex = sem_open(argv[2], O_RDWR);
    if (mutex == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    int fd = shm_open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }

    int nloop = atoi(argv[3]);

    unsigned int *ptr = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    for (int i = 0; i < nloop; i++) {
        sem_wait(mutex);
        printf("pid = %d count = %d\n", getpid(), ++(*ptr));
        sem_post(mutex);
    }

    
    return 0;    
}