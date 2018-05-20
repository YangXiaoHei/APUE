#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

#define WRITE(_msg_) \
write(STDOUT_FILENO, _msg_"\n", sizeof(_msg_))

jmp_buf jmp;
void __internal_longjmp(int signo) {
    WRITE("internal signal handler");
    longjmp(jmp, 1);
}
unsigned int yh_sleep(unsigned int seconds) {
    
    /* 用于内部唤醒 */
    typedef void (*__sig_handler_t) (int);
    __sig_handler_t old;
    if ((old = signal(SIGALRM, __internal_longjmp)) == SIG_ERR) {
        perror("signal() registe internal handler error");
        return seconds;
    }
    
    /* 对上下文的处理 */
    unsigned int reset_flag = 0,
                 now_alarm  = 0,
                 unslept    = 0,
                 early      = 0;
    if ((now_alarm = alarm(0)) != 0) {
        if (now_alarm > seconds) {
            reset_flag = 1;
        } else {
            early = 1;
            seconds = now_alarm;
        }
    }
    
    /* 只有非 longjmp 进入才挂起 */
    if (setjmp(jmp) == 0) {
        alarm(seconds);
        pause();
    }
    
    /* 重置 alarm */
    unslept = alarm(0);
    if (reset_flag)
        alarm(now_alarm - seconds);
    
    /* 恢复旧的 handler */
    if (signal(SIGALRM, old) == SIG_ERR)
        perror("signal() restore old handler error");
    
    /* 如果是提前唤醒的 */
    if (early)
        (*old)(SIGALRM);
    
    return unslept;
}

void caller_register_handler(int signo) {
    WRITE("caller register handler");
}

int main() {
    
    if (signal(SIGALRM, caller_register_handler) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    alarm(4);
    
    yh_sleep(7);
    
    while (1);
    
    return 0;
}
