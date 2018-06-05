#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

typedef void (*sig_handler)(int);
sig_handler yh_signal(int signo, sig_handler handler, int block_signo_num, ...) {
    struct sigaction new_act, old_act;
    sigemptyset(&new_act.sa_mask);
    if (block_signo_num != 0) {
        va_list vl;
        va_start(vl, block_signo_num);
        for (int i = 0; i < block_signo_num; i++) {
            int bs = va_arg(vl, int);
            printf("【%d】", bs);
            sigaddset(&new_act.sa_mask, bs);
            fflush(stdout);
        }
        va_end(vl);
    }
    new_act.sa_handler = handler;
    if (signo == SIGALRM) {
        #ifdef SA_INTERRUPT
        new_act.sa_flags = SA_INTERRUPT;
        #endif
    } else {
        new_act.sa_flags = SA_RESTART;
    }
    if (sigaction(signo, &new_act, &old_act) < 0) {
        return SIG_ERR;
    }
    return old_act.sa_handler;
}

int fd;

void sig_usr1(int signo) {
    struct flock write_lock;
    write_lock.l_type = F_WRLCK;
    write_lock.l_whence = SEEK_SET;
    write_lock.l_start = 0;
    write_lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &write_lock) < 0) {
        perror("write_lock fail");
        exit(1);
    }
    printf("write lock aqiure succ\n");
}

void sig_usr2(int signo) {
    struct flock read_lock;
    read_lock.l_type = F_RDLCK;
    read_lock.l_whence = SEEK_SET;
    read_lock.l_start = 0;
    read_lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &read_lock) < 0) {
        perror("write_lock fail");
        exit(1);
    }
    printf("read lock aqiure succ\n");
}

int main(int argc, char *argv[]) {
    
    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    sigset_t new_mask, old_mask, zero_mask;
    sigemptyset(&new_mask);
    sigemptyset(&old_mask);
    sigemptyset(&zero_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }

    if (yh_signal(SIGUSR1, sig_usr1, 1, SIGUSR2) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
    if (yh_signal(SIGUSR2, sig_usr2, 1, SIGUSR1) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }

    while (1) 
        sigsuspend(&zero_mask);
    
    return 0;    
}