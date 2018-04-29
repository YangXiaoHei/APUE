#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 4096


/*
 
 测试文件 ：【傷物語 冷血篇】Kizumonogatari ~03  888 MB
 测试环境 ：MacBook Pro 10.13.4
 
 每次写完都 fsync()
 YangHan:Chapter_03 YangHan$ time -p ./write_sync_test ~/Downloads/src_file.mp4  ./dst.mp4
 复制成功!
 real 19.07
 user 0.36
 sys 13.02
 
 
 在最后调一次 fsync()
 YangHan:Chapter_03 YangHan$ time -p ./write_sync_test ~/Downloads/src_file.mp4  ./dst.mp4
 复制成功!
 real 3.05
 user 0.19
 sys 2.59
 
 
 打开文件时设置 O_SYNC
 YangHan:Chapter_03 YangHan$ time -p ./write_sync_test ~/Downloads/src_file.mp4  ./dst.mp4
 复制成功!
 real 15.98
 user 0.23
 sys 10.08
 
 
 */

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        printf("./write_snyc_test.c [src_file_name][dst_file_name]\n");
        exit(1);
    }
    
    if (access(argv[1], F_OK) == -1) {
        printf("源文件不存在\n");
        exit(1);
    }
    
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("open ");
        exit(1);
    }
    
    int dst_fd = open(argv[2], O_CREAT | O_WRONLY | O_SYNC, 0755);
    if (dst_fd < 0) {
        perror("open ");
        exit(1);
    }
    
    char buf[BUF_SIZE] = { 0 };
    
    ssize_t rd_len = 0, wr_len = 0;
    while ((rd_len = read(src_fd, buf, sizeof(buf))) > 0) {
        if ((wr_len = write(dst_fd, buf, rd_len)) != rd_len) {
            perror("write ");
            exit(1);
        }
        // fsync(dst_fd)
    }
//    fsync(dst_fd);
    
    if (rd_len == 0) {
        printf("复制成功!\n");
    } else if (rd_len < 0) {
        perror("read ");
        exit(1);
    }
    
    close(src_fd);
    close(dst_fd);
    
    return 0;
}
