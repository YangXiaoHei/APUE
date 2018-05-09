#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

static int my_dup2(int oldfd, int newfd, long *line_no) {

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
        *line_no = __LINE__;
        errno = EBADF;
        return -1;
    }
    
    /* 如果 oldfd 没有打开，报错 */
    if (fcntl(oldfd, F_GETFD) < 0) {
        errno = EBADF;
        *line_no = __LINE__;
        return -1;
    }
    
    /* 如果相等，直接返回 newfd */
    if (oldfd == newfd) {
        return newfd;
    }
    
    /* 如果 newfd 是打开的，关闭它 */
    if (fcntl(newfd, F_GETFD) >= 0) {
        if (close(newfd) < 0) {
            *line_no = __LINE__;
            return -1;
        }
    }
    
    /*
      如果 newfd 更小，
      那么关闭 newfd 后，
      dup(oldfd) 返回的一定是 newfd
     */
    if (newfd < oldfd) {
        printf("这里\n");
        if (dup(newfd) < 0) {
            *line_no = __LINE__;
            return -1;
        }
        return newfd;
    }
    
    printf("到了这里\n");

    /* 复制 oldfd 直到返回值等于 newfd，沿途记录每个打开的 fd */
    int *fds = malloc(open_max);
    int n = 0;
    while (1) {
        if ((fds[n] = dup(oldfd)) < 0) {
            *line_no = __LINE__;
            return -1;
        }
        if (fds[n] == newfd) {
            break;
        }
        ++n;
    }
    /* 关闭沿途打开的所有 fd */
    for (int j = 0; j < n; ++j) {
        if (close(fds[j]) < 0) {
            *line_no = __LINE__;
        }
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
    
    long line = 0;
    if (my_dup2(fd, STDOUT_FILENO, &line) < 0) {
        printf("line = %d\n", line);
    }
    
//    if (dup2(fd, STDOUT_FILENO) < 0) {
//        perror("dup2");
//        exit(1);
//    }
    
    char buf[1024] = { 0 };
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        fputs(buf, stdout);
    }

    return 0;
}
