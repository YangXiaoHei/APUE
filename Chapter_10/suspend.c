#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t quitflag;

static void sig_int(int signo) {
    if (signo == SIGINT) {
        printf("\n interrupt \n");
    } else if (signo == SIGQUIT) {
        quitflag = 1;
    }
}

int main() {
    
    sigset_t new_mask, old_mask, zero_mask;
    
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (signal(SIGQUIT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    sigemptyset(&zero_mask);
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGQUIT);
    
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    
    while (quitflag == 0)
        sigsuspend(&zero_mask);
    
    quitflag = 0;
    
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    
    return 0;
}
