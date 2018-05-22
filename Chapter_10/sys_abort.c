#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sig_abrt(int signo) {
    printf("sig_abrt signo = %d\n", signo);
}

int main() {
    
    if (signal(SIGABRT, sig_abrt) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    abort();
    
    return 0;
}
