#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

static void __internal_donothing(int signo) {
    printf("__internal recevied signo %d\n", signo);
}

static void caller_register_sig_alrm(int signo) {
    printf("caller register recevied signo %d\n", signo);
}

unsigned int yh_sleep(unsigned int seconds) {
    unsigned int now_alrm, uslept;
    int reset_flag = 0;
    typedef void (*__sig_handler_t)(int);
    __sig_handler_t old_handler;    /* 记录旧的 handler */
    if ((now_alrm = alarm(0)) != 0) {
        if (now_alrm > seconds) {
            reset_flag = 1;     /* 重置 alarm */
        } else {
            seconds = now_alrm; /* 经测试，这是系统默认做法，打断 sleep */
        }
    }
    if ((old_handler = signal(SIGALRM, __internal_donothing)) == SIG_ERR) {
        return seconds;
    }
    alarm(seconds);
    pause();
    uslept = alarm(0);
    if (reset_flag)
        alarm(now_alrm - seconds);
    if (signal(SIGALRM, old_handler) == SIG_ERR) {
        perror("signal restore old handler fail");
    }
    return uslept;
}

int main() {
    
    if (signal(SIGALRM, caller_register_sig_alrm) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    alarm(7);
//    alarm(3);
    
    printf("before sleep\n");
    yh_sleep(1);
//    yh_sleep(100);
    printf("after sleep \n");
    
    while (1);
    
    return 0;
}
