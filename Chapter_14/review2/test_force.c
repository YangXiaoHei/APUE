#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#define PATH "./yanghan_test"

int main(int argc, char *argv[]) {
    
    int fd = open(PATH, O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }
    if (fchmod(fd, (st.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        perror("fchmod error");
        exit(1);
    }

    if (write(fd, "123456789", 9) != 9) {
        perror("write error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(3);
        char buf[4];
        if (read(fd, buf, sizeof(buf)) < 0) {
            printf("force lock!\n");
        } else {
            printf("no force lock\n");
        }
        exit(0);
    }

    struct flock wr_lock;
    wr_lock.l_type = F_WRLCK;
    wr_lock.l_whence = SEEK_SET;
    wr_lock.l_start = 0;
    wr_lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &wr_lock) < 0) {
        perror("lock file error");
        exit(1);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }
    
    return 0;    
}