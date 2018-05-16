#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

void pr_status(int status) {
    if (WIFEXITED(status)) {
        printf("正常死掉, exit code = %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("被信号干死 signo = %d\n", WTERMSIG(status));
    }
}

int main(int argc, char *argv[]) {
    
    pid_t pid;
    int status;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if (pid > 0) {
            exit(0);
        }
        
        sleep(2);
        printf("my father is %d\n", getppid());
        exit(1);
    }
    
    if (wait(&status) < 0) {
        perror("wati");
        exit(1);
    }
    
    return 0;
}
