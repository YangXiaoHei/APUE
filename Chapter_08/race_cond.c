#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int pfd1[2], pfd2[2];

#define W_END 1
#define R_END 0

void TELL_WAIT() {
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0) {
        perror("pipe");
        exit(1);
    }
}

/* 在子进程中 */
void TELL_PARENT(pid_t pid) {
    if (write(pfd2[W_END], "c", 1) != 1) {
        perror("tell parent fail");
        exit(1);
    }
}

/* 在父进程中 */
void WAIT_CHILD() {
    char c = 0;
    if (read(pfd2[R_END], &c, 1) < 0) {
        perror("wait child fail\n");
        exit(1);
    }
    if (c != 'c') {
        printf("wrong data from child\n");
        exit(1);
    }
}

/* 在父进程中 */
void TELL_CHILD(pid_t pid) {
    if (write(pfd1[W_END], "p", 1) != 1) {
        perror("tell child fail");
        exit(1);
    }
}

/* 在子进程中 */
void WAIT_PARENT() {
    char c = 0;
    if (read(pfd1[R_END], &c, 1) < 0) {
        perror("wait parent fail\n");
        exit(1);
    }
    if (c != 'p') {
        printf("wrong data from parent\n");
        exit(1);
    }
}

void yh_print(char *str) {
    
    char *p;
    int c;
    
    setbuf(stdout, NULL);
    
    for (p = str; (c = *p++) != 0; ) {
        putchar(c);
    }
    putchar('\n');
}

int main() {
    
    char o[1000], a[1000];
    for (int i = 0; i < sizeof(a); i++) {
        o[i] = 'O';
    }
    o[sizeof(o) - 1] = 0;
    
    for (int i = 0; i < sizeof(a); i++) {
        a[i] = 'A';
    }
    a[sizeof(a) - 1] = 0;
    
    TELL_WAIT();
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        yh_print(o);
        TELL_PARENT(pid);
        WAIT_PARENT();
    } else {
        WAIT_CHILD();
        yh_print(a);
        TELL_CHILD(pid);
    }
    
    return 0;
}
