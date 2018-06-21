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
        printf("usage : %s <file><nloop>\n", argv[0]);
        exit(1);
    }
    
    sem_unlink("mutex");
    sem_t *mutex = sem_open("mutex", O_CREAT | O_EXCL, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    int fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int zero = 0;
    if (write(fd, &zero, sizeof(zero)) != sizeof(zero)) {
        perror("write error");
        exit(1);
    }
    
    // if (write(fd, "0", 1) != 1) {
    //     perror("write error");
    //     exit(1);
    // }

    int nloop = atoi(argv[2]);

    int *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        for (int i = 0; i < nloop; i++) {
            sem_wait(mutex);
            printf("child %d\n", ++(*ptr));
            sem_post(mutex);
        }
        exit(1);
    }

    for (int i = 0; i < nloop; i++) {
        sem_wait(mutex);
        printf("parent %d\n", ++(*ptr));
        sem_post(mutex);
    }

    
    return 0;    
}