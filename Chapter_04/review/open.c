#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1],O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        perror("open fail");
        exit(1);
    }
    
    char msg[] = "I am king of the world!";
    if (write(fd, msg, strlen(msg)) != strlen(msg)) {
        perror("write error");
        exit(1);
    }
    lseek(fd, SEEK_SET, 0);
    
    char buf[1024] = { 0 };
    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read error");
        exit(1);
    }
    
    printf("read content : %s\n", buf);
    
    
    return 0;
}
