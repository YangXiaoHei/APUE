#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

void sig_usr1(int signo) {}

int main(int argc, char *argv[]) {

    sigset_t new_mask, zero_mask;
    sigemptyset(&zero_mask);
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) {
        perror("sigproc mask");
        exit(1);
    }

    struct sigaction new_act;
    new_act.sa_handler = sig_usr1;
    new_act.sa_mask = 0;
    new_act.sa_flags = 0;
    if (sigaction(SIGUSR1, &new_act, NULL) < 0) {
        perror("sigaction error");
        exit(1);
    }

    if (argc != 2) {
        printf("you must input a file name\n");
        exit(1);
    }
    
    int fd = creat(argv[1], 0644);
    if (fd < 0) {
        perror("create fail\n");
        exit(1);
    }

    if (write(fd, "abcdefg", 7) != 7) {
        perror("write error");
        exit(1);
    }

    struct flock write_lock;
    write_lock.l_type = F_WRLCK;
    write_lock.l_whence = SEEK_SET;
    write_lock.l_start = 0;
    write_lock.l_len = 1;

    if (fcntl(fd, F_SETLK, &write_lock) < 0) {
        perror("%s hold write lock fail\n");
        exit(1);
    }
    printf("%s write lock succ\n", argv[1]);

    printf("proc %ld begin to suspend\n", (long)getpid());
    sigsuspend(&zero_mask);

    if (close(fd) < 0) {
        perror("close fd fail\b");
        exit(1);
    }

    printf("close fd succ\n");

    while (1) sleep(1);

    return 0;    
}