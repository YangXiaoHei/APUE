#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

void pr_exit(int status) {
    
    /* 若子进程正常终止 */
    if (WIFEXITED(status)) {
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
        
    /* 若子进程接到一个不捕获的信号 */
    } else if (WIFSIGNALED(status)) {
        printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
               /* 如果已产生终止进程的 core 文件 */
#ifdef WCOREDUMP
               WCOREDUMP(status) ? "(core file generated)" : "");
#else
        "");
#endif
        /* 若当前子进程为暂停状态 */
    } else if (WIFSTOPPED(status)) {
        printf("child stopped, signal number = %d\n",
               /* 获取使子进程暂停的信号编号 */
               WSTOPSIG(status));
    }
}


int main(int argc, char *argv[]) {
    
    pid_t pid;
    int   status;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        exit(7);
    }
    
    if (wait(&status) != pid) {
        perror("wait error");
        exit(1);
    }
    pr_exit(status);
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        abort();
    }
    
    if (wait(&status) != pid) {
        perror("wait error");
        exit(1);
    }
    pr_exit(status);
    
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        status /= 0;
    }
    
    if (wait(&status) != pid) {
        perror("wait error");
        exit(1);
    }
    pr_exit(status);
    
    
    return 0;
}
