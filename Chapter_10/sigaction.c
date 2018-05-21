#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


void sig_int(int signo) {
    printf("begin sleep 5s\n");
    sleep(5);
    printf("\nend sleep\n");
}


int main() {
   
    struct sigaction new_act, old_act;
    sigemptyset(&new_act.sa_mask);
    new_act.sa_handler = sig_int;
    new_act.sa_flags = 0;
    if (sigaction(SIGINT, &new_act, &old_act) < 0) {
        perror("sigaction error");
        exit(1);
    }
    
    kill(getpid(), SIGINT);
    return 0;
}
