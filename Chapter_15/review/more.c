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

#define PAGER "/usr/bin/more"

int main(int argc, char const *argv[])
{
    int fds[2];
    if (pipe(fds) < 0)
    {
        perror("pipe error");
        exit(1);
    }

    FILE *fp = NULL;
    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("fopen error\n");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0)
    {
        close(fds[0]);

        char buf[1024];
        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            int n = strlen(buf);
            if (write(fds[1], buf, n) != n)
            {
                perror("write error");
                exit(1);
            }
        }

        if (ferror(stdin))
        {
            perror("fgets error");
            exit(1);
        }

        close(fds[1]);

        // int status;
        // if (waitpid(pid, &status, 0) < 0)
        // {
        //     perror("waitpid error");
        //     exit(1);
        // }
        exit(0);
    }
    else
    {
        close(fds[1]);
        if (fds[0] != STDIN_FILENO)
        {
            if (dup2(fds[0], STDIN_FILENO) != STDIN_FILENO)
            {
                perror("dup2 error");
                exit(1);
            }
            close(fds[0]);
        }

        char *argv0 = strrchr(PAGER, '/');

        if (execl(PAGER, argv0, (char *)0) < 0)
        {
            perror("execl error");
            exit(1);
        }
        exit(0);
    }
    return 0;
}