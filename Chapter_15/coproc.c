#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int cfds_in[2], cfds_out[2];
    if (pipe(cfds_out) < 0 || pipe(cfds_in) < 0) {
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) {
        close(cfds_in[0]);
        close(cfds_out[1]);
        
        char buf[1024] = { 0 };
        while (1) {
            // printf("input two num > ");
            fputs("input two num >", stdout);
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf("fgets error");
                exit(1);
            }
            ssize_t len = strlen(buf);
            if (write(cfds_in[1], buf, len) != len) {
                perror("write error");
                exit(1);
            }
            if ((len = read(cfds_out[0], buf, sizeof(buf))) < 0) {
                perror("read error");
                exit(1);
            }
            buf[len] = 0;
            char buf2[1024] = { 0 };
            strncpy(buf2, "result is :", 11);
            strncat(buf2, buf, strlen(buf));
            if (fputs(buf2, stdout) == EOF) {
                if (ferror(stdout)) {
                    perror("fputs error");
                    exit(1);
                }
            }
        }
    } else {
        close(cfds_in[1]);
        close(cfds_out[0]);

        if (dup2(cfds_in[0], STDIN_FILENO) != STDIN_FILENO) {
            perror("dup2 error");
            exit(1);
        }
        if (dup2(cfds_out[1], STDOUT_FILENO) != STDOUT_FILENO) {
            perror("dup2 error");
            exit(1);
        }
        close(cfds_in[0]);
        close(cfds_out[1]);

        if (execl("./add2", "add2", (char *)0) < 0) {
            perror("execl error");
            _exit(127);
        }
    }
    return 0;    
}