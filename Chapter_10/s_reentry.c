#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
#include <pwd.h>

#define WRITE(_msg_) \
write(STDOUT_FILENO, _msg_"\n", sizeof(_msg_));

static void my_alarm(int signo) {
    static int cnt = 0;
    struct passwd *rootptr;
    WRITE("recv signal");
    if ((rootptr = getpwnam("root")) == NULL) {
        WRITE("sig_handler getpwnam error");
        exit(1);
    }
}


int main() {
    
    struct passwd *ptr;
    signal(SIGALRM, my_alarm);
    
    while (1) {
        if ((ptr = getpwnam("root")) == NULL) {
            WRITE("getpwnam error");
            exit(1);
        }
        if (strcmp(ptr->pw_name, "root") != 0) {
            WRITE("strcmp error");
        }
    }
    
    return 0;
}
