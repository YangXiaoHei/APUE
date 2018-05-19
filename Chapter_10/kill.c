#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    
    pid_t pid = atoi(argv[1]);
    
    setbuf(stdout, NULL);
    
    int cnt = 0;
    
    while (1) {
        printf("send SIGALRM  [seqno = %d]\n", cnt++);
        kill(pid, SIGALRM);
        usleep(1000 * 100);
    }
    
    
    return 0;
}
