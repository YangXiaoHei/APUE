#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#define WRITE(_msg_) \
write(STDOUT_FILENO, _msg_"\n", sizeof(_msg_))

jmp_buf jmp;

void __internal_donothing(int signo) {
    WRITE("internal donothing");
    longjmp(jmp, 1);
}

unsigned int yh_sleep(unsigned int seconds) {
    unsigned int now_alarm  = alarm(0),
                 unslept    = 0,
                 reset_flag = 0,
                 old_invoke = 0;
    
    if (now_alarm != 0) {
        if (now_alarm < seconds) {
            seconds = now_alarm;
            old_invoke = 1;
        } else {
            reset_flag = 1;
        }
    }
    
    typedef void (*__sig_handler_t)(int);
    __sig_handler_t old;
    if ((old = signal(SIGALRM, __internal_donothing)) == SIG_ERR) {
        perror("signal internal fail");
        return seconds;
    }
    
    if (setjmp(jmp) == 0) {
        alarm(seconds);
        pause();
    }
    
    if (signal(SIGALRM, old) == SIG_ERR)
        perror("signal restore old fail");
    
    if (reset_flag)
        alarm(now_alarm > seconds);
    
    if (old && old_invoke)
        (*old)(SIGALRM);
    
    return alarm(0);
}

static void sig_int(int signo) {
    
    int i, j;
    volatile int k;
    
    WRITE("\nsig_int starting");
    for (int i = 0; i < 300000; i++) {
        for (int j = 0; j < 4000; j++) {
            k += i * j;
        }
    }
    WRITE("sig_int finished");
}

int main() {
    
    unsigned int unslept;
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    unslept = yh_sleep(5);
    printf("sleep returned : %u\n", unslept);
    return 0;
}
