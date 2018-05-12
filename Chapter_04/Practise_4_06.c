#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    /* 创建一个带空洞的文件 */
    if (argc == 2) {
        
        int fd = creat(argv[1], 0644);
        if (fd < 0) {
            perror("creat");
            exit(1);
        }
        
        char n[] = "123456789";
        for (int i = 0; i < strlen(n); ++i) {
            if (write(fd, n + i, 1) < 0) {
                perror("write");
                exit(1);
            }
            lseek(fd, 3, SEEK_CUR);
        }
        printf("创建带空洞的文件成功!\n");
        return 0;
    }
    
    if (argc != 3) {
        printf("./a.out [src_file][dst_file]\n");
        exit(1);
    }
    
    int src_fd = open(argv[1], O_RDONLY);
    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
    if (src_fd < 0 || dst_fd < 0) {
        perror("open ");
        exit(1);
    }
    
    size_t best_io_blk = 0;
    struct stat st;
    if (lstat(argv[1], &st) < 0) {
        perror("lstat");
        best_io_blk = 4096;
    } else {
        best_io_blk = st.st_blksize;
    }                                   /* 获取最佳 I/O 块大小 */
    
    size_t buf_size = best_io_blk;
    
    char *buf = malloc(buf_size);       /* 读缓冲区 */
    char *tmp = malloc(buf_size);       /* 临时缓冲区 */
    if (buf == NULL || tmp == NULL) {
        printf("malloc fail\n");
        exit(1);
    }
    
    ssize_t rl = 0, wl = 0, bytes = 0;
    while ((rl = read(src_fd, buf, buf_size)) > 0) {
        
        /* 内存逐字节拷贝 */
        bytes = 0;
        for (int i = 0; i < rl; ++i) {
            if (buf[i] != '\0') {
                tmp[bytes++] = buf[i];
            }
        }
        
        /* 只写入那些不为空的字节 */
        if ((wl = write(dst_fd, tmp, bytes)) != bytes) {
            perror("write");
            exit(1);
        }
    }
    
    if (rl < 0) {
        perror("read");
    } else {
        printf("copy success!\n");
    }
 
    free(buf);
    free(tmp);
    return 0;
}
