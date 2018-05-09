#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    
    umask(0);
    
    int fd = open(argv[1], O_CREAT | O_EXCL | O_WRONLY, 0444);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    if (write(fd, "123", 3) != 3) {
        perror("write");
        exit(1);
    }

    
    return 0;
}
