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
    int fd1[2], fd2[2];

    if (pipe(fd1) < 0 || pipe(fd2) < 0)
    {
        perror("pipe error");
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
        close(fd1[0]);
        close(fd2[1]);

        FILE *fd1_write = fdopen(fd1[1], "w");
        FILE *fd2_read = fdopen(fd2[0], "r");

        setbuf(fd1_write, NULL);
        setbuf(fd2_read, NULL);

        char buf[1024];
        while (fgets(buf, sizeof(buf), stdin) != NULL)
        {
            if (fputs(buf, fd1_write) == EOF)
            {
                printf("write to fliter error\n");
                exit(1);
            }
            if (fgets(buf, sizeof(buf), fd2_read) == NULL)
            {
                if (ferror(fd2_read))
                {
                    printf("read from fliter error\n");
                    exit(1);
                }
            }
            if (fputs(buf, stdout) == EOF)
            {
                printf("write to stdout error\n");
                exit(1);
            }
        }

        if (ferror(stdin))
        {
            printf("read from stdin error\n");
            exit(1);
        }
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid error");
            exit(1);
        }
        exit(0);
    }
    else
    {
        close(fd1[1]);
        close(fd2[0]);

        if (fd1[0] != STDIN_FILENO)
        {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
            {
                perror("dup2 to STDIN_FILENO error");
                exit(1);
            }
            close(fd1[0]);
        }
        if (fd2[1] != STDOUT_FILENO)
        {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
            {
                perror("dup2 to STDOUT_FILENO error");
                exit(1);
            }
            close(fd2[1]);
        }

        if (execl("add", "add", (char *)0) < 0)
        {
            perror("execl add error");
            exit(1);
        }
        exit(0);
    }
    return 0;
}