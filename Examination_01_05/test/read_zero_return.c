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

/*
 * 文件读到末尾
 */
void case1(void)
{
    unlink("test_file");
    int fd = open("test_file", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("creat error");
        exit(1);
    }

    write(fd, "123456", 6);
    lseek(fd, 0, SEEK_SET);
    char buf[6] = { 0 };
    read(fd, buf, sizeof(buf));

    int rd_len = 0;
    if ((rd_len = read(fd, buf, sizeof(buf))) < 0)
        printf("return value = %d reason : %s\n", rd_len, strerror(errno));
    else if (rd_len == 0)
        printf("return value = %d\n", rd_len);
    else if (rd_len != sizeof(buf))
        printf("return value = %d\n", rd_len);
    else
        printf("read succ\n");
}

/*
 * 读一个写端关闭的管道
 */
void case2(void)
{
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    close(fds[1]);

    int rd_len = 0;
    char buf[1024] = { 0 };
    if ((rd_len = read(fds[0], buf, sizeof(buf))) < 0)
        printf("return value = %d reason : %s\n", rd_len, strerror(errno));
    else if (rd_len == 0)
        printf("return value = %d\n", rd_len);
    else if (rd_len != sizeof(buf))
        printf("return value = %d\n", rd_len);
    else
        printf("read succ\n");
}

int main(int argc, char *argv[]) {
    
    case1();
    case2();
    
    return 0;    
}