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
#define MAX_IO_BUF_SZ 4096

/*
 *  @param  fd      用于读的文件描述符
 *  @param  ptr     取预读内容的指针
 *  @return 错误返回 -1，EOF 返回 0
 */
ssize_t read_helper(int fd, char *ptr) {
    static char buf[MAX_IO_BUF_SZ];
    static long read_cnt = 0;
    static char *read_ptr = NULL;
    if (read_cnt <= 0) {
try_again:
        if ((read_cnt = read(fd, buf, sizeof(buf))) < 0) {
            if (errno == EINTR) 
                goto try_again;
            return -1;
        } else if (read_cnt == 0)
            return 0;
        read_ptr = buf;
    }
    *ptr = *read_ptr++;
    read_cnt--;
    return 1;
}


 // *  @param fd       用于读数据的文件描述符
 // *  @param buf      用于存储读到的数据
 // *  @param nbytes   一次最多读取的字节数
 // *  @return 成功返回实际读到的字节数，可能少于 nbytes，比如遇到 '\n' ，或者本身就
 // *          没那么多数据可读，错误返回 -1
 
ssize_t readline(int fd, char *buf, long nbytes) {
    int n = 0; char c = 0; long rc = 0;
    for (n = 1; n < nbytes; n++) {
        if ((rc = read_helper(fd, &c)) == 1) {
            *buf++ = c;
            if (c == '\n') break;
        } else if (rc == 0) {
            if (n != 1) break;
            return 0;
        } else
            return -1;
    }
    *buf = 0;
    return n;
}

int main(int argc, char *argv[]) {
    
    // if (mkfifo(SERV_FIFO, 0644) < 0 && (errno != EEXIST)) {
    //     perror("serv mkfifo fifo error");
    //     exit(1);
    // }

    int serv_fifo_read_fd = open(SERV_FIFO, O_RDONLY);
    // if (serv_fifo_read_fd < 0) {
    //     perror("serv open read fifo error");
    //     exit(1);
    // }
    // printf("serv open read fifo succ\n");

    int serv_fifo_write_fd = open(SERV_FIFO, O_WRONLY);
    // if (serv_fifo_write_fd < 0) {
    //     perror("serv open write fifo error");
    //     exit(1);
    // }
    // printf("serv open write fifo succ\n");

    long n; 
    char req_buf[4096];
    // char cli_fifo_path[MAX_IO_BUF_SZ];
    // char rsp_buf[MAX_IO_BUF_SZ];
    while ((n = readline(serv_fifo_read_fd, req_buf, sizeof(req_buf))) > 0) {
        // if (req_buf[n - 1] == '\n') 
        //     n--;
        // req_buf[n] = 0;

        printf("serv recv request : %s\n", req_buf);

        // char *handle_file_ptr = strchr(req_buf, ' ');
        // if (handle_file_ptr == NULL) {
        //     printf("invalid request : %s\n", req_buf);
        //     continue;
        // }
        // *handle_file_ptr++ = 0;
        // printf("handle file path is : %s\n", handle_file_ptr);

        // pid_t pid = atol(req_buf);
        // if (pid == 0) {
        //     printf("invalid request : %s\n", req_buf);
        //     continue;
        // }

        // snprintf(cli_fifo_path, sizeof(req_buf), "/tmp/fifo.pid_%ld", (long)pid);
        // printf("client_fifo_path = %s\n", cli_fifo_path);
        // int cli_fifo_write_fd = open(cli_fifo_path, O_WRONLY);
        // if (cli_fifo_write_fd < 0) {
        //     printf("%s cannot open : %s\n", cli_fifo_path, strerror(errno));
        //     continue;
        // }
        // printf("serv open client write fifo succ\n");

        // int fd = open(handle_file_ptr, O_RDONLY);
        // if (fd < 0) {
        //     snprintf(rsp_buf, sizeof(rsp_buf), 
        //         "cannot open %s for request : %s\n",
        //         handle_file_ptr, req_buf);
        //     n = strlen(rsp_buf);
        //     if (write(cli_fifo_write_fd, rsp_buf, n) != n) 
        //         printf("handle request : %s fail : %s\n", req_buf, strerror(errno));
        //     close(cli_fifo_write_fd);
        // } else {
        //     while (( n = read(fd, rsp_buf, sizeof(rsp_buf))) > 0) {
        //         if (write(cli_fifo_write_fd, rsp_buf, n) != n) {
        //             printf("handle request %s fail : %s\n", req_buf, strerror(errno));
        //             break;
        //         }
        //     }
        //     printf("server handle request finished!\n");
        //     close(cli_fifo_write_fd);
        //     close(fd);
        // }   
    }
    // if (n < 0) {
    //     printf("read req_buf fail : %s\n", strerror(errno));
    //     unlink(SERV_FIFO);
    // } else {
    //     printf("unexpected procedure\n");
    //     unlink(SERV_FIFO);
    //     abort();
    // }
    return 0;    
}