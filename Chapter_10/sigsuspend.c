#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef void(*yh_sig_handler) (int);

yh_sig_handler yh_signal(int signo, yh_sig_handler handler) {
    struct sigaction new_act, old_act;
    sigemptyset(&new_act.sa_mask);
    new_act.sa_flags = 0;
    new_act.sa_handler = handler;
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

void pr_mask(const char *msg) {
    printf("%s : ", msg);
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigemptyset(&old_mask);
    if (sigprocmask(0, NULL, &old_mask) < 0) {
        perror("sigprocmask ");
        exit(0);
    } else {
        for (int i = 0; i < NSIG; ++i) {
            if (sigismember(&old_mask, i)) {
                printf("【 %d 】", i);
            }
        }
        printf("\n");
    }
}

void sig_int(int signo) {
    pr_mask("in sig_int");
}

void sig_usr1(int signo) {
    pr_mask("in sig_usr1");
}

int main() {
    pr_mask("main starting...");
    
    if (yh_signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    if (yh_signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    sigset_t new_mask, old_mask, wait_mask;
    sigemptyset(&new_mask);
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGUSR1);
    sigaddset(&new_mask, SIGINT);
    
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask ");
        exit(1);
    }
    
    pr_mask("in critical region");
    
    if (sigsuspend(&wait_mask) != -1) {
        perror("sigsuspend error");
        exit(1);
    }
    
    pr_mask("after return from sigsuspend");
    
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    
    pr_mask("program exit");
   
    return 0;
}
