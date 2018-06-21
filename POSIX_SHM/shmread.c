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

    int fd = shm_open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }

    unsigned char *ptr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    int c;
    for (int i = 0; i < st.st_size; i++) {
        printf("ptr[%d] = %d\n", i, ptr[i]);
        // if ((c = *ptr++) != (i % 256)) {
        //     printf("error exit : ptr[%d] = %d\n", i, c);
        //     exit(1);
        // }
    }
    
    return 0;    
}