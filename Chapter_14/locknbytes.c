#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_WRONLY);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    int len = (argc != 3) ? 1 : atoi(argv[2]);

    struct flock write_lock;
    write_lock.l_type = F_WRLCK;
    write_lock.l_whence = SEEK_SET;
    write_lock.l_start = 0;
    write_lock.l_len = len;
    
    if (fcntl(fd, F_SETLK, &write_lock) < 0) {
        perror("write lock fail\n");
        exit(1);
    }
    printf("%s hold a write lock\n", argv[1]);
    
    return 0;    
}