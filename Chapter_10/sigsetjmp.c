#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>

static sigjmp_buf jmp;
static volatile sig_atomic_t canjump;

static void pr_mask(const char *msg) {
    printf("%s", msg);
    sigset_t curset;
    sigemptyset(&curset);
    if (sigprocmask(0, NULL, &curset) < 0) {
        perror("sigprocmask error");
        exit(1);
    } else {
        extern const char *const sys_siglist[NSIG];
        for (int i = 0; i < NSIG; ++i) {
            if (sigismember(&curset, i)) {
                printf("【 %d 】", i);
            }
        }
        printf("\n");
    }
}

static void sig_usr1(int signo) {
    
    time_t start;
    if (canjump == 0)
        return;
    
    pr_mask("starting user1 :");
    
    alarm(3);
    start = time(NULL);
    while (1)
        if (time(NULL) > start + 5)
            break;
    pr_mask("finishing sig_usr1 : ");
    
    canjump = 0;
    siglongjmp(jmp, 1);
}

static void sig_alrm(int signo) {
    pr_mask("in sig_alrm : ");
}

typedef void(*yh_sig_handler_t) (int);
yh_sig_handler_t signal(int signo, yh_sig_handler_t handler) {
    struct sigaction old_act, new_act;
    sigemptyset(&old_act.sa_mask);
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

int main(int argc, char *argv[]) {
    
    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    pr_mask("starting main: ");
    
    if (sigsetjmp(jmp, 1)) {
        pr_mask("ending main :");
        exit(1);
    }
    
    canjump = 1;
    
    while (1)
        pause();
    
    return 0;
}
