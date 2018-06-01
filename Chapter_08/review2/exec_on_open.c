#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

    int fd = open("/", O_RDONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    printf("after open \"\\\"\n");

     if (fcntl(fd, F_GETFD) == FD_CLOEXEC) {
        printf("FD_CLOEXEC setting\n");
    } else {
        printf("FD_CLOEXEC not setting\n");
    }
    close(fd);

    DIR *dr = opendir("/");
    if (dr == NULL) {
        printf("opendir error");
        exit(1);
    }

    printf("after opendir\n");
    
    if (fcntl(dr->__dd_fd, F_GETFD) == FD_CLOEXEC) {
        printf("FD_CLOEXEC setting\n");
    } else {
        printf("FD_CLOEXEC not setting\n");
    }


    return 0;    
}