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

ssize_t read_for_less_syscall(int fd, char *ptr) {

    static ssize_t read_cnt = 0; /* 外层取完还剩下多少 */
    static char *read_ptr = NULL;  /* 给外层取预读内容的指针 */
    static char buf[4096];  /* 预读缓冲区 */

    if (read_cnt <= 0) {
again:
        if ((read_cnt = read(fd, buf, sizeof(buf))) < 0) {
            if (errno == EINTR) /* 如果被信号中断，just retry */
                goto again;
            return -1;
        } else if (read_cnt == 0) /* 没数据啦～ */
            return 0;
        read_ptr = buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}

ssize_t readline(int fd, char *buf, ssize_t maxlen) {
    char c = 0; ssize_t rc = 0;
    char *ptr = buf;
    for (int i = 1; i < maxlen; i++) {
        if ((rc = read_for_less_syscall(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n') 
                break;
        } else if (rc == 0) {
            if (i == 1)  /* 没得读了 */
                return 0;
            else  /* 读了一些读到头 */
                break;
        } else
            return -1;
    }
    *ptr = 0;
    return 0;
}

#define SERV_FIFO "/tmp/serv.fifo"

int main(int argc, char *argv[]) {
    
    if (mkfifo(SERV_FIFO, 0644) < 0 && (errno != EEXIST)) {
        perror("mkfifo error");
        exit(1);
    }

    int read_fd = open(SERV_FIFO, O_RDONLY);
    if (read_fd < 0) {
        perror("open error");
        exit(1);
    }
    int dummy_fd = open(SERV_FIFO, O_WRONLY);
    if (dummy_fd < 0) {
        perror("open error");
        exit(1);
    }

    ssize_t n = 0;
    char buf[1024] = { 0 };
    char fifo_name[1024] = { 0 };
    char *ptr = NULL;
    int write_fd, fd;
    pid_t pid;
    while ((n = readline(read_fd, buf, sizeof(buf))) > 0) {
        if (buf[n - 1] == '\n') 
            n--;
        buf[n] = 0;

        if ((ptr = strchr(buf, ' ')) == NULL) {
            printf("request error\n");
            continue;
        }

        *ptr++ = 0;
        pid = atol(buf);
        snprintf(fifo_name, sizeof(fifo_name), "/tmp/%ld.fifo", (long)pid);
        if ((write_fd = open(fifo_name, O_WRONLY)) < 0) {
            printf("can't open %s : %s\n", fifo_name, strerror(errno));
            continue;
        }

        if ((fd = open(ptr, O_RDONLY)) < 0) {
            snprintf(buf + n, sizeof(buf) - n, ": can't open, %s\n", strerror(errno));
            n = strlen(buf);
            if (write(write_fd, buf, n) != n) {
                perror("send err fail");
                exit(1);
            }
            close(write_fd);
        } else {

            while ((n = read(fd, buf, sizeof(buf))) > 0) {
                if (write(write_fd, buf, n) != n) {
                    perror("send back data fail");
                    exit(1);
                }
            }
        }
    }
    
    return 0;    
}