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
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

long GetCurrentTimeUs(void)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char const *argv[])
{
    int n1 = strtoll(argv[1], NULL, 10);
    int n2 = strtoll(argv[1], NULL, 10);

    printf("n1 = %d n2 = %d\n", n1 ,n2);

    char *buf1 = malloc(n1);
    memset(buf1, 'a', n1);

    char *buf2 = malloc(n2);
    memset(buf2, 'b', n2);

    int fd = open("yanghan", O_RDWR | O_TRUNC | O_CREAT, 0644);
    if (fd < 0) 
    {
        perror("open error");
        exit(1);
    }

    long beg, end;

    {
        beg = GetCurrentTimeUs();

        char *buf3 = malloc(n1 + n2);
        memcpy(buf3, buf1, n1);
        memcpy(buf3 + n1, buf2, n2);
        write(fd, buf3, n1 + n2);

        end = GetCurrentTimeUs();
        printf("缓冲区复制，一次 write : %ld\n", end - beg);
    }

    {
        beg = GetCurrentTimeUs();

        struct iovec iov[2];
        iov[0].iov_base = buf1;
        iov[0].iov_len = n1;

        iov[1].iov_base = buf2;
        iov[1].iov_len = n2;

        writev(fd, iov, 2);

        end = GetCurrentTimeUs();
        printf("一次 writev : %ld\n", end - beg);
    }

    return 0;
}