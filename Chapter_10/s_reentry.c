#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
#include <pwd.h>

static void my_alarm(int signo) {
    static int cnt = 0;
    struct passwd *rootptr;
    printf("recevied signal %d [seqno = %d]\n", signo, cnt++);
    if ((rootptr = getpwnam("root")) == NULL) {
        printf("getpwnam err\n");
        exit(1);
    }
    alarm(1);
}


int main() {
    
    struct passwd *ptr;
    
    setbuf(stdout, NULL);
    signal(SIGALRM, my_alarm);
    
    alarm(1);
    
    while (1) {
        if ((ptr = getpwnam("root")) == NULL) {
            printf("getpwnam error");
            exit(1);
        }
        if (strcmp(ptr->pw_name, "root") != 0) {
            printf("return value corrupted!, pw_name = %s\n",
                   ptr->pw_name);
        }
    }
    
    return 0;
}
