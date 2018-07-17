#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

void abort(void) {

    sigset_t mask;
    struct sigaction action;

    /*  将替 SIGABRT 注册的信号捕获函数清除掉
        保留一份原来的信号捕获设定
     */
    sigaction(SIGABRT, NULL, &action);
    if (action.sa_handler == SIG_IGN) {

        /*  如果原本是调用者想要将 SIGABRT 忽略掉，
            那么将其设置为默认动作 */
        action.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &action, NULL);
    }

    /* 如果 SIGABRT 的信号函数是默认动作，那么就清刷所有缓冲区 */
    if (action.sa_handler == SIG_DFL)
        fflush(NULL);

    /* 将当前信号屏蔽字设置为只允许 SIGABRT 通过 */
    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    /* 然后对自己发送 SIGABRT 信号 */
    kill(getpid(), SIGABRT);

    /*  如果 kill 函数返回，说明该信号被捕获或者忽略，
        此时，刷清缓冲区，然后将 SIGABRT 设置为系统
        的默认动作，然后再向自己发送 SIGABRT 信号 */
    fflush(NULL);
    action.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &action, NULL);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    kill(getpid(), SIGABRT);
    exit(1);
}

void sig_abort(int signo) {
    printf("sig_abort\n");
}

int main(int argc, char const *argv[])
{
    if (signal(SIGABRT, sig_abort) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }




    
    return 0;
}