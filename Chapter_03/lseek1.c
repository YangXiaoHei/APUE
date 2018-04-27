#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc,  char *argv[]) {
    
    // hole
    int hole_fd = open(argv[1], O_RDWR | O_CREAT, 0755);
    if (hole_fd < 0) {
        perror("open ");
        exit(1);
    }
    
    // non_hole
    int non_hole_fd = open(argv[2], O_RDWR | O_CREAT, 0755);
    if (non_hole_fd < 0) {
        perror("open ");
        exit(1);
    }
    
    char buf[1000] = { 0 };
    for (int i = 0; i < 1000; i++) {
        buf[i] = '0' + i;
    }
    
    // hole
    off_t new_off = lseek(hole_fd, 1000, SEEK_END);
    if (new_off == -1) {
        perror("lseek ");
        exit(1);
    }
    if (write(hole_fd, buf, sizeof(buf)) < 0) {
        perror("write");
        exit(1);
    }
    
    // non_hole
    if (write(non_hole_fd, buf, sizeof(buf)) < 0) {
        perror("write");
        exit(1);
    }
    if (write(non_hole_fd, buf, sizeof(buf)) < 0) {
        perror("write");
        exit(1);
    }
    
    close(hole_fd);
    close(non_hole_fd);
    
    return 0;
}
