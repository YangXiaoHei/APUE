#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sig_int(int signo) {
    printf("sig_int\n");
}

void (*yh_signal(int signo, void (*sig_handler)(int)))(int) {
    struct sigaction act, oact;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return oact.sa_handler;
}

int main() {
    
    if (yh_signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal register error");
        exit(1);
    }
    
    char buf[1024];
    if (read(STDIN_FILENO, buf, sizeof(buf)) < 0) {
        perror("read fail");
        exit(1);
    }
    printf("%s\n", buf);
    
    return 0;
}
