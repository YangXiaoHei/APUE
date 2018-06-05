#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void another(char *argv[]) {
    int fd = creat(argv[1], 0644);
    if (fd < 0) {
        perror("creat error");
        exit(1);
    }

    struct flock read_lock;
    read_lock.l_type = F_RDLCK;
    read_lock.l_whence = SEEK_SET;
    read_lock.l_start = 0;
    read_lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &read_lock) < 0) {
        perror("read lock acqiure fail");
        exit(1);
    }
    printf("read lock acqiure succ\n");
}


int main(int argc, char *argv[]) {

    if (argc != 3) {
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror("open error");
            exit(1);
        }

        struct flock write_lock;
        write_lock.l_type = F_WRLCK;
        write_lock.l_whence = SEEK_SET;
        write_lock.l_start = 0;
        write_lock.l_len = 0;
        if (fcntl(fd, F_SETLK, &write_lock) < 0) {
            perror("write lock acqiure fail");
            exit(1);
        }
        printf("write lock acqiure succ\n");
    } else {
        another(argv);
    }
    
    return 0;    
}