#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void pr_cur_mask() {
    
    sigset_t old;
    int errno_save;
    
    errno_save = errno;
    if (sigprocmask(0, NULL, &old) < 0) {
        perror("sigprocmask");
        exit(1);
    } else {
        extern const char * const sys_siglist[NSIG];
        for (int i = 0; i < sizeof(sys_siglist) / sizeof(sys_siglist[0]); i++) {
            if (sigismember(&old, i + 1)) {
                printf("%s\n", sys_siglist[i]);
            } else {
                printf("没有屏蔽 %s\n", sys_siglist[i]);
            }
        }
    }
}

int main() {
    
    pr_cur_mask();
    
    
    return 0;
}
