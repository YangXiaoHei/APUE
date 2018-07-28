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

int capcacity(void)
{
    int fds[2];
    if (pipe(fds) < 0) {
        perror("pipe error");
        exit(1);
    }

    int flags = fcntl(fds[1], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fds[1], F_SETFL, flags);

    int cap = 0;

    int ret_code = 0;
    while (1) 
    {
        if (write(fds[1], "a", 1) != 1 && errno == EAGAIN) 
            break;   
        cap++;
    }
    return cap;
}

int main(int argc, char const *argv[])
{
    printf("pipe capacity = %d\n", capcacity());
    printf("PIPE_BUF = %d\n", _POSIX_PIPE_BUF);
    return 0;
}