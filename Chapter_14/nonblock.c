#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    
    char buf[500 << 10] = { 0 };
    char *p = NULL;

    ssize_t ntowrite, written;
    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));

    int flag = fcntl(STDOUT_FILENO, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(STDOUT_FILENO, F_SETFL, flag);

    p = buf;
    while (ntowrite > 0) {
        errno = 0;
        written = write(STDOUT_FILENO, buf, ntowrite);
        fprintf(stderr, "nwrite = %zd, errno = %d\n", written, errno);

        if (written > 0) {
            p += written;
            ntowrite -= written;
        }
    }

    flag &= ~O_NONBLOCK;
    fcntl(STDOUT_FILENO, F_SETFL, flag);
    
    return 0;    
}