#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/*
 
 SA_SIGINFO
 
 siginfo_t  结构体中的内容
 void * 可以被强转为 ucontext_t 
 
 
 */

void sig_int(int signo, siginfo_t *info, void *arg) {
    printf("signo %d\n", signo);
    printf("info %ld\n", (long)info);
    printf("arg %ld\n", (long)arg);
}


/*
 
 // SA_NODEFER
 
int i = 0;
void sig_int(int signo) {
    ++i;
    printf("begin to sleep 5s\n");
    sleep(5);
    printf("end sleep i = %d\n", i);
}
 */

int main() {
    
    struct siginfo
    
    
    struct sigaction old_act, new_act;
    sigemptyset(&new_act.sa_mask);
    sigemptyset(&old_act.sa_mask);
    new_act.sa_sigaction = sig_int;
    new_act.sa_flags |= SA_SIGINFO;
    if (sigaction(SIGINT, &new_act, &old_act) < 0) {
        perror("sigaction error");
        exit(1);
    }
    kill(getpid(), SIGINT);
    return 0;
}
