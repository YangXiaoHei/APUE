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
 * 向一个读端关闭的管道中写数据，但是 SIGPIPE 信号被捕获
 */
void sig_pipe(int signo) {}
void case1(void)
{
    if (signal(SIGPIPE, sig_pipe) == SIG_ERR) {
        printf("signal error");
        exit(1);
    }
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }
    close(fds[0]);
    int wr_len = 0;
    if ((wr_len = write(fds[1], "1234", 4)) < 0) 
        printf("return value = %d, reason : %s\n", wr_len, strerror(errno));
    else if (wr_len != 4)
        printf("return value = %d\n", wr_len);
    else
        printf("write succ\n");

    if (signal(SIGPIPE, SIG_DFL) == SIG_ERR) {
        printf("signal error");
        exit(1);
    }
}

/*
 * 管道被写满，write 返回此时写入管道的总字节数
 * 继续向已写满管道写数据，返回 -1
 */
void case2(void)
{
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    int total_bytes = 10 << 20;

    // 10M
    char *buf = malloc(total_bytes);
    memset(buf, 'a', total_bytes);
    if (buf == NULL) {
        printf("malloc fail\n");
        exit(1);
    }
    int flags = fcntl(fds[1], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fds[1], F_SETFL, flags);

    int wr_len = 0;
    if ((wr_len = write(fds[1], buf, total_bytes)) < 0) 
        printf("return value = %d, reason : %s\n", wr_len, strerror(errno));
    else if (wr_len != total_bytes)
        printf("return value = %d, not equal bytes to write %d\n", wr_len, total_bytes);
    else
        printf("write succ\n");

    if ((wr_len = write(fds[1], buf + wr_len, total_bytes - wr_len)) < 0) 
        printf("return value = %d, reason : %s\n", wr_len, strerror(errno));
    else if (wr_len != (total_bytes - wr_len))
        printf("return value = %d\n", wr_len);
    else
        printf("write succ\n");
}

/*
 * 向一个没有以写权限打开的文件写数据
 */
void case3(void)
{
    unlink("test_file");
    int fd = creat("test_file", 0600);
    if (fd < 0) {
        perror("creat error");
        exit(1);
    }
    close(fd);
    fd = open("test_file", O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    int wr_len = 0;
    if ((wr_len = write(fd, "1234", 4)) < 0) 
        printf("return value = %d, reason : %s\n", wr_len, strerror(errno));
    else if (wr_len != 4)
        printf("return value = %d\n", wr_len);
    else
        printf("write succ\n");
}

int main(int argc, char *argv[]) {
    
    case1();
    case2();
    case3();
    
    return 0;    
}