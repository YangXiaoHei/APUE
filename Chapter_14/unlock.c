#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    struct flock unlock;
    unlock.l_type = F_UNLCK;
    unlock.l_whence = SEEK_SET;
    unlock.l_start = 0;
    unlock.l_len = 100;

    if (fcntl(fd, F_SETLK, &unlock) < 0) {
        perror("unlock fail");
        exit(1);
    }

    printf("unlock succ\n");

    while (1) sleep(1);
    
    return 0;    
}