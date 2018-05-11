#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define RWRWRW 0666

int main(int argc, char *argv[]) {
    
    umask(0);
    
    if (creat("foo", RWRWRW) < 0) {
        perror("creat error for foo");
//        exit(1);
    }
    
    umask(0066);
    
    if (creat("bar", RWRWRW) < 0) {
        perror("creat error for bar");
//        exit(1);
    }

    return 0;
}
