#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int pipe_1[2], pipe_2[2];

#define W_END 1
#define R_END 0

void TELL_WAIT() {
    if (pipe(pipe_1) < 0 || pipe(pipe_2) < 0) {
        perror("create pipe fail\n");
        exit(1);
    }
}

void WAIT_PARENT() {
    char c;
    if (read(pipe_1[R_END], &c, 1) < 0) {
        perror("read");
        exit(1);
    }
    if (c != 'p') {
        printf("parent noti error\n");
    }
}

void TELL_CHILD() {
    if (write(pipe_1[W_END], "p", 1) != 1) {
        perror("write");
        exit(1);
    }
}

void WAIT_CHILD() {
    char c;
    if (read(pipe_2[R_END], &c, 1) < 0) {
        perror("read");
        exit(1);
    }
    if (c != 'c') {
        printf("child noti error\n");
    }
}

void TELL_PARENT() {
    if (write(pipe_2[W_END], "c", 1) != 1) {
        perror("write");
        exit(1);
    }
}


int main() {
    
    pid_t pid;
    
    TELL_WAIT();
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        WAIT_PARENT();
        for (int i = 0; i < 500; i++) {
            write(STDOUT_FILENO, "A", 1);
        }
        TELL_PARENT();
    } else {
        for (int i = 0; i < 500; i++) {
            write(STDOUT_FILENO, "O", 1);
        }
        TELL_CHILD();
        WAIT_CHILD();
        
        int status;
        if (wait(&status) < 0) {
            perror("wait");
            exit(1);
        }
    }
    
    return 0;
}
