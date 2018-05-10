#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#define LOG_PATH "./my_dup2.log"

static int my_dup2(int oldfd, int newfd) {
    
    /* 因为可能会重定向标准输出，所以创建一个文件用来打错误日志 */
    int log_on = 0;
    int log_fd = open(LOG_PATH, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (log_fd < 0) {
        printf("my_dup2 log create fail");
    } else {
        log_on = 1;
    }

    char buf[1024] = { 0 };
#ifdef LOG
#error "LOG Macro has already be defined!"
#else
#define LOG(_fmt_, ...) \
do {    \
    if (log_on) {   \
        snprintf(buf, sizeof(buf), _fmt_, ##__VA_ARGS__);   \
        if (buf[strlen(buf) - 1] == '\0') { \
            buf[strlen(buf) - 1] = '\n';    \
        }   \
        write(log_fd, buf, strlen(buf)); \
        bzero(buf, sizeof(buf));    \
    }   \
} while (0)
#endif

    /* 校验 fd 是否合法 */
    long open_max = sysconf(_SC_OPEN_MAX);
    if (open_max < 0 || open_max == LONG_MAX) {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
            LOG("get file limit error");
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
        LOG("fd %d or fd %d beyond valid range", oldfd, newfd);
        errno = EBADF;
        return -1;
    }
    
    /* 如果 oldfd 没有打开，报错 */
    if (fcntl(oldfd, F_GETFD) < 0) {
        errno = EBADF;
        LOG("oldfd %d not open", oldfd);
        return -1;
    }
    
    /* 如果相等，直接返回 newfd */
    if (oldfd == newfd) {
        LOG("oldfd == newfd");
        return newfd;
    }
    
    /* 如果 newfd 是打开的，关闭它 */
    if (fcntl(newfd, F_GETFD) >= 0) {
        if (close(newfd) < 0) {
            LOG("newfd %d close fail", newfd);
            return -1;
        }
    }

    /* 复制 oldfd 直到返回值等于 newfd，沿途记录每个打开的 fd */
    int *fds = malloc(open_max);
    int n = 0;
    while (1) {
        if ((fds[n] = dup(oldfd)) < 0) {
            LOG("oldfd %d dup fail", oldfd);
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
            LOG("extra dupped fd %d close fail", fds[j]);
        }
    }
    free(fds);
#undef LOG
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
