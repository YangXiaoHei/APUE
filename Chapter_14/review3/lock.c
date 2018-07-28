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
#include <poll.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int req_wr_lock_wait(int fd)
{
    struct flock lk;
    lk.l_type = F_WRLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;
    return fcntl(fd, F_SETLKW, &lk);
}

int req_wr_lock(int fd)
{
    struct flock lk;
    lk.l_type = F_WRLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;
    return fcntl(fd, F_SETLKW, &lk);
}

int req_rd_lock_wait(int fd)
{
    struct flock lk;
    lk.l_type = F_RDLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;
    return fcntl(fd, F_SETLKW, &lk);
}

int req_rd_lock(int fd)
{
    struct flock lk;
    lk.l_type = F_RDLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;
    return fcntl(fd, F_SETLKW, &lk);
}

int release_lock(int fd)
{
    struct flock lk;
    lk.l_type = F_UNLCK;
    lk.l_whence = SEEK_SET;
    lk.l_start = 0;
    lk.l_len = 0;
    return fcntl(fd, F_SETLK, &lk);
}

int fd1, fd2;
void TELL_WAIT()
{
    fd1 = open("tmp_lock1", O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd1 < 0) 
    {
        perror("open error");
        exit(1);
    }
    fd2 = open("tmp_lock2", O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd2 < 0) 
    {
        perror("open error");
        exit(1);
    }
    write(fd1, "c", 1);
    write(fd2, "c", 1);

    req_rd_lock(fd1);
    req_rd_lock(fd2);
}

void WAIT_PARENT()
{
    while (req_wr_lock(fd1) < 0);
}

void TELL_CHILD()
{
    release_lock(fd1);
}

void WAIT_CHILD()
{
    
}


int main(int argc, char const *argv[])
{

    return 0;
}