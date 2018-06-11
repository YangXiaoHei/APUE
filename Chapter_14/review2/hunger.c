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

int main(int argc, char *argv[]) {

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    struct flock read_lock;
    read_lock.l_type = F_RDLCK;
    read_lock.l_whence = SEEK_SET;
    read_lock.l_start = 0;
    read_lock.l_len  =0;
    if (fcntl(fd, F_SETLK, &read_lock) < 0) {
        perror("fcntl error");
        exit(1);
    }
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(1);
        printf("first child begin to obtain write lock\n");
        struct flock write_lock;
        write_lock.l_type = F_WRLCK;
        write_lock.l_whence = SEEK_SET;
        write_lock.l_start = 0;
        write_lock.l_len = 0;
        if (fcntl(fd, F_SETLKW, &write_lock) < 0) {
            perror("fcntl error");
            exit(1);
        }
        printf("first child hold a write lock\n");
        exit(0);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(3);
        printf("second child begin to obtain read lock\n");
        struct flock read_lock;
        read_lock.l_type = F_RDLCK;
        read_lock.l_whence = SEEK_SET;
        read_lock.l_start = 0;
        read_lock.l_len  =0;
        if (fcntl(fd, F_SETLKW, &read_lock) < 0) {
            perror("fcntl error");
            exit(1);
        }
        printf("second child hold a read lock\n");
        exit(0);
    }

    sleep(5);
    printf("parent begin to release read lock\n");
    struct flock unlock;
    unlock.l_type = F_UNLCK;
    unlock.l_whence = SEEK_SET;
    unlock.l_start = 0;
    unlock.l_len  =0;
    if (fcntl(fd, F_SETLK, &unlock) < 0) {
        perror("fcntl error");
        exit(1);
    }
    printf("parent release read lock\n");

    int status; 
    if (wait(&status) < 0) {
        perror("wait error");
        exit(1);
    }

    
    return 0;    
}