#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CMD  "ps -o pid,ppid,state,tty,command"

int main(int argc, char *argv[]) {
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        exit(0);
    }

    sleep(5);
    system(CMD);
    return 0;    
}