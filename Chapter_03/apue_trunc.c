#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_TRUNC | O_RDWR, 0777);
    if (fd < 0) {
        perror("open ");
        exit(1);
    }
    
    char buf[128] = { 0 };
    ssize_t write_len = 0, read_len = 0;
    while ((read_len = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if ((write_len = write(fd, buf, strlen(buf))) < 0)  {
            perror("write ");
            exit(1);
        }
    }
    
    return 0;
}
