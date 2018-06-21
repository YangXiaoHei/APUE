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
    
    unsigned int *ptr1;
    unsigned int *ptr2;

    pid_t pid;

    shm_unlink(argv[1]);

    int fd1 = open(argv[0], O_RDWR);
    if (fd1 < 0) {
        perror("open error");
        exit(1);
    }
    struct stat st;
    if (fstat(fd1, &st) < 0) {
        perror("fstat error");
        exit(1);
    }

    int fd = shm_open(argv[1], O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }
    if (ftruncate(fd, sizeof(int)) < 0) {
        perror("ftruncate error");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        ptr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        ptr2 = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
        if (ptr1 == MAP_FAILED || ptr2 == MAP_FAILED) {
            perror("mmap error");
            exit(1);
        }
        printf("child ptr1 = %ld\n", (long)ptr1);
        sleep(5);
        printf("child *ptr1 = %u\n", *ptr1);
        exit(0);
    }

    ptr2 = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    ptr1 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr1 == MAP_FAILED || ptr2 == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    printf("parent ptr1 = %ld\n", (long)ptr1);
    *ptr1 = 123456;

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }
    
    return 0;    
}