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

#define SERV_FIFO "/tmp/serv.fifo"

int main(int argc, char *argv[]) {
    
    pid_t pid = getpid();

    char cli_fifo[1024] = { 0 };
    snprintf(cli_fifo, sizeof(cli_fifo), "/tmp/%ld.fifo", (long)pid);

    if (mkfifo(cli_fifo, 0644) < 0 && errno != EEXIST) {
        perror("mkfifo error");
        exit(1);
    }

    int serv_fd = open(SERV_FIFO, O_WRONLY);
    if (serv_fd < 0) {
        perror("open error");
        exit(1);
    }
    printf("client open serv_fifo succ\n");


    char buf[1024] = { 0 };
    snprintf(buf, sizeof(buf), "%ld ", (long)pid);
    ssize_t n = strlen(buf);
    if (fgets(buf + n, sizeof(buf) - n, stdin) != NULL) {
        n = strlen(buf);
        if (write(serv_fd, buf, n) != n) {
            perror("write error");
            exit(1);
        }

        printf("client send content : %s\n", buf);

        int read_fd = open(cli_fifo, O_RDONLY);
        if (read_fd < 0) {
            perror("open error");
            exit(1);
        }
        printf("client open cli_fifo succ\n");

        while ((n = read(read_fd, buf, sizeof(buf))) > 0) {
            if (write(STDOUT_FILENO, buf, n) != n) {
                perror("write error");
                exit(1);
            }
        }
        printf("client finished\n");
    }

    return 0;    
}