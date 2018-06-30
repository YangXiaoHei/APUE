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

void sig_stop(int signo) {
    printf("执行 SIGSTOP 捕获函数\n");
}

void sig_kill(int signo) {
    printf("执行 SIGKILL 捕获函数\n");
}

int main(int argc, char *argv[]) {
    
    int state = 0;

    if (signal(SIGSTOP, sig_stop) == SIG_ERR) {
        perror("为 SIGSTOP 注册捕获函数失败");
        state++;
    }
    
    if (signal(SIGKILL, sig_kill) == SIG_ERR) {
        perror("为 SIGKILL 注册捕获函数失败");
        state++;
    }

    if (signal(SIGSTOP, SIG_IGN) == SIG_ERR) {
        perror("忽略 SIGSTOP 失败");
        state++;
    }
    
    if (signal(SIGKILL, SIG_IGN) == SIG_ERR) {
        perror("忽略 SIGKILL 失败");
        state++;
    }
    
    return 0;    
}