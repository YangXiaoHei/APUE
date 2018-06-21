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

int resize_file_and_clean(const char *pathname, int size) {
    int fd = open(pathname, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    if (lseek(fd, size - 1, SEEK_SET) < 0) {
        perror("lseek error");
        exit(1);
    }
    if (write(fd, "", 1) != 1) {
        perror("write error");
        exit(1);
    }
    return fd;
}

void sig_segv(int signo) {
    printf("receive SIGSEGV\n");
}

void sig_bus(int signo) {
    printf("receive SIGBUS\n");
}

int main(int argc, char *argv[]) {

    if (signal(SIGBUS, sig_bus) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (signal(SIGSEGV, sig_segv) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int filesize;
    int mmapsize;

    char *ptr = NULL;

    setbuf(stdout, NULL);
    while (1) {
        if (ptr != NULL) {
            if (munmap(ptr, mmapsize) < 0) {
                perror("munmap error");
                exit(1);
            }
            printf("munmap succ\n");
        }
        printf("please input filesize mmapsize > ");
        if (scanf("%d%d", &filesize, &mmapsize) != 2 ||
            filesize <= 0 ||
            mmapsize <= 0 ) {
            printf("⚠️ input invalid, please reinput\n");
            continue;
        }
        close(fd);
        resize_file_and_clean(argv[1], filesize);
        if ((ptr = mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
            perror("mmap error");
            exit(1);
        }
        printf("mmap succ\n");
        while (1) {
            printf("access bytes in > ");
            int byte;
            if (scanf("%d", &byte) != 1 ||
                (byte <= 0 && byte != -1)) {
                printf("⚠️ input invalid, please reinput\n");
                continue;
            } else if (byte == -1)
                break;
            ptr[byte] = 1;
            printf("ptr[%d] = %d\n", byte, ptr[byte]);
        }
    }
    
    return 0;    
}