#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sig_int(int signo) {
    printf("sig_int %d\n", signo);
}

int main(int argc, char *argv[]) {
    
    setbuf(stdout, NULL);
    
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
        exit(1);
    }
    
    printf("begin to sleep \n");
    
    unsigned int unslept = sleep(10);
    
    printf("end sleep \n");
    
    printf("unslept %d\n", unslept);
    
    return 0;
}
