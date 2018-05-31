#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>

ssize_t yh_printf(const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    char buf[4096] = { 0 };
    vsnprintf(buf, sizeof(buf), format, vl);
    buf[strlen(buf)] = '\n';
    ssize_t bytes = 0;
    for (bytes = 0; bytes < strlen(buf); bytes++) 
        if (write(STDOUT_FILENO, buf + bytes, 1) != 1) /* 让内核态切换更加频繁 */
            break;
    va_end(vl);
    return bytes;
}

typedef void(*sig_handler)(int);
sig_handler yh_signal(int signo, sig_handler handler) {
    struct sigaction new_act, old_act;
    bzero(&new_act, sizeof(new_act));
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        new_act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        new_act.sa_flags |= SA_RESTART;
    }
    sigemptyset(&new_act.sa_mask);
    new_act.sa_handler = handler;
    if (sigaction(signo, &new_act, &old_act) < 0)
        return SIG_ERR;
    return old_act.sa_handler; 
}

void sig_internal_handler(int signo) { /* do nothing */ }

void PREPARE() {
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    if (yh_signal(SIGUSR1, sig_internal_handler) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
    if (yh_signal(SIGUSR2, sig_internal_handler) == SIG_ERR) {
        perror("yh_signal error");
        exit(1);
    }
}

void WAIT_PARENT() {
    sigset_t mask;
    sigemptyset(&mask);
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigsuspend(&mask);
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1);
}

void WAIT_CHILD() {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR2);
    sigsuspend(&mask);
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2);
} 

int main(int argc, char *argv[]) {
    
    PREPARE();

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        WAIT_PARENT();
        char buf[20];
        memset(buf, 'A', sizeof(buf));
        buf[sizeof(buf) - 1] = 0;
        yh_printf(buf);
        TELL_PARENT(getppid());
    } else {
        char buf[20];
        memset(buf, 'O', sizeof(buf));
        buf[sizeof(buf) - 1] = 0;
        yh_printf(buf);
        TELL_CHILD(pid);
        WAIT_CHILD();
    }
    
    return 0;    
}