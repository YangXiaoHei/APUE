#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sig_alrm(int signo) { /* do nothing */ }

unsigned int yh_sleep(unsigned int seconds) {
    struct sigaction new_act, old_act;
    sigset_t new_mask, old_mask, suspend_mask;
    unsigned int unslept;
    
    /* set our handler, save previous information */
    new_act.sa_handler = sig_alrm;
    sigemptyset(&new_act.sa_mask);
    new_act.sa_flags = 0;
    sigaction(SIGALRM, &new_act, &old_act);
    
    /* block SIGALRM and save current signal mask */
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGALRM);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
    
    alarm(seconds);
    suspend_mask = old_mask;
    
    /* make sure SIGALRM isn't blocked */
    sigdelset(&suspend_mask, SIGALRM);
    
    /* wait for any signal to be caught */
    sigsuspend(&suspend_mask);
    
    /* som signal has been caught, SIGALRM is now blocked */
    unslept = alarm(0);
    
    /* reset previous action */
    sigaction(SIGALRM, &old_act, NULL);
    
    /* reset signal mask, which unblocks SIGALRM */
    sigprocmask(SIG_SETMASK, &old_act, NULL);
    return unslept;
}

int main() {
    
    
    return 0;
}
