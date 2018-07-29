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
    
 // struct pollfd {
 //     int    fd;       /* file descriptor */
 //     short  events;   /* events to look for */
 //     short  revents;  /* events returned */
 // };

    int fds[2];
    if (pipe(fds) < 0)
    {
        perror("pipe error");
        exit(1);
    }
    
    struct pollfd pf[1];
    pf[0].fd = fds[0];
    pf[0].events = POLLIN;

    int nfds = sizeof(pf) / sizeof(pf[0]);

    int sec = 2;
    int n;

    pid_t pid;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0)
    {
        close(fds[1]);

        int status;
        while (1)
        {
            if ((n = poll(pf, nfds, sec * 1000)) < 0)
            {
                if (errno == EINTR)
                    continue;
                perror("poll error");
                break;
            }
            else if (n == 0)
            {
                printf("no readable event occur, try again!\n");
                continue;
            }

            for (int i = 0; i < nfds; i++)
            {
                if ((pf[0].revents & POLLIN) == 1)
                {
                    char buf[3] = { 0 };
                    int nread;
                    if ((nread = read(fds[0], buf, 3)) < 0)
                    {
                        perror("read error");
                        exit(1);
                    }
                    else if (nread == 0)
                    {
                        printf("EOF\n");
                        goto finished;
                    }
                    else
                    {
                        buf[nread] = 0;
                        printf("read content : %s\n", buf);
                    }
                }
                else
                {
                    printf("pf[0].revents = %d\n", pf[0].revents);
                }
            }
        }
finished:
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid error");
            exit(1);
        }
        exit(0);
    }
    else
    {
        close(fds[0]);

        printf("child proc begin to sleep 3 secs\n");
        sleep(3);
        printf("child wakeup\n");

        write(fds[1], "123456789", 9);

        close(fds[1]);

        exit(0);
    }

    return 0;
}