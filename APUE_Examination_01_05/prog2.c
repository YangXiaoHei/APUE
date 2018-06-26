#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

static int my_dup2(int oldfd, int newfd) {
    
    /* 校验 fd 是否合法 */
    long open_max = sysconf(_SC_OPEN_MAX);
    if (open_max < 0 || open_max == LONG_MAX) {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0) 
            return -1;
        if (rl.rlim_max == RLIM_INFINITY) 
            open_max = 256;
        else 
            open_max = rl.rlim_max;
    }
    if ((oldfd < 0 || oldfd >= open_max) ||
        (newfd < 0 || newfd >= open_max)) {
        errno = EBADF;
        return -1;
    }
    
    /* 如果 oldfd 没有打开，报错 */
    struct stat st;
    if (fstat(oldfd, &st) < 0) {
        errno = EBADF;
        return -1;
    }
    
    /* 如果相等，直接返回 newfd */
    if (oldfd == newfd)  
        return newfd;
    
    /* 如果 newfd 是打开的，关闭它 */
    if (fstat(newfd, &st) >= 0) 
        if (close(newfd) < 0) 
            return -1;

    /* 复制 oldfd 直到返回值等于 newfd，沿途记录每个打开的 fd */
    int *fds = malloc(open_max);
    int n = 0;
    while (1) {
        if ((fds[n] = dup(oldfd)) < 0) 
            return -1;
        if (fds[n] == newfd) 
            break;
        ++n;
    }
    /* 关闭沿途打开的所有 fd */
    for (int j = 0; j < n; ++j) 
        close(fds[j]);
    free(fds);
    return newfd;
}

int main(int argc, char *argv[]) {
    
    close(2);
    if (my_dup2(2, 5) < 0) 
        printf("my_dup2 fail 1: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 1\n");

    if (my_dup2(1, 1000) < 0) 
        printf("my_dup2 fail 2: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 2\n");

    int file_fd_1 = open("test_file_1", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (my_dup2(1, file_fd_1) < 0)
        printf("my_dup2 fail 3: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 3\n");

    const char *ptr = "hello world to stdout\n";
    write(file_fd_1, ptr, strlen(ptr));

    if (my_dup2(1, 1) < 0)
        printf("my_dup2 fail 4: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 4\n");
    
    return 0;    
}


