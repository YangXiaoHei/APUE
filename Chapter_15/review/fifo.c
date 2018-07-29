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


int main(int argc, char const *argv[])
{
    if (mkfifo(argv[1], 0644) < 0 && errno != EEXIST)
    {
        perror("mkfifo error");
        exit(1);
    }

    int fdread;
    if ((fdread = open(argv[1], O_RDONLY | O_NONBLOCK)) < 0)
    {
        perror("open for read error");
        exit(1);
    }
    int fdwrite;
    if ((fdwrite = open(argv[1], O_WRONLY)) < 0)
    {
        perror("open for write error");
        exit(1);
    }

    int flags = fcntl(fdread, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(fdread, F_SETFL, flags);

    return 0;
}