#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    
    /* 复制源文件，目的文件 */
    int src_fd = open(argv[1], O_RDONLY);
    int dst_fd = open(argv[2], O_CREAT | O_WRONLY | O_EXCL, 0644);
    
    /* 最大迭代次数 */
//    int niterator = atoi(argv[3]);
    
    if (src_fd < 0 || dst_fd < 0) {
        perror("open ");
        exit(1);
    }
    
    ssize_t rl = 0, wl = 0;
    char buf[10] = { 0 };
    long total_wr = 0;
    errno = 0;
    
    while (/*niterator-- &&*/ (rl = read(src_fd, buf, sizeof(buf))) > 0) {
        if (write(dst_fd, buf, rl) != rl) {
            perror("write");
            exit(1);
        }
        total_wr += rl;
        printf("复制了 %ld 字节数\n", total_wr);
    }
    if (errno != 0) {
        perror("read");
        exit(1);
    }
    printf("总共复制了 %ld 字节数据\n",total_wr);
    
    return 0;
}
