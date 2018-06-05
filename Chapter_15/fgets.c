#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    int fd = open("./yanghan_test", O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    char buf[] = "123456";
    for (int i = 0; i < strlen(buf); i++) {
        if (write(fd, buf + i, 1) != 1) {
            perror("write error");
            exit(1);
        }
        lseek(fd, 2, SEEK_CUR);
        write(fd, "\n", 1);
    }
    lseek(fd, 0, SEEK_SET);
    close(fd);

    //@^@^@^@^@^@^@^@^@^@a

    FILE *fp = fopen("./yanghan_test", "r");
    if (fp == NULL) {
        printf("fopen error");
        exit(1);
    }
    
    char buf2[10] = { 0 };
    memset(buf2, 'x', sizeof(buf2));
    while (fgets(buf2, sizeof(buf2), fp) != NULL) {
        printf("read content : 【");
        for (int i = 0; i < sizeof(buf2); i++) {
            printf("%c", buf2[i] == 0 ? '@' : buf2[i]);
        }
        printf("】\n");
        memset(buf2, 'x', sizeof(buf2));
    }

    if (ferror(fp)) {
        printf("fgets error");
        exit(1);
    }

    
    return 0;    
}