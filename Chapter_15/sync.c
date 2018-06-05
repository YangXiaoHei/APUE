#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int pfds1[2], pfds2[2];

void TELL_WAIT(void) {
    if (pipe(pfds2) < 0 || pipe(pfds1) < 0) {
        perror("pipe error");
        exit(1);
    }
}

void WAIT_PARENT(void) {
    int c = 0;
    if (read(pfds1[0], &c, 1) < 0) {
        perror("read error");
        exit(1);
    }
    if (c != 'c') {
        printf("notify error\n");
        exit(1);
    }
}

void TELL_CHILD(pid_t pid) {
    if (write(pfds1[1], "c", 1) != 1) {
        perror("write error");
        exit(1);
    }
}

void WAIT_CHILD(void) {
    int c = 0;
    if (read(pfds2[0], &c, 1) < 0) {
        perror("read error");
        exit(1);
    }
    if (c != 'p') {
        printf("notify error\n");
        exit(1);
    }
}

void TELL_PARENT(pid_t pid) {
    if (write(pfds2[1], "p", 1) != 1) {
        perror("write error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    
    TELL_WAIT();

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) {
        char buf[1024];
        memset(buf, 'A', sizeof(buf));
        for (int i = 0; i < sizeof(buf); i++) {
            if (write(STDOUT_FILENO, buf + i, 1) != 1) {
                perror("write error");
                exit(1);
            }
        }
        TELL_CHILD(pid);
        WAIT_CHILD();
    } else {
        WAIT_PARENT();
        char buf[1024];
        memset(buf, 'O', sizeof(buf));
        for (int i = 0; i < sizeof(buf); i++) {
            if (write(STDOUT_FILENO, buf + i, 1) != 1) {
                perror("write error");
                exit(1);
            }
        }
        TELL_PARENT(getppid());
    }
    
    return 0;    
}