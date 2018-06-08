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
    
    int read_fd = open("./yanghan", O_RDONLY);
    int write_fd = open("./yanghan", O_WRONLY);

    char buf[4096];
    int n;
    while ((n = readline(read_fd, buf, sizeof(buf))) > 0) {
        printf("read content : %s\n", buf);
    } 
    
    return 0;    
}