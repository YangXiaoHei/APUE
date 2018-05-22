#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

void pr_status(int status) {
    if (WIFEXITED(status)) {
        printf("normal termination with code : %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("signal termination with signo : %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
        printf("stop termination with signo : %d\n", WSTOPSIG(status));
    }
}

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
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127);
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }
    }
    pr_status(status);
    
    if (sigaction(SIGINT, &save_int_act, NULL) < 0) {
        return -1;
    }
    if (sigaction(SIGQUIT, &save_quit_act, NULL) < 0) {
        return -1;
    }
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        return -1;
    }
    
    return status;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("./a.out command args\n");
        exit(1);
    }
    
    setbuf(stdout, NULL);
    if (yh_system(argv[1]) < 0) {
        perror("system() error");
        exit(1);
    }

    return 0;
}
