

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

jmp_buf jmp;
void __internal_donothing(int signo) {
    longjmp(jmp, 1);
}

ssize_t read_untill(int fd,
                    char *buf,
                    size_t buf_len,
                    unsigned int seconds) {
    if (seconds > 0) {
        typedef void (*__sig_handler_t) (int);
        __sig_handler_t old;
        if ((old = signal(SIGALRM, __internal_donothing)) == SIG_ERR) {
            perror("signal error");
            exit(1);
        }
        ssize_t rl;
        if (setjmp(jmp) == 0) {
            alarm(seconds);
            rl = read(fd, buf, buf_len);
            alarm(0);
        }
        if (signal(SIGALRM, old) == SIG_ERR) {
            perror("signal error");
        }
        return rl;
    }
    return read(fd, buf, buf_len);
}

int main() {
    
    char buf[1024];
    if (read_untill(STDOUT_FILENO, buf, sizeof(buf), 3) < 0) {
        perror("read_untill");
        exit(1);
    }
    printf("%s\n", buf);
    return 0;
}
