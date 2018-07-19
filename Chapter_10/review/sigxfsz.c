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

void sig_fzx(int signo) {
    fprintf(stderr, "SIGXFSZ captured\n");
}

#define BUFSIZE 100

int main(int argc, char const *argv[])
{

    if (signal(SIGXFSZ, sig_fzx) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    struct rlimit rl;
    if (getrlimit(RLIMIT_FSIZE, &rl) < 0) {
        perror("getrlimit error");
        exit(1);
    }

    rl.rlim_cur = 1024;

    if (setrlimit(RLIMIT_FSIZE, &rl) < 0) {
        perror("setrlimit error");
        exit(1);
    }

    char buf[BUFSIZE];
    int nread, nwrite;
    while ((nread = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
try_again:
        if ((nwrite = write(STDOUT_FILENO, buf, nread)) != nread) {
            if (nwrite < 0) {
                fprintf(stderr, "incompletely nwrite = %d bytes\n", nwrite);
                perror("write error");
                exit(1);
            }
            fprintf(stderr, "incompletely nwrite = %d bytes\n", nwrite);
            nwrite = nread - nwrite;
            goto try_again;
        }
    }
    fprintf(stderr, "finish\n");

    return 0;
}