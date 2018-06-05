#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {

    /* ./executable_file file_name start len */
    if (argc != 5) {
        printf("usage : %s file_name rd|wr start len\n", argv[0]);
        exit(1);
    }
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    short type;
    if (strncmp(argv[2], "wr", 2) == 0) {
        type = F_WRLCK;
    } else if (strncmp(argv[2], "rd", 2) == 0) {
        type = F_RDLCK;
    } else {
        printf("unexpected lock type\n");
        exit(1);
    }

    struct flock test_lock;
    test_lock.l_type = type;
    test_lock.l_whence = SEEK_SET;
    test_lock.l_start = atoi(argv[2]);
    test_lock.l_len = atoi(argv[3]);

    if (fcntl(fd, F_GETLK, &test_lock) < 0) {
        printf("%s get lock fail : %s\n", argv[1], strerror(errno));
        exit(1);
    }

    if (type == F_WRLCK) {
        if (test_lock.l_type == F_UNLCK) {
            printf("%s can reqiure wr lock\n", argv[1]);
        } else {
            printf("%s has locked by proc %ld", argv[1], (long)test_lock.l_pid);
        }
    } else {
        if (test_lock.l_type == F_UNLCK) {
            printf("%s can reqiure rd lock\n", argv[1]);
        } else {
            printf("%s has locked by proc %ld", argv[1], (long)test_lock.l_pid);
        }
    }
    
    return 0;    
}