#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define FIFO_PATH "/tmp/yh_fifo"

int main(int argc, char *argv[]) {
    
    if (mkfifo(FIFO_PATH, 0644) < 0 && errno != EEXIST) {
        perror("mkfifo error");
        exit(1);   
    }

    int fd = open(FIFO_PATH, O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int c;
    if (write(fd, "a", 1) != 1) {
        perror("write error");
        exit(1);
    }

    if (read(fd, &c, 1) < 0) {
        perror("read error");
        exit(1);
    }
    printf("read content : %c\n", c);
    
    
    return 0;    
}