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
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

typedef void(*sig_info_handler)(int, siginfo_t *, void *);
sig_info_handler signal_rt(int signo, sig_info_handler handler, sigset_t *mask) {
    struct sigaction sa;
    sa.sa_mask = *mask;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        sa.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef SA_RESTART
        sa.sa_flags |= SA_RESTART;
#endif
    }
    if (sigaction(signo, &sa, NULL) < 0) 
        return (sig_info_handler)SIG_ERR;
    return sa.sa_sigaction;
}

void handler(int signo, siginfo_t *info, void *ctx) {
    printf("received signal #%d, code = %d, ival = %d\n",
        signo, (int)info->si_code, (int)info->si_value.sival_int);
}

int main(int argc, char const *argv[])
{
    printf("SIGRTMIN = %d  SIGRTMAX = %d\n", SIGRTMIN, SIGRTMAX);

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sigset_t new_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask, SIGRTMAX);
        sigaddset(&new_mask, SIGRTMAX - 1);
        sigaddset(&new_mask, SIGRTMAX - 2);
        if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) {
            perror("sigprocmask error");
            exit(1);
        }

        if (signal_rt(SIGRTMAX, handler,  &new_mask) == (sig_info_handler)SIG_ERR) {
            perror("signal_rt error");
            exit(1);
        }
        if (signal_rt(SIGRTMAX - 1, handler,  &new_mask) == (sig_info_handler)SIG_ERR) {
            perror("signal_rt error");
            exit(1);
        }
        if (signal_rt(SIGRTMAX - 2, handler,  &new_mask) == (sig_info_handler)SIG_ERR) {
            perror("signal_rt error");
            exit(1);
        }

        sleep(6);

        if (sigprocmask(SIG_UNBLOCK, &new_mask, NULL) < 0) {
            perror("sigprocmask error");
            exit(1);
        }
        sleep(3);
        exit(0);
    }

    sleep(3);

    union sigval val;
    for (int i = SIGRTMAX; i >= SIGRTMAX - 2; i--) {
        for (int j = 0; j < 3; j++) {
            val.sival_int = j;
            if (sigqueue(pid, i, val) < 0) {
                perror("sigqueue error");
                exit(1);
            }
            printf("sent signal %d, val = %d\n", i, j);
        }
    }
    return 0;
}