#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sig_int(int signo) {
    printf("begin to sleep 5s\n");
    sleep(5);
    printf("\nend sleep\n");
}


int main(int argc, char *argv[]) {
    
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    kill(getpid(), SIGINT);
    
    return 0;
}
