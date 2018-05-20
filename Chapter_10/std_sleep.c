#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


void sig_alrm(int signo) {
    printf("received signo = %d\n", signo);
}


int main(int argc, char *argv[]) {
    
    if (signal(SIGALRM, sig_alrm) == SIG_ERR) {
        perror("signal");
    }
    alarm(3);
    printf("begin sleep \n");
    sleep(100);
    printf("after sleep \n");
    return 0;
}
