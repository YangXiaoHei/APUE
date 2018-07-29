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
    setbuf(stdout, NULL);
    setbuf(stdin, NULL);

    char buf[1024];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        int a, b;
        if (sscanf(buf, "%d%d", &a, &b) != 2)
        {
            printf("sscanf a b error\n");
            exit(1);
        }
        sprintf(buf, "%d\n", a + b);
        if (fputs(buf, stdout) == EOF)
            break;
    }
    if (ferror(stdin))
    {
        printf("read from stdin error\n");
        exit(1);
    }
    if (ferror(stdout))
    {
        printf("write to stdout error\n");
        exit(1);
    }
    return 0;
}