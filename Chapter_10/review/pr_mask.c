#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int sig2str(int signo, char **str) {

    if (str == NULL) {
        errno = EINVAL;
        return -1;
    }

//1
#ifdef SIGHUP
    if (signo == SIGHUP) {
        *str = "SIGHUP";
        return 1;
    }
#endif

//2
#ifdef SIGINT
    if (signo == SIGINT) {
        *str = "SIGINT";
        return 1;
    }
#endif

//3
#ifdef SIGQUIT
    if (signo == SIGQUIT) {
        *str = "SIGQUIT";
        return 1;
    }
#endif

// 4
#ifdef SIGILL
    if (signo == SIGILL) {
        *str = "SIGILL";
        return 1;
    }
#endif

// 5
#ifdef SIGTRAP
    if (signo == SIGTRAP) {
        *str = "SIGTRAP";
        return 1;
    }
#endif

// 6
#ifdef SIGABRT
    if (signo == SIGABRT) {
        *str = "SIGABRT";
        return 1;
    }
#endif

// 7
#ifdef SIGPOLL
    if (signo == SIGPOLL) {
        *str = "SIGPOLL";
        return 1;
    }
#endif

// 8
#ifdef SIGIOT
    if (signo == SIGIOT) {
        *str = "SIGIOT";
        return 1;
    }
#endif

// 7
#ifdef SIGEMT
    if (signo == SIGEMT) {
        *str = "SIGEMT";
        return 1;
    }
#endif

// 8
#ifdef SIGFPE
    if (signo == SIGFPE) {
        *str = "SIGFPE";
        return 1;
    }
#endif

// 9
#ifdef SIGKILL
    if (signo == SIGKILL) {
        *str = "SIGKILL";
        return 1;
    }
#endif

// 10
#ifdef SIGBUS
    if (signo == SIGBUS) {
        *str = "SIGBUS";
        return 1;
    }
#endif

// 11
#ifdef SIGSEGV
    if (signo == SIGSEGV) {
        *str = "SIGSEGV";
        return 1;
    }
#endif

// 12
#ifdef SIGSYS
    if (signo == SIGSYS) {
        *str = "SIGSYS";
        return 1;
    }
#endif

// 13
#ifdef SIGPIPE
    if (signo == SIGPIPE) {
        *str = "SIGPIPE";
        return 1;
    }
#endif

// 14
#ifdef SIGALRM
    if (signo == SIGALRM) {
        *str = "SIGALRM";
        return 1;
    }
#endif

// 15
#ifdef SIGTERM
    if (signo == SIGTERM) {
        *str = "SIGTERM";
        return 1;
    }
#endif

// 16
#ifdef SIGURG
    if (signo == SIGURG) {
        *str = "SIGURG";
        return 1;
    }
#endif

// 17
#ifdef SIGSTOP
    if (signo == SIGSTOP) {
        *str = "SIGSTOP";
        return 1;
    }
#endif

// 18
#ifdef SIGTSTP
    if (signo == SIGTSTP) {
        *str = "SIGTSTP";
        return 1;
    }
#endif

// 19
#ifdef SIGCONT
    if (signo == SIGCONT) {
        *str = "SIGCONT";
        return 1;
    }
#endif

// 20
#ifdef SIGCHLD
    if (signo == SIGCHLD) {
        *str = "SIGCHLD";
        return 1;
    }
#endif

// 21
#ifdef SIGTTIN
    if (signo == SIGTTIN) {
        *str = "SIGTTIN";
        return 1;
    }
#endif

// 22
#ifdef SIGTTOU
    if (signo == SIGTTOU) {
        *str = "SIGTTOU";
        return 1;
    }
#endif

// 23
#ifdef SIGIO
    if (signo == SIGIO) {
        *str = "SIGIO";
        return 1;
    }
#endif

// 24
#ifdef SIGXFSZ
    if (signo == SIGXFSZ) {
        *str = "SIGXFSZ";
        return 1;
    }
#endif

// 25
#ifdef SIGXFSZ
    if (signo == SIGXFSZ) {
        *str = "SIGXFSZ";
        return 1;
    }
#endif

// 26
#ifdef SIGVTALRM
    if (signo == SIGVTALRM) {
        *str = "SIGVTALRM";
        return 1;
    }
#endif

// 27
#ifdef SIGPROF
    if (signo == SIGPROF) {
        *str = "SIGPROF";
        return 1;
    }
#endif

// 28
#ifdef SIGWINCH
    if (signo == SIGWINCH) {
        *str = "SIGWINCH";
        return 1;
    }
#endif

// 29
#ifdef SIGINFO
    if (signo == SIGINFO) {
        *str = "SIGINFO";
        return 1;
    }
#endif

// 30
#ifdef SIGUSR1
    if (signo == SIGUSR1) {
        *str = "SIGUSR1";
        return 1;
    }
#endif

// 31
#ifdef SIGUSR2
    if (signo == SIGUSR2) {
        *str = "SIGUSR2";
        return 1;
    }
#endif

#ifdef SIGINFO
    if (signo == SIGINFO) {
        *str = "SIGINFO";
        return 1;
    }
#endif

#ifdef SIGINFO
    if (signo == SIGINFO) {
        *str = "SIGINFO";
        return 1;
    }
#endif
    errno = EINVAL;
    return -1;
}



void pr_mask(const char *msg) {
    if (msg != NULL)
        printf("%s ", msg);
    sigset_t mask;
    sigemptyset(&mask);
    sigprocmask(0, NULL, &mask);
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(&mask, i)) {
            char *str = NULL;
            sig2str(i, &str);
            printf("%s \n", str);
        }
    }
}


int main(int argc, char const *argv[])
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    pr_mask(NULL);


    return 0;
}