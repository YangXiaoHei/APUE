#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sig_int(int signo) {
    printf("caught SIGINT\n");
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
        printf("can't reset SIGINT");
        exit(1);
    }
}

int main() {
    
    sigset_t newmask, oldmask, pendmask;
    
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("sigprocmask");
        exit(1);
    }
    
    printf("begin to sleep 5s\n");
    sleep(5);
    printf("end sleep\n");
    
    if (sigpending(&pendmask) < 0) {
        perror("sigpending ");
        exit(1);
    }
    if (sigismember(&pendmask, SIGINT)) {
        printf("\nSIGINT pending\n");
    }
    
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    printf("SIGINT unblock\n");
    
    printf("begin to sleep 5s\n");
    sleep(5);
    printf("end sleep\n");
    return 0;
}
