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
    
    unlink("test_1");
    int fd = open("test_1", O_CREAT | O_RDWR, 0644);
    write(fd, "123456789", 9);

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }
    printf("test_1 文件大小为 %ld\n", (long)st.st_size);

    close(fd);

    fd = creat("test_1", 0644);
    if (fstat(fd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }
    printf("test_1 文件大小为 %ld\n", (long)st.st_size);
    
    return 0;    
}