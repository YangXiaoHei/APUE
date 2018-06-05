#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void pr_status(int status) {
    if (WIFEXITED(status)) {
        printf("normal exit with code : %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("signal termination with signo : %d\n", WTERMSIG(status));
    } 
}

void sig_pipe(int signo) {
    printf("sig_pipe\n");
}

int main(int argc, char *argv[]) {
    
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    if (signal(SIGPIPE, sig_pipe) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        close(fds[0]);
        sleep(1);
        while (1) {
            if (write(fds[1], "a", 1) < 0) {
                perror("write error");
                fflush(stdout);
                sleep(3);
            }
            printf("write succ\n");
        }
    } 
    close(fds[0]);
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }

    pr_status(status);
    return 0;    
}