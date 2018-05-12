#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    
    pid_t pid;
    int status;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if (pid > 0) {
            exit(0);
        }
        
        sleep(2);
        printf("second child proc, parent pid = %d self = %d\n", getppid(), getpid());
        exit(0);
    }
    
    if (waitpid(pid, &status, 0) != pid) {
        perror("waitpid");
        exit(1);
    }
    
    return 0;
}
