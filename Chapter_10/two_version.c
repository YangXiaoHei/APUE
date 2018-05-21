#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

typedef void(*sig_fun_t) (int);
sig_fun_t signal(int signo, sig_fun_t handler) {
    struct sigaction new_act, old_act;
    sigemptyset(&new_act.sa_mask);
    sigemptyset(&old_act.sa_mask);
    new_act.sa_handler = handler;
    new_act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        new_act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        new_act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &new_act, &old_act) < 0)
        return SIG_ERR;
    return old_act.sa_handler;
}

sig_fun_t signal_inter(int signo, sig_fun_t handler) {
    struct sigaction new_act, old_act;
    sigemptyset(&new_act.sa_mask);
    sigemptyset(&old_act.sa_mask);
    new_act.sa_handler = handler;
    new_act.sa_flags = 0;
#ifdef SA_INTERRUPT
    new_act.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &new_act, &old_act) < 0)
        return SIG_ERR;
    return old_act.sa_handler;
}

int main() {

    
    return 0;
}
