#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    umask(0);
    
    int fd = open(argv[1], O_RDWR | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    printf("向文件写数据 : 123456\n");
    if (write(fd, "123456", 6) != 6) {
        perror("write ");
        exit(1);
    }
    
    while (1) {
        
    }
    
//    fsync(fd);
    
    lseek(fd, 0, SEEK_SET);
    char buf[1024] = { 0 };
    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read");
        exit(1);
    }
    
    printf("当前文件内容是 : %s\n", buf);
    
    printf("将文件偏移量往回挪动 3 位\n");
    if (lseek(fd, -3, SEEK_CUR) < 0) {
        perror("lseek");
        exit(1);
    }
    
    printf("向文件写数据 : ABC\n");
    if (write(fd, "ABC", 3) != 3) {
        perror("write ");
        exit(1);
    }
    
//    fsync(fd);
    
    bzero(buf, sizeof(buf));
    lseek(fd, 0, SEEK_SET);
    if (read(fd, buf, sizeof(buf)) < 0) {
        perror("read");
        exit(1);
    }
    
    printf("当前文件内容是 : %s\n", buf);
    
    return 0;
}
