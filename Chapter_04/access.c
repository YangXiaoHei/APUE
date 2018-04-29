#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    printf("当前进程有效用户 ID : %d  有效组 ID : %d\n", getuid(), getgid());
    
    if (access(argv[1], R_OK) < 0) {
        perror("access error");
//        exit(1);
    } else {
        printf("read access OK!\n");
    }
    
    if (open(argv[1], O_RDONLY) < 0) {
        perror("open error");
//        exit(1);
    } else {
        printf("read open OK!\n");
    }

    return 0;
}
