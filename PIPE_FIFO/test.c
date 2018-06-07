#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


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
        sleep(3);
        int c;
        if (read(fds[0], &c, 1) < 0) {
            perror("child read error");
            exit(1);
        }
        printf("child read %c from pipe 0\n", c);
        if (write(fds[0], &c, 1) != 1) {
            perror("child write error");
            exit(1);
        }
        exit(0);
    }

    int c = 'p';
    if (write(fds[1], &c, 1) != 1) {
        perror("parent write error");
        exit(1);
    }
    if (read(fds[1], &c, 1) < 0) {
        perror("parent read error");
        exit(1);
    }

    return 0;    
}