#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {

    /* ./executable_file file_name start len */
    if (argc != 4) {
        printf("usage : %s file_name start len\n", argv[0]);
        exit(1);
    }
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    struct flock write_lock;
    write_lock.l_type = F_WRLCK;
    write_lock.l_whence = SEEK_SET;
    write_lock.l_start = atoi(argv[2]);
    write_lock.l_len = atoi(argv[3]);

    if (fcntl(fd, F_SETLK, &write_lock) < 0) {
        printf("%s reqiure write lock fail : %s", argv[1], strerror(errno));
        exit(1);
    }
    printf("%s reqiure write lock succ\n", argv[1]);

    while(1) sleep(1);
    
    return 0;    
}