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

/*
 * 该程序用于证明 FIFO 不是真正意义上的全双工管道，只是一个半双工管道具有全双工的能力
 */

#define FIFO_PATH  "/tmp/yh_fifo.1"

typedef void(*sig_handler)(int);
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

int sig_flag1;
int sig_flag2;
void __sig_usr1(int signo) {
    sig_flag1 = 1;
}

void __sig_usr2(int signo) {
    sig_flag2 = 1;
}

sigset_t old_mask, new_mask, zero_mask;
void TELL_WAIT() {
    sigemptyset(&old_mask);
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    sigemptyset(&zero_mask);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    if (yh_signal(SIGUSR1, __sig_usr1) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
    if (yh_signal(SIGUSR2, __sig_usr2) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1);
}

void WAIT_PARENT(void) {
    while (sig_flag1 == 0) 
        sigsuspend(&zero_mask);
    sig_flag1 = 0;
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2);
}

void WAIT_CHILD(void) {
    while (sig_flag2 == 0) 
        sigsuspend(&zero_mask);
    sig_flag2 = 0;
}

int main(int argc, char const *argv[]) {
    
    if (mkfifo(FIFO_PATH, 0644) < 0 && errno != EEXIST) {
        perror("mkfifo error");
        exit(1);
    }

    int fd = open(FIFO_PATH, O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    TELL_WAIT();

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        WAIT_PARENT();

        /* 2 */
        printf("step 2\n");

        if (write(fd, "xyz", 3) != 3) {
            perror("child write error");
            exit(1);
        }
        TELL_PARENT(getppid());
        WAIT_PARENT();

        /* 4 */
        printf("step 4\n");

        char buf[3] = { 0 };
        if (read(fd, buf, sizeof(buf) - 1) < 0) {
            perror("child read error");
            exit(1);
        }
        printf("child read content : %s\n", buf);
        TELL_PARENT(getppid());
        exit(0);
    } else {

        /* 1 */
        printf("step 1\n");

        if (write(fd, "abc", 3) != 3) {
            perror("parent write error");
            exit(1);
        }
        TELL_CHILD(pid);
        WAIT_CHILD();

        /* 3 */
        printf("step 3\n");

        char buf[3] = { 0 };
        if (read(fd, buf, sizeof(buf) - 1) < 0) {
            perror("parent read error");
            exit(1);
        }
        printf("parent read content : %s\n", buf);
        TELL_CHILD(pid);
        WAIT_CHILD();

        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid error");
            exit(1);
        }
    }

    return 0;
}