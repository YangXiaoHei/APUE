#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    
    struct rlimit rl;
    
    if (getrlimit(RLIMIT_AS, &rl) < 0) {
        printf("error\n");
        exit(1);
    }
    
    printf("进程总的可用存储空间的最大长度 %ld MB  %ld MB\n ", (long)rl.rlim_cur / 1024 / 1024, (long)rl.rlim_max / 1024 / 1024);
    
    
    
    return 0;
}
