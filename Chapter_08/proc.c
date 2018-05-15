#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        sleep(2);
        exit(1);
    }
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        sleep(4);
        abort();
    }
    
    if ((pid = fork()) < 0)  {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        if (execl("/bin/date", "date",NULL) < 0) {
            printf("execl /bin/date fail\n");
        }
        exit(7);
    }
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        sleep(8);
        exit(0);
    }
    
    sleep(6);
    kill(getpid(), SIGKILL);
    
    return 0;
}
