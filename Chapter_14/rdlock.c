#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

void sig_usr1(int signo) {}

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    struct sigaction new_act;
    new_act.sa_handler = sig_usr1;
    new_act.sa_flags = 0;
    new_act.sa_mask = 0;
    if (sigaction(SIGUSR1, &new_act, NULL) <  0) {
        perror("sigaction error");
        exit(1);
    }

    struct flock rd;
    rd.l_type = F_RDLCK;
    rd.l_whence = SEEK_SET;
    rd.l_start = 0;
    rd.l_len = 100;

    if (fcntl(fd, F_SETLKW, &rd) < 0) {
        perror("read lock fail");
        exit(1);
    }

    printf("read lock reqiure succ\n");

    while(1)  sleep(1);
    return 0;    
}