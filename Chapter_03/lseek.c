
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open ");
        exit(1);
    }
    
    off_t off = lseek(fd, 0, SEEK_CUR);
    printf("当前文件偏移量为 : %d\n", off);
    
    printf("从头向前偏移 5 个字节\n");
    off = lseek(fd, 5, SEEK_SET);
    printf("当前文件偏移量为 : %d\n", off);
    
    printf("写入\n");
    char buf[128] = { 0 };
    scanf("%s", buf);
    
    ssize_t l = strlen(buf);
    buf[l] = '\n';
    
    ssize_t wl = write(fd, buf, strlen(buf));
    if (wl < 0) {
        perror("write ");
        exit(1);
    }
    
    printf("此时文件内容为 : \n");
    ssize_t rl = 0;
    while ((rl = read(fd, buf, sizeof(buf))) > 0) {
        buf[rl - 1] = 0;
        printf("%s", buf);
    }
    
    off = lseek(fd, 0, SEEK_END);
    printf("当前文件偏移量为 : %d\n", off);
    
    printf("写入 YANGHAN\n");
    write(fd, "YANGHAN\n", 8);
    
    close(fd);
    
    return 0;
}
