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

void first(const char *pathname) {
    struct stat st;
    if (lstat(pathname, &st) < 0) {
        perror("lstat error");
        exit(1);
    }
    printf("第一种方式：文件大小为 : %ld\n", (long)st.st_size);
}

void second(const char *pathname) {
    int fd = open(pathname, O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    int file_size = lseek(fd, 0, SEEK_END);
    printf("第二种方式：文件大小为 : %d\n", file_size);
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage : %s <pathname>\n", argv[0]);
        exit(1);
    }
    
    first(argv[1]);
    second(argv[1]);
    
    return 0;    
}