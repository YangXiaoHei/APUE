#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void make_temp(const char *str) {
    
    int fd;
    struct stat sa;
    
    if ((fd = mkstemp(str)) < 0) {
        perror("mkstemp ");
        exit(1);
    }
    printf("temp name = %s\n", str);
    close(fd);
    
    if (lstat(str, &sa) < 0) {
        if (errno == ENOENT) {
            printf("file doesn't exist\n");
        } else {
            perror("lstat ");
            exit(1);
        }
    } else {
        printf("file exist\n");
        unlink(str);
    }
}


int main(int argc, char *argv[]) {
    
    char good_template[] = "/tmp/dirXXXXXX";
    char *bad_template = "/tmp/dirXXXXXX";
    
    printf("trying to create first tmp file ...\n");
    make_temp(good_template);
    printf("trying to create second tmp file ...\n");
    make_temp(bad_template);
    
    return 0;
}
