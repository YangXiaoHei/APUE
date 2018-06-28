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

void printf_buf(unsigned char *buf, int size, const char *msg) {
    printf("%s : ", msg);
    for (int i = 0; i < size; i++) 
        printf("%c", buf[i] == 0 ? '@' : buf[i] == '\n' ? '~' : buf[i]);
    printf("\n");
}


int main(int argc, char *argv[]) {
    
    int fd = open("test_file", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error\n");
        exit(1);
    }
    close(fd);

    FILE *fp = fopen("test_file", "r");
    if (fp == NULL) {
        printf("fopen error\n");
        exit(1);
    }

    char buf[4] = { 0 };
    char *ptr = fgets(buf, sizeof(buf), fp);
    if (ptr == NULL) {
        printf("return value == NULL\n");
        if (ferror(fp)) {
            printf("fgets 出错\n");
            exit(1);
        }
        if (feof(fp)) {
            printf("fgets 到头了\n");
            exit(1);
        }
    }

    
    return 0;    
}