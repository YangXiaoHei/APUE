#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void dup_test() {
    
    int newfd = dup(STDOUT_FILENO);
    if (newfd < 0) {
        perror("dup ");
        exit(1);
    }
    
    if (write(newfd, "hello world\n", strlen("hello world\n")) < 0) {
        perror("write ");
        exit(1);
    }
}

void dup2_test(const char *path_name) {
    int fd = open(path_name, O_CREAT | O_RDWR, 0755);
    if (fd < 0) {
        perror("open ");
        exit(1);
    }
    
    // 现在 fd 和 STDOUT_FILENO 都指向 fd 的文件表项，共享相同的文件状态信息，
    // 文件偏移量，v 节点指针
    // 本来 shell 向标准输出写数据，会将信息写入到用于屏幕显示的文件中，
    // 但现在通过 STDOUT_FILENO 写，则会写到 fd 的文件中
    /*
     
     fd 0
     fd 1   ----> [                    ]
     fd 2         [  argv[1]打开的文件   ]
     fd 3   ----> [                    ]
     
     
     */
    int newfd = dup2(fd, STDOUT_FILENO);
    if (newfd < 0) {
        perror("dup2 ");
        exit(1);
    }
    
    // 这里 newfd == STDOUT_FILENO
    // 并且 printf() 里的这句话，也会写入到原 fd 指向的文件中
    // 因为 printf() 是将内容写到 STDOUT_FILENO 中的函数
    printf("新的指向原打开文件的文件描述符为 : %d\n", newfd);
    
    if (write(STDOUT_FILENO, "1234\n", 5) < 0) {
        perror("write ");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    
    dup_test();
    
    
    return 0;
}
