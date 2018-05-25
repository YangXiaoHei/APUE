#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

static void LOG(int fd, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    char buf[1024] = { 0 };
    vsnprintf(buf, sizeof(buf), format, vl);
    write(fd, buf, strlen(buf));
    va_end(vl);
}

#ifdef MAXLINE
#undef MAXLINE
#endif
#define MAXLINE 4

int main(int argc, char *argv[]) {
    
    int log_fd = open("/dev/ttys000", O_RDWR);
    LOG(log_fd, "open \"/dev/ttys000\" on file descriptor %d\n", log_fd);
    if (argc != 1) {
        LOG(log_fd, "./a.out < [src_file] > [dst_file]\n");
        _exit(1);
    }
    if (log_fd < 0)
        _exit(1);
    
    char buf[MAXLINE];
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        LOG(log_fd, "read content : 【%s】", buf);
        if (fputs(buf, stdout) == EOF) {
            LOG(log_fd, "output error");
            _exit(3);
        } else {
            LOG(log_fd, "write succ\n");
        }
    }
    
    fflush(stdout);
    
    if (ferror(stdin)) {
        LOG(log_fd, "input error");
        _exit(4);
    }
    _exit(0);
}
