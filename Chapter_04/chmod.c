#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    
    struct stat sa;
    
    if (stat("foo", &sa) < 0) {
        perror("stat ");
        exit(1);
    }
    
    int mode = sa.st_mode;
    for (int i = 31; i >= 0; i--) {
        printf("%2d", (mode >> i) & 1);
    }
    printf("\n");
    
    if (chmod("foo", (mode & ~S_IXGRP) | S_ISGID) < 0) {
        perror("chmod");
        exit(1);
    }
    
    if (chmod("bar", 0640) < 0) {
        perror("chmod");
        exit(1);
    }
    
    return 0;
}
