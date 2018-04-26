#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_CREAT, 0777);
    if (fd < 0) {
        perror("open ");
    }
    
    printf("最长文件名 : %ld\n", fpathconf(fd, _PC_NAME_MAX));
    

    return 0;
}
