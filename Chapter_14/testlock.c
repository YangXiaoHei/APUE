#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    if (argc != 3) {
        printf("you must input lock type\n");
        exit(1);
    }

    int is_rd_lock = strncmp(argv[2], "rd", 2) == 0;
    int is_wr_lock = strncmp(argv[2], "wr", 2) == 0;

    short type = is_rd_lock ? F_RDLCK : (is_wr_lock ? F_WRLCK : 0);
    if (type == 0) {
        printf("unknow lock type\n");
        exit(1);
    }

    struct flock test;
    test.l_type = type;
    test.l_whence = SEEK_SET;
    test.l_start = 0;
    test.l_len = 100;

    if (fcntl(fd, F_GETLK, &test) < 0) {
        perror("get lock fail");
        exit(1);
    }

    if (test.l_type == F_UNLCK) {
        printf("%s can be locked by %s\n", argv[1], argv[2]);
    } else if (test.l_type == F_RDLCK) {
        printf("process %ld hold a read lock for %s\n", (long)test.l_pid, argv[1]);
    } else if (test.l_type == F_WRLCK) {
        printf("process %ld hold a write lock for %s\n", (long)test.l_pid, argv[1]);
    }
    
    return 0;    
}