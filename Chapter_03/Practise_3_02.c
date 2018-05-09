#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

static int my_dup2(int oldfd, int newfd) {

    /* 校验 fd 是否合法 */
    long open_max = sysconf(_SC_OPEN_MAX);
    if (open_max < 0 || open_max == LONG_MAX) {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
            printf("get file limit error");
            exit(1);
        }
        if (rl.rlim_max == RLIM_INFINITY) {
            open_max = 256;
        } else {
            open_max = rl.rlim_max;
        }
    }
    if ((oldfd < 0 || oldfd >= open_max) ||
        (newfd < 0 || newfd >= open_max)) {
        errno = EBADF;
        return -1;
    }
    
    /* 如果 oldfd 没有打开，报错 */
    if (fcntl(oldfd, F_GETFD) < 0) {
        errno = EBADF;
        return -1;
    }
    
    /* 如果相等，直接返回 newfd */
    if (oldfd == newfd) {
        return newfd;
    }
    
    /* 如果 newfd 是打开的，关闭它 */
    if (fcntl(newfd, F_GETFD) >= 0) {
        if (close(newfd) < 0) {
            return -1;
        }
    }

    /* 复制 oldfd 直到返回值等于 newfd，沿途记录每个打开的 fd */
    int *fds = malloc(open_max);
    int n = 0;
    while (1) {
        if ((fds[n] = dup(oldfd)) < 0) {
            return -1;
        }
        if (fds[n] == newfd) {
            break;
        }
        ++n;
    }
    /* 关闭沿途打开的所有 fd */
    for (int j = 0; j < n; ++j) {
        close(fds[j]);
    }
    free(fds);
    return newfd;
}

int main(int argc, char *argv[]) {
    
    int fd = open(argv[1], O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        perror("fd");
        exit(1);
    }
    
    my_dup2(fd, STDOUT_FILENO);
        
    char buf[1024] = { 0 };
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        fputs(buf, stdout);
    }

    return 0;
}
