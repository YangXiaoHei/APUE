#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

/*
 
 父进程阻塞 SIGCHLD
 父进程忽略 SIGINT, SIGQUIT
 
 */

int yh_system(const char *cmd) {
    pid_t pid;
    int status;
    sigset_t old_mask, new_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        return -1;
    }
    struct sigaction ignore_act, save_int_act, save_quit_act;
    ignore_act.sa_handler = SIG_IGN;
    ignore_act.sa_mask = 0;
    ignore_act.sa_flags = 0;
    if (sigaction(SIGINT, &ignore_act, &save_int_act) < 0) {
        return -1;
    }
    if (sigaction(SIGQUIT, &ignore_act, &save_quit_act) < 0) {
        return -1;
    }
    if ((pid = fork()) < 0) {
        return -1;
    } else if (pid == 0) {
        if (sigaction(SIGINT, &save_quit_act, NULL) < 0) {
            return -1;
        }
        if (sigaction(SIGQUIT, &save_quit_act, NULL) < 0) {
            return -1;
        }
        if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
            return -1;
        }
        execl("/bin/bash", "bash", "-c", cmd, (char *)0);
        _exit(127);
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    }
    return status;
}

void sig_int(int signo) {
    printf("\nsig_int signo = %d\n", signo);
}

void sig_quit(int signo) {
    printf("\nsig_quit signo = %d\n", signo);
}

int main(int argc, char *argv[]) {
    
    setbuf(stdout, NULL);
    
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (signal(SIGQUIT, sig_quit) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (yh_system("/bin/ed") < 0) {
        perror("system() error");
        exit(1);
    }

    return 0;
}
