#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        int i = 0;
        while (1) {
            char buf[100] = { 0 };
            snprintf(buf, sizeof(buf), "hello world --- %d", i++);
            if (write(fds[1], buf, strlen(buf)) != strlen(buf)) {
                perror("write fail");
                exit(1);
            }
            printf("------write succ : %s\n", buf);
            fflush(stdout);
            bzero(buf, sizeof(buf));
            sleep(1);
        }
    } else {
        while (1) {
            char buf[100] = { 0 };
            read(fds[0], buf, sizeof(buf));
            printf("read content %s\n", buf);
            bzero(buf, sizeof(buf));
        }
    }
    
    return 0;    
}