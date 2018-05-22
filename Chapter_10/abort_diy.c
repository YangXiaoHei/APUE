#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void yh_abort() {
    
    sigset_t mask;
    struct sigaction old_act;
    sigaction(SIGABRT, NULL, &old_act);
    if (old_act.sa_handler == SIG_IGN) {
        old_act.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &old_act, NULL);
    }
    
    if (old_act.sa_handler == SIG_DFL)
        fflush(NULL);
    
    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    kill(getpid(), SIGABRT);
    
    fflush(NULL);
    old_act.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &old_act, NULL);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    kill(getpid(), SIGABRT);
    exit(1);
}

void sig_abrt(int signo) {
    printf("sig_abrt signo = %d\n", signo);
}

int main() {
    
    if (signal(SIGABRT, sig_abrt) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    
    yh_abort();
    
    
    return 0;
}
