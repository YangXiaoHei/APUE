#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int sig_flag;
sigset_t new_mask, old_mask, zero_mask;

void sig_usr(int signo) {
    sig_flag = 1;
}

void TELL_WAIT(void) {
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
    if (signal(SIGUSR1, sig_usr) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    if (signal(SIGUSR2, sig_usr) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR2);
}

void WAIT_PARENT(void) {
    while (sig_flag == 0) 
        sigsuspend(&zero_mask);
    sig_flag = 0;
}

void WAIT_CHILD(void) {
    while (sig_flag == 0) 
        sigsuspend(&zero_mask);
    sig_flag = 0;
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR1);
}

int main(int argc, char const *argv[])
{

    TELL_WAIT();

    int fd = open("test", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int init = 0;
    write(fd, &init, 4);
    lseek(fd, 0, SEEK_SET);

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        int num;
        for (int i = 0; i < 6; i++) {
            
            int rt = read(fd, &num, 4);
            lseek(fd, 0, SEEK_SET);
            num++;
            int wt = write(fd, &num, 4);
            printf("子进程 --- %d\n", num);
            lseek(fd, 0, SEEK_SET);
            
            TELL_PARENT(getppid());
            WAIT_PARENT();
        }
        TELL_PARENT(getppid());
        exit(0);
    }

    int num;

    WAIT_CHILD();
    for (int i = 0; i < 6; i++) {

        int rt = read(fd, &num, 4);
        lseek(fd, 0, SEEK_SET);
        num++;
        int wt = write(fd, &num, 4);
        lseek(fd, 0, SEEK_SET);
        printf("父进程 --- %d\n", num);

        TELL_CHILD(pid);
        WAIT_CHILD();
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }

    return 0;
}