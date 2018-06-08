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

#define SERV_FIFO "./serv.fifo"
#define MAX_IO_BUF_SZ 1024

int main(int argc, char *argv[]) {

    char cli_fifo_path[MAX_IO_BUF_SZ];
    snprintf(cli_fifo_path, sizeof(cli_fifo_path),  "/tmp/fifo.pid_%ld", (long)getpid());
    if (mkfifo(cli_fifo_path, 0644) < 0 && errno != EEXIST) {
        perror("client mkfifo error");
        exit(1);
    }

    int serv_fifo_write_fd = open(SERV_FIFO, O_WRONLY);
    if (serv_fifo_write_fd < 0) {
        perror("open serv write fifo error");
        exit(1);
    }
    printf("client open serv write fifo succ\n");
    
    char buf[MAX_IO_BUF_SZ],
         req_buf[MAX_IO_BUF_SZ],
         rsp_buf[MAX_IO_BUF_SZ];
    long n;
    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        snprintf(req_buf, sizeof(req_buf), "%ld %s", (long)getpid(), buf);
        n = strlen(req_buf);
        printf("request content : %s\n", req_buf);

        if (write(serv_fifo_write_fd, req_buf, n) != n) {
            perror("send request error ");
            unlink(cli_fifo_path);
            exit(1);
        }
        printf("request send succ\n");

        printf("client_fifo_path = %s\n", cli_fifo_path);
        int cli_fifo_read_fd = open(cli_fifo_path, O_RDONLY);
        if (cli_fifo_read_fd < 0) {
            perror("client open read fifo error");
            unlink(cli_fifo_path);
            exit(1);
        }
        printf("client open read fifo succ\n");

        while ((n = read(cli_fifo_read_fd, rsp_buf, sizeof(rsp_buf))) > 0) {
            if (write(STDOUT_FILENO, rsp_buf, n) != n) {
                perror("client recv response error");
                exit(1);
            }
        }
        printf("client recv response finished!\n");
    }

    if (ferror(stdin)) {
        printf("client read stdin error\n");
        exit(1);
    }
    
    return 0;    
}