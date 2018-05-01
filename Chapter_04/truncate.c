#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    if (argc != 4) {
        printf("useage : ./truncate [new_file][new_file_size][truncated_length]\n");
        exit(1);
    }
    
    int fd = 0;
//    if ((fd = creat(argv[1], 0740)) < 0) {  // 以只写的方式打开
    if ((fd = open(argv[1], O_CREAT | O_EXCL | O_RDWR, 0740)) < 0) {
        
        perror("open ");
        exit(1);
    }
    printf("fd = %d\n", fd);
    
    char *alphbets = "abcdefghijklmnopqrstuvwxyz";
    char *p = alphbets;
    int i = 0;
    int new_size = atoi(argv[2]);
    while (new_size--) {
        if (write(fd, p + (i++ % strlen(alphbets)), 1) < 0) {
            perror("write ");
            exit(1);
        }
    }
    
    int tl = atoi(argv[3]);
    // ftruncated(fd, tl) < 0
    if (truncate(argv[1], tl) < 0) {
        perror("truncate ");
        exit(1);
    }
    return 0;
}
