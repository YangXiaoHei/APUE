#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void sig_handler(int signo) {
    printf("Proc %d capture signo %d\n",getpid(), signo);
}

void (*yh_signal(int signo, void (*sig_handler)(int))) (int) {
    struct sigaction act, oact;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    ct.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return oact.sa_handler;
}


int main(int argc, char *argv[]) {
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        yh_signal(SIGUSR1, sig_handler);
        char buf[1024];
        while (fgets(buf, sizeof(buf), stdin) != NULL) {
            if (buf[strlen(buf) - 1] != '\n') {
                buf[strlen(buf) - 1] = '\n';
            }
            fputs(buf, stdout);
        }
    } else {
        sleep(5);
        printf("Child %d begin to send SIGUSR1\n", getpid());
        kill(getppid(), SIGUSR1);
    }

    return 0;
}
