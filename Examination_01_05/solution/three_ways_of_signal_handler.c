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

void sig_alrm(int signo) {
    printf("接收到 SIGALRM 信号，控制流程转交给信号捕获函数\n");
    printf("信号捕获函数执行完毕\n");
}

int main(int argc, char *argv[]) {
    
    printf("为 SIGALRM 信号注册捕获函数\n");
    if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    printf("3 秒后向本进程发送 SIGALRM 信号\n");
    alarm(3);

    for (int i = 3; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }

    printf("控制流程回到 main 函数\n");

    printf("将 SIGALRM 信号设置为忽略\n");
    if (signal(SIGALRM, SIG_IGN) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    printf("3 秒后向本进程发送 SIGALRM 信号\n");
    alarm(3);

    for (int i = 3; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }

    printf("接收到信号，但什么都没有发生\n");
    printf("将 SIGALRM 设置为默认动作\n");

    if (signal(SIGALRM, SIG_DFL) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    printf("3 秒后向本进程发送 SIGALRM 信号\n");
    alarm(3);

    for (int i = 3; i > 0; i--) {
        printf("%d\n", i);
        sleep(1);
    }
    
    return 0;    
}