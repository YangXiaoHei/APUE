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
    int fds[2];
    if (pipe(fds) < 0)
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
        close(fds[1]);

        fd_set rd_fd;
        
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int nfd = fds[0] + 1;
        int nready = 0;

        char buf[1024];
        int status;

        while (1)
        {
            /*  select 是水平触发的，只要有数据，就会一直报告有可读事件发生，
                ⚠️注意，因为 select 会修改传入的参数，所以一旦这次没有检测到有可读可写异常事件
                发生，设置要监听的文件描述符会被清除掉，所以下一次迭代时，
                必须对 fd_set 重新设置要监听的文件描述符
             */
            FD_ZERO(&rd_fd);
            FD_SET(fds[0], &rd_fd);
            if ((nready = select(nfd, &rd_fd, NULL, NULL, &tv)) < 0)
            {
                if (errno == EINTR)
                    continue;
                perror("select error");
                break;
            }
            else if (nready == 0)
            {
                printf("no readable event, try again!\n");
                continue;
            }

            for (int i = 0; i < nfd; i++)
            {
                if (FD_ISSET(fds[0], &rd_fd))
                {
                    int n;
                    if ((n = read(fds[0], buf, 3)) < 0)
                    {
                        perror("read error");
                        exit(1);
                    }
                    else if (n == 0)
                    {
                        printf("EOF\n");
                        goto finished;
                    }
                    buf[n] = 0;
                    printf("read content : %s\n", buf);
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

        printf("child begin to sleep 3 seconds\n");
        sleep(3);
        printf("child wakeup\n");

        write(fds[1], "123456789", 9);

        close(fds[1]);
        exit(0);
    }

    return 0;
}