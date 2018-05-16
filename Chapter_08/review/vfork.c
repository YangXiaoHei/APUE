#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int glob = 0;

int main(int argc, char *argv[]) {
    
    int local = 8;
    
    pid_t pid;
    if ((pid = vfork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        glob++;
        local++;
        _exit(0);
    }
    
    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        exit(1);
    }
    
    printf("glob = %d  local = %d\n", glob, local);
    
    return 0;
}
