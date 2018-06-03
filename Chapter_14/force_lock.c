#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

volatile int sig_flag;
sigset_t new_mask, old_mask, zero_mask;

typedef void (*sig_handler)(int);
sig_handler yh_signal(int signo, sig_handler handler) {
    struct sigaction new_act, old_act;
    new_act.sa_handler = handler;
    new_act.sa_mask = 0;
    if (signo == SIGALRM) {
        #ifdef SA_INTERRUPT
        new_act.sa_flags |= SA_INTERRUPT;
        #endif
    } else {
        new_act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &new_act, &old_act) < 0) {
        return SIG_ERR;
    }
    return old_act.sa_handler;
}

void sig_usr(int signo) {
    sig_flag = 1;
}

void PREPARE(void) {
    sigemptyset(&zero_mask);
    sigemptyset(&new_mask);
    sigemptyset(&old_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    if (yh_signal(SIGUSR1, sig_usr) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
    if (yh_signal(SIGUSR2, sig_usr) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
}

void WAIT_PARENT() {
    while (sig_flag == 0) 
        sigsuspend(&zero_mask);
    sig_flag = 0;

    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1);
}

void WAIT_CHILD() {
    while (sig_flag == 0) 
        sigsuspend(&zero_mask);
    sig_flag = 0;

    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2);
}

int main(int argc, char *argv[]) {

    int fd;
    // if ((fd = creat(argv[1], 0644)) < 0) {
    //     perror("creat error");
    //     exit(1);
    // }
    if ((fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0644)) < 0) {
        perror("open error");
        exit(1);
    }
    if (write(fd, "abcdef", 6) != 6) {
        perror("write error");
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

    PREPARE();
    
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid > 0) {
        struct flock write_lock;
        write_lock.l_type = F_WRLCK;
        write_lock.l_whence = SEEK_SET;
        write_lock.l_start = 0;
        write_lock.l_len = 0;  // lock to EOF
        if (fcntl(fd, F_SETLK, &write_lock) < 0) {
            perror("fcntl error");
            exit(1);
        }
        TELL_CHILD(pid);
        WAIT_CHILD();
    } else {
        WAIT_PARENT();

        int flag = fcntl(fd, F_GETFL);
        flag |= O_NONBLOCK;
        fcntl(fd, F_SETFL, flag);

        struct flock read_lock;
        read_lock.l_type = F_RDLCK;
        read_lock.l_whence = SEEK_SET;
        read_lock.l_start = 0;
        read_lock.l_len = 0;
        if (fcntl(fd, F_SETLK, &read_lock) == 0) {
            perror("read lock reqiure succ!\n");
            exit(1);
        }
        printf("can't reqiure a read lock : %s  errno = %d\n", strerror(errno), errno);

        if (lseek(fd, 0, SEEK_SET) < 0) {
            perror("lseek error");
            exit(1);
        }

        char buf[3] = { 0 };
        if (read(fd, buf, 2) < 0) {
            printf("force lock works: %s  errno = %d\n", strerror(errno), errno);
        } else {
            printf("no force lock working, read content : %s  errno = %d\n", buf, errno);
        }
        TELL_PARENT(getppid());
    }
    return 0;    
}