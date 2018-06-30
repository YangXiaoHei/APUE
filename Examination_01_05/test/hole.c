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
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("usage : %s <size>\n", argv[0]);
        exit(1);
    }

    char c[] = "123456789";

    int fd = open("hole_file", O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int fsize = atoi(argv[1]);
    // fsize <<= 20;

    char *buffer = calloc(1, fsize);
    if (buffer == NULL) {
        printf("malloc error");
        exit(1);
    }

    int j = 0;
    int len = strlen(c);
    for (int i = 0; i < fsize; i += 3) 
        buffer[i] = c[j++ % len];

    if (write(fd, buffer, fsize) != fsize) {
        perror("write error");
        exit(1);
    }
    free(buffer);
    printf("create hole succ\n");
    
    return 0;    
}