#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CMD "ps -o pid,ppid,state,tty,command"

int main(int argc, char *argv[]) {
    
    pid_t pid;
    if ((pid = vfork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        sleep(100);
    }
    
    return 0;    
}