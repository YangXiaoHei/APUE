#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define FIFO_PATH  "/tmp/yh_fifo"

int main(int argc, char *argv[]) {
    
    if (mkfifo(FIFO_PATH, 0644) < 0 && errno != EEXIST) {
        perror("mkfifo error");
        exit(1);
    }
    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    
    
    return 0;    
}