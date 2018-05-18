#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("./a.out ttyname\n");
        exit(0);
    }
    
    int fd = open(argv[1], O_RDWR);    
 
    if (fd < 0) {
        perror("open ");
        exit(0);
    }
    if (write(fd, argv[2], strlen(argv[2])) != strlen(argv[2])) {
        perror("write");
        exit(0);
    }
    return 0;
}
