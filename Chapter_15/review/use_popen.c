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
    FILE *fp = NULL;
    if ((fp = popen(argv[1], "r")) == NULL)
    {
        printf("popen error\n");
        exit(1);
    }
    char buf[1024];
    while (fgets(buf, sizeof(buf), fp) != NULL)
        if (fputs(buf, stdout) == EOF)
            break;
    if (ferror(stdout))
    {
        printf("fputs error");
        exit(1);
    }
    if (ferror(fp))
    {
        printf("fgets error");
        exit(1);
    }
    return 0;
}