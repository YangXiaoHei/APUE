#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define EXEC_MORE "/usr/bin/more"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage : %s [input_file]\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("fopen error");
        exit(1);
    }
    
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) {
        close(fds[0]);
        char buf[1024] = { 0 };
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            size_t len = strlen(buf);
            if (write(fds[1], buf, len) != len) {
                perror("write error");
                exit(1);
            }
        }
        if (ferror(fp)) {
            printf("fgets error\n");
            exit(1);
        }
        close(fds[1]);
        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid error");
            exit(1);
        }
    } else {
        close(fds[1]);
        if (dup2(fds[0], STDIN_FILENO) < 0) {
            perror("dup2 error");
            exit(1);
        }
        close(fds[0]);
        if (execl(EXEC_MORE, "a", (char *)0) < 0) {
            perror("execl error");
            exit(1);
        }
    }
    
    return 0;    
}