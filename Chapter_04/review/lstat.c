#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    
    char buf[1024] = { 0 };
    printf("%s\n", getcwd(buf, sizeof(buf)));
    
    struct stat sa;
    if (lstat("myftw.c", &sa) < 0) {
        perror("lstat error");
        exit(1);
    }
    
    
    return 0;
}
