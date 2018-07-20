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

void sig_alrm(int signo) {
    printf("sig_alrm received\n");
}

typedef void(*sig_handler)(int);

sig_handler yh_signal(int signo, sig_handler handler) {
    struct sigaction new_act, old_act;
    bzero(&new_act, sizeof(new_act));
    new_act.sa_handler = handler;
    if (signo == SIGALRM) {
        #ifdef SA_INTERRPUT
        new_act.sa_flags |= SA_INTERRPUT;
        #endif
    } else {
        new_act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &new_act, &old_act) < 0)
        return SIG_ERR;
    return old_act.sa_handler;
}

int main(int argc, char const *argv[])
{
    if (yh_signal(SIGALRM, sig_alrm) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    setbuf(stdout, NULL);

    FILE *fp = fopen(argv[1], "w+");
    if (fp == NULL) {
        printf("fopen error");
        exit(1);
    }

    char *buffer = malloc(1UL << 30);
    memset(buffer, 'b', (1UL << 30));
    printf("begin\n");

    alarm(4);

    if (fwrite(buffer, 1UL << 30, 1, fp) != 1) {
        printf("fwrite error\n");
        exit(1);
    }

    return 0;
}