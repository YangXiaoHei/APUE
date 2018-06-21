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
    
    int fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int num = 0x123456FF;
    if (write(fd, &num, 4) != 4) {
        perror("write error");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);
    for (int i = 0; i < 4; i++) {
        int n;
        if (read(fd, &n, 1) < 0) {
            perror("read error");
            exit(1);
        }
        printf("%x ", n);
    }
    printf("\n------------\n");

    char *ptr = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    char *vptr = ptr;

    for (int i = 0; i < 4; i++) {
        printf("%x ", *vptr++);
    }
    printf("\n------------\n");

    (*(int *)ptr)++;
    // (*ptr)++;

    vptr = ptr;

    for (int i = 0; i < 4; i++) {
        printf("%x ", *vptr++);
    }
    
    return 0;    
}