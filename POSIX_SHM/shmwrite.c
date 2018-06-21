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

    int fd = shm_open(argv[1], O_RDWR, 0644);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat error");
        exit(1);       
    }
    char *ptr = mmap(NULL, st.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }

    close(fd);

    for (int i = 0; i < st.st_size; i++) 
        *ptr++ = i % 256;
    
    return 0;    
}