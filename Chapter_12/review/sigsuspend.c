#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main() {
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    
    printf("before\n");
    
    sigsuspend(&mask);
    
    printf("after\n");
    
    return 0;
}
