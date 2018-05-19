#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <signal.h>
#include <string.h>
#include <pwd.h>

static void my_alarm(int signo) {
    static int cnt = 0;
    printf("recevied signal %d [seqno = %d]\n", signo, cnt++);
}


int main() {
    
    setbuf(stdout, NULL);
    signal(SIGALRM, my_alarm);
    
    while (1);
    
    return 0;
}
