#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <string.h>

void pr_status(int status) {
    char buf[128] = { 0 };
    if (WIFEXITED(status)) {
        snprintf(buf, sizeof(buf), "          normal termination with code : %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        snprintf(buf, sizeof(buf), "          signal termination with signo : %d\n", WTERMSIG(status));
    }
    write(STDOUT_FILENO, buf, strlen(buf));
}

void pr_mask(const char *msg) {
    
    write(STDOUT_FILENO, msg, strlen(msg));
    sigset_t old_mask;
    if (sigprocmask(0, NULL, &old_mask) < 0) {
        perror("sigprocmask");
        exit(1);
    } else {
        for (int i = 0; i < NSIG; ++i) {
            if (sigismember(&old_mask, i)) {
                char buf[128] = { 0 };
                snprintf(buf, sizeof(buf), "[ %d ]", i);
                write(STDOUT_FILENO, buf, strlen(buf));
            }
        }
    }
    write(STDOUT_FILENO, "\n", 1);
}

static sigjmp_buf jmp;


void sig_chld(int signo) {
    
    pr_mask("          in sig_chld ");
    
    int status;
    pid_t pid;
    if ((pid = wait(&status)) < 0) {
        perror("wait error");
        exit(1);
    }
    
    static unsigned int collect_count = 0;
    char buf[128] = { 0 };
    snprintf(buf, sizeof(buf), "          collect process %d\n", pid);
    write(STDOUT_FILENO, buf, strlen(buf));
    
    pr_status(status);
    
    if (++collect_count == 5) {
        siglongjmp(jmp, 1);
    }
}


int main(int argc, char *argv[]) {
    
    if (signal(SIGCHLD, sig_chld) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }

    pid_t pid;
    for (int i = 0; i < 5; i++) {
        if ((pid = fork()) < 0) {
            perror("fork error");
            exit(1);
        } else if (pid == 0) {
            struct sigaction ignore;
            ignore.sa_mask = 0;
            ignore.sa_handler = SIG_IGN;
            ignore.sa_flags = 0;
            
            if (sigaction(SIGCHLD, &ignore, NULL) < 0) {
                perror("sigaction error");
                exit(1);
            }
            
            char buf[128] = { 0 };
            snprintf(buf, sizeof(buf), "create process %d\n", getpid());
            write(STDOUT_FILENO, buf, strlen(buf));
            exit(3);
        }
    }
    
    while (!sigsetjmp(jmp, 1));
    
    return 0;
}
