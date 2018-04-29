#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void set_fl(int fd, int flags) {
    
    int flag = fcntl(fd, F_GETFL);
    if (flag < 0) {
        perror("fcntl ");
        exit(1);
    }
    
    flag |= flags;
    
    if (fcntl(fd, F_SETFL, flag) < 0) {
        perror("fcntl ");
        exit(1);
    }
}
