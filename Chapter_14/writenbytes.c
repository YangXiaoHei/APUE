#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_WRONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    if (write(fd, "123456", 6) != 6) {
        perror("write fail");
        exit(1);
    }
    
    return 0;    
}