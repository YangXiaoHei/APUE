#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int globvar = 88;

int main() {
    
    int var = 7;
    pid_t pid;
    
    printf("before vfork\n");
    
    if ((pid = vfork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        globvar++;
        var++;
//        _exit(0);  /* _exit() 并不执行标准 I/O 缓冲区的冲洗操作 */
        exit(0);  /* 在我的电脑上，exit() 看起来并没有关闭标准输出文件描述符 */
    }
    
    printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globvar, var);
    
    return 0;
}
