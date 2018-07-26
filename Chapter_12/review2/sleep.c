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

void sig_alrm(int signo) {}

unsigned int yh_sleep(unsigned int seconds)
{
    struct sigaction newact, oldact;
    sigset_t newmask, oldmask, suspmask;
    newact.sa_handler = sig_alrm;
    newact.sa_flags = 0;
    sigaction(SIGALRM, &newact, &oldact);

    sigemptyset(&newmask);
    sigaddset(&newact, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    suspmask = oldmask;
    alarm(seconds);

    sigsuspend(&suspmask);

    unslept = alarm(0);

    sigaction(SIGALRM, &oldact, NULL);
    sigprocmaks(SIG_SETMASK, &oldmask, NULL);
    
    return  unslept; 
}

int main(int argc, char const *argv[])
{
    
    return 0;
}