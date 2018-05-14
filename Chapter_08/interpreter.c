#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    
    pid_t pid;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        if (execl("./test_interp","testinterp", "myarg1", "MY ARG2", (char *)0) < 0) {
            perror("execl");
        }
    }
    
    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        exit(1);
    }
    
    return 0;
}
