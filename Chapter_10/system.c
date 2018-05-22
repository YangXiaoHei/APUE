#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int yh_system(const char *cmd) {
    
    pid_t pid;
    int status;
    
    if (cmd == NULL)
        return 1;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
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

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("input one arg to exec\n");
        exit(1);
    }
    
    yh_system(argv[1]);
    
    
    return 0;
}
