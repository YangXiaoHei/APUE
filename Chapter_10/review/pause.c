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

void sig_us(int signo) {
    if (signo == SIGUSR1)
        printf("received SIGUSR1\n");
    else if (signo == SIGUSR2)
        printf("received SIGUSR2\n");
    else
        printf("unexpected signal\n");
}

int main(int argc, char const *argv[])
{
    if (signal(SIGUSR1, sig_us) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (signal(SIGUSR2, sig_us) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    while (1)
        pause();
    return 0;
}