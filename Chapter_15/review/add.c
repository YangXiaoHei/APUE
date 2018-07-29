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
    char buf[1024];
    int n;
    exit(0);
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        buf[n] = 0;
        int a, b;
        if (sscanf(buf, "%d%d", &a, &b) != 2)
        {
            const char *err_msg = "invalid arg\n";
            if (write(STDOUT_FILENO, err_msg, strlen(err_msg)) != strlen(err_msg))
            {
                perror("write error");
                exit(1);
            }
        }
        /* sprintf 会自动添加 \0 */
        sprintf(buf, "%d\n", a + b);
        n = strlen(buf);
        if (write(STDOUT_FILENO, buf, n) != n)
        {
            perror("write error");
            exit(1);
        }
    }
    return 0;
}