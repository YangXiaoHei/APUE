#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define SERV_FIFO "/tmp/serv_fifo"
#define CLI_FIFO "/tmp/cli_fifo"

int main(int argc, char *argv[]) {
    
    if (mkfifo(SERV_FIFO, 0644) < 0 && errno != EEXIST) {
        perror("mkfifo error");
        exit(1);
    }
    if (mkfifo(CLI_FIFO, 0644) < 0 && errno != EEXIST) {
        unlink(SERV_FIFO);
        perror("mkfifo error");
        exit(1);
    }

    int serv_fd = open(SERV_FIFO, O_RDONLY);
    if (serv_fd < 0) {
        perror("open error");
        exit(1);
    }
    int cli_fd = open(CLI_FIFO, O_WRONLY);
    if (cli_fd < 0) {
        perror("open error");
        exit(1);
    }

    char file_name[1024] = { 0 };
    if (read(serv_fd, file_name, sizeof(file_name)) < 0) {
        perror("read error");
        exit(1);
    }
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    char buf[4096] = { 0 };
    ssize_t len = 0;
    while ((len = read(fd, buf, sizeof(buf))) > 0) {
        if (write(cli_fd, buf, len) != len) {
            perror("write error");
            exit(1);
        }
    }
    if (len < 0) {
        perror("read error");
        exit(1);
    }
    
    
    return 0;    
}