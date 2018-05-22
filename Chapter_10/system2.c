#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int yh_system(const char *cmd) {
    
    if (cmd == NULL)
        return 1;
    
    pid_t pid;
    int status;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        execl("/bin/bash", "bash", "-c", cmd, (char *)0);
        _exit(127);
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    }
    return status;
}

void sig_int(int signo) {
    printf("sig_int signo = %d\n", signo);
}

void sig_chld(int signo) {
    printf("sig_chld signo = %d\n", signo);
}



int main() {
    
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    if (signal(SIGCHLD, sig_chld) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    if (yh_system("/bin/ed") < 0) {
        printf("system error\n");
        exit(1);
    }
    
    return 0;
}
