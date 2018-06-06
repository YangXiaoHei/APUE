#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_CREAT | O_TRUNC | O_APPEND | O_RDWR, 0644);
    if (fd < 0) {
        perror("creat error");
        exit(1);
    }
    size_t sz = (10 << 10) + 1;
    char *buf = malloc(sz);
    if (buf == NULL) {
        printf("malloc error\n");
        exit(1);
    }
    memset(buf, 'A', sz);
    buf[sz - 1] = 0;
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        for (int i = 0; i < sz; i++) {
            if (write(fd, buf + i, 1) != 1) {
                perror("write error");
                exit(1);
            }
        }
        printf("child proc succ\n");
        exit(1);
    } 

    for (int i = 0; i < sz; i++) {
        if (write(fd, buf + i, 1) != 1) {
            perror("write error");
            exit(1);
        }
    }
    printf("parent proc succ\n");

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }

    printf("total size = %ld\n",(long)st.st_size);
    
    return 0;    
}