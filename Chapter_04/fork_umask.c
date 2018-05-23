#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    setbuf(stdout, NULL);
    
    printf("old umask = %d\n", umask(0444));
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        printf("child process umask = %d\n", umask(0));
    }
    
    
    return 0;
}
