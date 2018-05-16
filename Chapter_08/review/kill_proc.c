#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void pr_status(int status) {
    
    if (WIFEXITED(status)) {
        printf("正常死的, exit code = %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("被信号干死, signo = %d\n", WTERMSIG(status));
    }
}


int main(int argc, char *argv[]) {
    
    pid_t pid;
    int status;
    
    // 1
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        sleep(1);
        exit(3);
    }
    
    if (wait(&status) < 0) {
        perror("wait");
        exit(1);
    }
    
    pr_status(status);
    
    // 2
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        sleep(2);
        abort();
    }
    
    if (wait(&status) < 0) {
        perror("wait");
        exit(1);
    }
    
    pr_status(status);
    
    // 3
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        sleep(3);
        status /= 0;
    }
    
    if (wait(&status) < 0) {
        perror("wait");
        exit(1);
    }
    pr_status(status);
    
    // 4
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        sleep(4);
        kill(getpid(), SIGPIPE);
    }
    
    if (wait(&status) < 0) {
        perror("wait");
        exit(1);
    }
    pr_status(status);
    
    
    
    
    return 0;
}
