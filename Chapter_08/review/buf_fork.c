#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    
    pid_t pid;
    int status;
    
//    printf("hello world\n");
    
    write(STDOUT_FILENO, "hello wolrd\n", 12);
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        exit(1);
    }
    
    if (wait(&status) < 0) {
        perror("wait fail");
        exit(1);
    }
    
    return 0;
}
