#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void sig_hup(int signo) {
    printf("子进程收到 %d 信号\n", signo);
}

void sig_con(int signo) {
    printf("子进程收到 %d 信号\n", signo);
}


int main(int argc, char *argv[]) {
    
    pid_t pid;
    char c;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        sleep(5);
    } else {
        signal(SIGHUP, sig_hup);
        signal(SIGCONT, sig_con);
        kill(getpid(), SIGTSTP);
        printf("子进程继续\n");
        if (read(STDIN_FILENO, &c, 1) != 1) {
            printf("read error %d on controlling TTY\n", errno);
        }
    }
    
    return 0;
}
