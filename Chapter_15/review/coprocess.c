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

// void sig_pipe(int signo)
// {
//     printf("caught SIGPIPE\n");
//     exit(1);
// }


int main(int argc, char const *argv[])
{
    int fds1[2], fds2[2];
    if (pipe(fds1) < 0 || pipe(fds2) < 0)
    {
        perror("pipe error");
        exit(1);
    }

    // if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
    // {
    //     perror("signal error");
    //     exit(1);
    // }

    pid_t pid;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0)
    {
        close(fds1[0]);
        close(fds2[1]);

        char buf[1024];
        while (fgets(buf, sizeof(buf), stdin) != NULL)
        {
            int n = strlen(buf);
            if (write(fds1[1], buf, n) != n)
            {
                perror("write to fliter prog error");
                exit(1);
            }
            if ((n = read(fds2[0], buf, sizeof(buf))) < 0)
            {
                perror("read from fliter prog error");
                exit(1);
            }
            buf[n] = 0;
            if (fputs(buf, stdout) == EOF)
                break;
        }
        if (ferror(stdin))
        {
            printf("parent read from stdin error\n");
            exit(1);
        }
        if (ferror(stdout))
        {
            printf("parent write to stdout error\n");
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
        if (fds1[0] != STDIN_FILENO)
        {
            if (dup2(fds1[0], STDIN_FILENO) != STDIN_FILENO)
            {
                perror("dup2 to STDIN_FILENO error");
                exit(1);
            }
            close(fds1[0]);
        }
        if (fds2[1] != STDOUT_FILENO)
        {
            if (dup2(fds2[1], STDOUT_FILENO) != STDOUT_FILENO)
            {
                perror("dup2 to STDOUT_FILENO error");
                exit(1);
            }
            close(fds2[1]);
        }

        if (execl("./add", "add", (char *)0) < 0)
        {
            perror("execl add error");
            exit(1);
        }
        exit(0);
    }

    return 0;
}