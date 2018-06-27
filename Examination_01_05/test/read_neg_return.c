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


void case1(void) 
{
    unlink("test_file");
    int fd = creat("test_file", 0644);
    if (fd < 0) {
        perror("creat error");
        exit(1);
    }
    int rd_len = 0;
    char buf[1024] = { 0 };
    if ((rd_len = read(fd, buf, sizeof(buf))) < 0) 
        printf("return value = %d reason : %s\n", rd_len, strerror(errno));
    else if (rd_len != sizeof(buf))
        printf("return value = %d\n", rd_len);
    else
        printf("read succ\n");
}


void sig_alrm(int signo) { }
void case2(void)
{
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = sig_alrm;
    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(1);
    }

    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    alarm(1);

    int rd_len = 0;
    char buf[1024];
    if ((rd_len = read(fds[0], buf, sizeof(buf))) < 0) 
        printf("return value = %d reason : %s\n", rd_len, strerror(errno));
    else if (rd_len != sizeof(buf))
        printf("return value = %d\n", rd_len);
    else
        printf("read succ\n");

    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
}

void case3(void)
{
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    int flags = fcntl(fds[0], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fds[0], F_SETFL, flags);

    int rd_len = 0;
    char buf[1024];
    if ((rd_len = read(fds[0], buf, sizeof(buf))) < 0) 
        printf("return value = %d reason : %s\n", rd_len, strerror(errno));
    else if (rd_len != sizeof(buf))
        printf("return value = %d\n", rd_len);
    else
        printf("read succ\n");

    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    
    case1();
    case2();
    case3();

    return 0;    
}