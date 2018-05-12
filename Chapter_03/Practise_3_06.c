#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("./a.out [file_name]\n");
        exit(1);
    }
    
    int fd = -1;
    /* 如果把 O_APPEND 去掉，那么最后文件中只有一个 9 */
    if ((fd = open(argv[1], O_CREAT | O_APPEND | O_RDWR, 0644)) < 0) {
        perror("open ");
        exit(1);
    }
    
    const char *p = "0123456789";
    char *q = (char *)p;
    
    char buf[64];
    
    for (int i = 0; i < 10; i++) {
        if (write(fd, q + i, 1) != 1) {
            perror("write");
            exit(1);
        }
        
        /* 如果 lseek 起作用，那么最后文件中只有一个 9 */
        lseek(fd, -1, SEEK_CUR);
        
        bzero(buf, sizeof(buf));
        if (read(fd, buf, sizeof(buf)) < 0) {
            perror("read");
            exit(1);
        }
        
        printf("read content : %s\n", buf);
    }
    return 0;
}
