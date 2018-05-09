#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    
    int ret = 0;
    
    ret = dup2(atoi(argv[1]), atoi(argv[2]));
    printf("ret = %d errno = %d\n", ret, errno);
    
    
    return 0;
}
