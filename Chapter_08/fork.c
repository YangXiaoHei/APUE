#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int globvar = 6;
char buf[] = "a write to stdout\n";

int main() {
    
    int var;
    pid_t pid;
    
    var = 88;
    
    /* 不带缓冲写 */
    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1) {
        perror("write");
        exit(1);
    }
    
    /* 缓冲区中存储了字符串，如果是标准输出链接到终端，那么行缓冲，会被冲洗
     如果重定向了标准输出，默认是全缓冲，则子进程会拷贝该缓冲区副本 */
    printf("before fork\n");
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        globvar++;
        var++;
    } else {
        sleep(2);
    }
    printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globvar, var);
    return 0;
}
