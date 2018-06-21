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

int max(int a, int b) { return a > b ? a : b; }

int main(int argc, char *argv[]) {
    
    int fd, i;
    char *ptr;
    int filesize, mmapsize, pagesize;

    if (argc != 4) {
        printf("usage : %s <pathname><filesize><mmapsize>\n", argv[0]);
        exit(1);
    }

    filesize = atoi(argv[2]);
    mmapsize = atoi(argv[3]);

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    if (lseek(fd, filesize - 1, SEEK_SET) < 0) {
        perror("lseek error");
        exit(1);
    }

    if (write(fd, "", 1) < 0) {
        perror("write error");
        exit(1);
    }

    ptr = mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    pagesize = sysconf(_SC_PAGESIZE);
    printf("pagesize = %d\n", pagesize);

    for (i = 0; i < max(filesize, mmapsize); i += pagesize) {
        printf("ptr[%d] = %d\n",i, ptr[i]);
        ptr[i] = 1;
        printf("1write ok\n");
        printf("ptr[%d] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
        ptr[i + pagesize - 1] = 1;
        printf("2write ok\n");
    }
    printf("ptr[%d] = %d\n", i, ptr[i]);
    
    return 0;    
}