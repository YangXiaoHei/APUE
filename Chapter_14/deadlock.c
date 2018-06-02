#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

sigset_t new_mask, old_mask, zero_mask;
volatile int sig_flag;

void __sig_usr(int signo)  {
    sig_flag = 1;
}

typedef void (*sig_handler)(int);
sig_handler yh_signal(int signo, sig_handler handler) {
    struct sigaction new_act, old_act;
    new_act.sa_handler = handler;
    // 我只觉得 sigemptyset(&new_act.sa_mask) 和 new_act.sa_mask = 0 就是一样的...
    // 因为 sigset_t 是个 int
    new_act.sa_mask = 0;   
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

void PREPARE() {
    sigemptyset(&new_mask);
    sigemptyset(&old_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigaddset(&new_mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) {
        perror("sigprocmask error");
        exit(1);
    }

    if (yh_signal(SIGUSR1, __sig_usr) == SIG_ERR) {
        printf("yh_signal fail\n");
        exit(1);
    }
    if (yh_signal(SIGUSR2, __sig_usr) == SIG_ERR) {
        printf("yh_signal fail\n");
        exit(1);
    }
}

void WAIT_PARENT(void) {

    while (sig_flag == 0) 
        sigsuspend(&zero_mask);
    sig_flag = 0;
    
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1);
}

void WAIT_CHILD(void) {
    while (sig_flag == 0) 
        sigsuspend(&zero_mask);
    sig_flag = 0;

    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) < 0) {
        perror("sigprocmask error");
        exit(1);
    }
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2);
}

void sync_test() {

    PREPARE();

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid  == 0) {
        WAIT_PARENT();
        char buf[1024] = { 0 };
        memset(buf, 'A', sizeof(buf) - 1);
        for (int i = 0; i < strlen(buf); i++) 
            write(STDOUT_FILENO, buf + i, 1);
        write(STDOUT_FILENO, "\n", 1);
        TELL_PARENT(getppid());
        exit(0);
    }

    char buf[1024] = { 0 };
    memset(buf, 'O', sizeof(buf) - 1);
    for (int i = 0; i < strlen(buf); i++) 
        write(STDOUT_FILENO, buf + i, 1);
    write(STDOUT_FILENO, "\n", 1);

    TELL_CHILD(pid);
    WAIT_CHILD();

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    
    int fd;
    if ((fd = creat("tmplock", 0644)) < 0) {
        perror("creat fail");
        exit(1);
    }

    if (write(fd, "ab", 2) != 2) {
        perror("write fail\n");
        exit(1);
    }

    PREPARE();

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        struct flock first_byte;
        first_byte.l_type = F_WRLCK;
        first_byte.l_whence = SEEK_SET;
        first_byte.l_start = 0;
        first_byte.l_len = 1;
        if (fcntl(fd, F_SETLK, &first_byte) < 0) {
            perror("child proc lock first fail\n");
            exit(1);
        }
        printf("child proc first succ\n");
        TELL_PARENT(getppid());
        WAIT_PARENT();

        struct flock second_byte;
        second_byte.l_type = F_WRLCK;
        second_byte.l_whence = SEEK_SET;
        second_byte.l_start = 1;
        second_byte.l_len = 1;
        if (fcntl(fd, F_SETLKW, &second_byte) < 0) {
            perror("child proc lock second fail");
            exit(1);
        }
        printf("child proc second succ\n");

    } else {
        struct flock second_byte;
        second_byte.l_type = F_WRLCK;
        second_byte.l_whence = SEEK_SET;
        second_byte.l_start = 1;
        second_byte.l_len = 1;
        if (fcntl(fd, F_SETLK, &second_byte) < 0) {
            perror("parent proc lock first fail");
            exit(1);
        }
        printf("parent proc second succ\n");
        TELL_CHILD(pid);
        WAIT_CHILD();

        struct flock first_byte;
        first_byte.l_type = F_WRLCK;
        first_byte.l_whence = SEEK_SET;
        first_byte.l_start = 0;
        first_byte.l_len = 1;
        // 如果不是 F_SETLKW 标志位，而是 F_SETLK 的话，是不会提示死锁的，只会提示资源暂不可用
        // 毕竟 F_SETLK 不会阻塞
        if (fcntl(fd, F_SETLKW, &first_byte) < 0) {
            perror("child proc lock first fail");
            exit(1);
        }
        printf("parent proc first succ\n");
    }
    
    
    return 0;    
}








