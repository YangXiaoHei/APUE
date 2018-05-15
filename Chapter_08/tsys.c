#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void pr_exit(int status) {
    
    if (WIFEXITED(status)) {
        printf("normal termination\n");
    } else if (WIFSIGNALED(status)) {
        printf("abnormal termination signal = %d%s\n", WTERMSIG(status),
               
#ifdef WCOREDUMP
               WCOREDUMP(status) ? "(core file generated)" : "");
#else
        "");
#endif
    } else if (WIFSTOPPED(status)) {
        printf("child stopped, signal = %d\n", WSTOPSIG(status));
    }
}


int main(int argc, char *argv[]) {
    
    int status;
    
    if (argc < 2) {
        printf("command-line argument required\n");
        exit(1);
    }
    
    if ((status = system(argv[1])) < 0) {
        printf("system() error");
        exit(1);
    }
    
    pr_exit(status);
    
    return 0;
}
