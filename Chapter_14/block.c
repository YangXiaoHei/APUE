#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

char buf[500 << 10];

int main(int argc, char *argv[]) {
    
    ssize_t total_len = read(STDIN_FILENO, buf, sizeof(buf));
    if (total_len < 0) {
        perror("read fail\n");
        exit(1);
    }
    ssize_t ntowrite = total_len, written = 0;
    char *p = buf;

    int flag = fcntl(STDOUT_FILENO, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(STDOUT_FILENO, F_SETFL, flag);

    while (ntowrite > 0) {
        written = write(STDOUT_FILENO, buf, ntowrite);
        fprintf(stderr, "write bytes = %zd\n", written);
        
        if (written > 0) {
            ntowrite -= written;
            p += written;
        }
    }

    flag &= ~O_NONBLOCK;
    fcntl(STDOUT_FILENO, F_SETFL, flag);

    
    return 0;    
}