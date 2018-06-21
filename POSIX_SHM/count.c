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

    if (argc != 3) {
        printf("usage : %s <shmname> <semname>\n", argv[0]);
        exit(1);
    }
    
    shm_unlink(argv[1]);
    int fd = shm_open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }

    if (ftruncate(fd, sizeof(int)) < 0) {
        perror("ftruncate error");
        exit(1);
    }

    sem_unlink(argv[2]);
    sem_t *mutex = sem_open(argv[2], O_CREAT | O_EXCL, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    unsigned char *ptr = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    return 0;    
}