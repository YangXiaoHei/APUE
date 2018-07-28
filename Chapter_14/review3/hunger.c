#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif


int req_wr_lock(int fd)
{
    struct flock wrlock;
    wrlock.l_type = F_WRLCK;
    wrlock.l_whence = SEEK_SET;
    wrlock.l_start = 0;
    wrlock.l_len = 0;
    return fcntl(fd, F_SETLKW, &wrlock);
}

int req_rd_lock(int fd)
{
    struct flock rdlock;
    rdlock.l_type = F_RDLCK;
    rdlock.l_whence = SEEK_SET;
    rdlock.l_start = 0;
    rdlock.l_len = 0;
    return fcntl(fd, F_SETLKW, &rdlock);
}

int release_lock(int fd)
{
    struct flock unlock;
    unlock.l_type = F_UNLCK;
    unlock.l_whence = SEEK_SET;
    unlock.l_start = 0;
    unlock.l_len = 0;
    return fcntl(fd, F_SETLK, &unlock);
}


int main(int argc, char const *argv[])
{
    setbuf(stdout, NULL);

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) 
    {
        perror("open error");
        exit(1);
    }

    if (!fork())
    {
        sleep(1);
        printf("child 0 write lock\n");
        req_wr_lock(fd);
        printf("child 0 get write lock succ\n");
        release_lock(fd);
        printf("child 0 release write lock\n");
        exit(0);
    }
    else 
    {
        printf("parent read lock\n");
        req_rd_lock(fd);
        printf("parent get read lock succ, sleep 3 seconds\n");
        sleep(3);
    }

    for (int i = 1; i < 5; i++)
    {
        if (!fork())
        {
            printf("child %d read lock\n", i);
            req_rd_lock(fd);
            printf("child %d get read lock succ\n", i);
            sleep(i + 1);
            release_lock(fd);
            printf("child %d release read lock\n", i);
            exit(0);
        }
    }
    
    return 0;
}