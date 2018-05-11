#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    umask(0);
    
    /* 创建文件，并以读写打开，设置文件模式字为 -w- --- --- */
    int fd = open(argv[1], O_CREAT | O_RDWR, 0200);
    if (fd < 0) {
        perror("open ");
        exit(1);
    }
    
    /* 写数据 */
    if (write(fd, "12345", 5) != 5) {
        perror("write");
        exit(1);
    }
    lseek(fd, 0, SEEK_SET);
    
    /* 读刚才写的数据 */
    char buf[1024] = { 0 };
    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read");
        exit(1);
    }
    printf("read content : %s\n", buf);
    
    return 0;
}
