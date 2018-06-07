#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>


#define SERV_FIFO "/tmp/serv_fifo"
#define CLI_FIFO "/tmp/cli_fifo"


int main(int argc, char *argv[]) {
    
    char file_name[1024];
    if (fgets(file_name, sizeof(file_name), stdin) == NULL) {
        printf("fgets error");
        exit(1);
    }
    ssize_t len = strlen(file_name);
    if (file_name[len - 1] == '\n') 
        file_name[len - 1] = 0;

    if (mkfifo(SERV_FIFO, 0644) < 0 && errno != EEXIST) {
        perror("mkfifo error");
        exit(1);
    }
    if (mkfifo(CLI_FIFO, 0644) < 0 && errno != EEXIST) {
        unlink(SERV_FIFO);
        perror("mkfifo error");
        exit(1);
    }

    int serv_fd = open(SERV_FIFO, O_WRONLY);
    if (serv_fd < 0) {
        perror("open error");
        exit(1);
    }

    int cli_fd = open(CLI_FIFO, O_RDONLY);
    if (cli_fd < 0) {
        perror("open error");
        exit(1);
    }

    if (write(serv_fd, file_name, len) != len) {
        perror("write error");
        exit(1);
    }
    char buf[4096] = { 0 };
    ssize_t rd_len = 0;
    while ((rd_len = read(cli_fd, buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, rd_len) != rd_len) {
            perror("write error");
            exit(1);
        }
    }
    return 0;    
}