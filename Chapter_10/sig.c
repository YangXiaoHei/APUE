#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void user_handler(int signo) {
    if (signo == SIGUSR1) {
        printf("capture user1\n");
    } else if (signo == SIGUSR2) {
        printf("capture user2\n");
    } else {
        printf("other signo = %d\n", signo);
    }
}

int main() {
    
    if (signal(SIGUSR1, user_handler) < 0) {
        printf("signal error\n");
        exit(1);
    }
    if (signal(SIGUSR2, user_handler) < 0) {
        printf("signal error\n");
        exit(1);
    }
    
    while (1)
        pause();
    
    
    return 0;
}
