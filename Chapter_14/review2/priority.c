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

void read_lock(int fd) {
    struct flock read_lock;
    read_lock.l_type = F_RDLCK;
    read_lock.l_whence = SEEK_SET;
    read_lock.l_start = 0;
    read_lock.l_len = 0;
    if (fcntl(fd, F_SETLKW, &read_lock) < 0) {
        perror("fcntl error");
        exit(1);
    }
}

void write_lock(int fd) {
    struct flock write_lock;
    write_lock.l_type = F_WRLCK;
    write_lock.l_whence = SEEK_SET;
    write_lock.l_start = 0;
    write_lock.l_len = 0;
    if (fcntl(fd, F_SETLKW, &write_lock) < 0) {
        perror("fcntl error");
        exit(1);
    }
}

void unlock(int fd) {
    struct flock unlock;
    unlock.l_type = F_UNLCK;
    unlock.l_whence = SEEK_SET;
    unlock.l_start = 0;
    unlock.l_len = 0;
    if (fcntl(fd, F_SETLK, &unlock) < 0) {
        perror("fcntl error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    
    int fd;
    if ((fd = open("./yanghan", O_RDWR)) < 0) {
        perror("open error");
        exit(1);
    }

    write_lock(fd);
    printf("parent hold a write lock\n");

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(1);
        printf("first child begin to obtain write lock\n");
        write_lock(fd);
        printf("first child hold write lock\n");
        sleep(2);
        unlock(fd);
        printf("first child release write lock\n");
        exit(0);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(3);
        printf("second child begin to obtain read lock\n");
        read_lock(fd);
        printf("second child hold read lock\n");
        sleep(4);
        unlock(fd);
        printf("second child release read lock\n");
        exit(0);
    }

    sleep(5);
    unlock(fd);
    printf("parent release write lock\n");

    int status;
    if (wait(&status) < 0) {
        perror("wait error");
        exit(1);
    }

    if (wait(&status) < 0) {
        perror("wait error");
        exit(1);
    }
    
    return 0;    
}