#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

static volatile sig_atomic_t sigflag;
static sigset_t new_mask, old_mask, zero_mask;

static void sig_usr(int signo) {
    sigflag = 1;
}

void TELL_WAIT(void) {
    if (signal(SIGUSR1, sig_usr) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (signal(SIGUSR2, sig_usr) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    sigemptyset(&new_mask);
    sigemptyset(&zero_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    
    /***************** 这一段去掉的话会有什么问题 ？ ************/
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    /*******************************************************/
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2);
}

void WAIT_CHILD(void) {
    while (sigflag == 0)
        sigsuspend(&zero_mask);
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("SIG_SETMASK error");
    }
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1);
}

void WAIT_PARENT(void) {
    while (sigflag == 0)
        sigsuspend(&zero_mask);
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    
    pid_t pid;
    
    setbuf(stdout, NULL);
    
    TELL_WAIT();
    
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        WAIT_PARENT();
        
        for (int i = 0; i < 512; ++i)
            printf("A");
        
        exit(0);
    }
    
    for (int i = 0; i < 512; ++i)
         printf("O");
        
    TELL_CHILD(pid);
    
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }

    return 0;
}
