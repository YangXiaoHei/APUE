#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int fd[2];
    if (pipe(fd) < 0) {
        perror("pipe error");
        exit(1);
    }

/* open-only flags */
// #define O_RDONLY    0x0000      /* open for reading only */
// #define O_WRONLY    0x0001      /* open for writing only */
// #define O_RDWR      0x0002      /* open for reading and writing */
// #define O_ACCMODE   0x0003      /* mask for above modes */

    int flag1 = fcntl(fd[0], F_GETFL);
    if ((flag1 & O_WRONLY) != 1 &&
        (flag1 & O_RDWR) != 1) {
        printf("fd[0] = %d read only\n", fd[0]);
    } else if ((flag1 & O_WRONLY) == 1) {
        printf("fd[0] = %d write only\n", fd[0]);
    } else {
        printf("fd[0] = %d read/write\n", fd[0]);
    }

    int flag2 = fcntl(fd[1], F_GETFL);
    if ((flag2 & O_WRONLY) != 1 &&
        (flag2 & O_RDWR) != 1) {
        printf("fd[1] = %d read only\n", fd[1]);
    } else if ((flag2 & O_WRONLY) == 1) {
        printf("fd[1] = %d write only\n", fd[1]);
    } else {
        printf("fd[1] = %d read/write\n", fd[1]);
    }
    
    return 0;    
}