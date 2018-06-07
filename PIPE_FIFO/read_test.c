#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

#define FIFO_PATH "/tmp/yangxiaohei.fifo"

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

    int rc = 0;
    char buf[1024] = { 0 };
    while ((rc = read(fd, buf, sizeof(buf))) > 0) {
        printf("read content : %s\n", buf);
    }
    if (rc == 0) {
        printf("rc == 0\n");
    } else
        perror("read error");
        
    return 0;    
}