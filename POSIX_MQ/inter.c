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

ssize_t printf_(const char *format, ...) {
    char buf[256] = { 0 };
    va_list vl;
    va_start(vl, format);
    vsnprintf(buf, sizeof(buf), format, vl);
    va_end(vl);
    ssize_t nwritten = write(STDOUT_FILENO, buf, strlen(buf)) != strlen(buf);
    return nwritten;
}

void sig_usr1(int signo) {
    for (int i = 0; i < 20; i++) {
        printf_("👼 SIGUSR1 %d\n", i);
        usleep(100000);
    }
}

void sig_usr2(int signo) {
    for (int i = 0; i < 20; i++) {
        printf_("🏊 SIGUSR2 --------- %d\n", i);
        usleep(300000);
    }
}

typedef void(*sig_func)(int);
sig_func signal_(int signo, sig_func func, sigset_t *mask) {
    struct sigaction sa, old;
    if (mask != NULL)
        sa.sa_mask = *mask;
    else
        sigemptyset(&sa.sa_mask);
    sa.sa_handler = func;
    if (signo == SIGALRM) {
        #ifdef SA_INTERRUPT
        sa.sa_flags |= SA_INTERRUPT;
        #endif
    } else {
        #ifdef SA_RESTART
        sa.sa_flags |= SA_RESTART;
        #endif
    }
    if (sigaction(signo, &sa, &old) < 0) 
        return SIG_ERR;
    return old.sa_handler;
}

int main(int argc, char const *argv[])
{
    sigset_t new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR2);

    if (signal_(SIGUSR1, sig_usr1, NULL) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    if (signal_(SIGUSR2, sig_usr2, NULL) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(1);
        kill(getppid(), SIGUSR2);
        exit(0);
    }

    kill(getpid(), SIGUSR1);

    while (1)
        pause();

    return 0;
}