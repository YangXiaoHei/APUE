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
    sem_t *sem = sem_open("/yanghan_sem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) 
    {
        perror("sem_open error");
        exit(1);
    }

    int fd = open(argv[1], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0)
    {
        perror("open error");
        exit(1);
    }
    int n = 0;
    write(fd, &n, 4);
    lseek(fd, 0, SEEK_SET);

    int loop = atoi(argv[2]);

    setbuf(stdout, NULL);

    pid_t pid;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0)
    {
        int value;
        for (int i = 0; i < loop; i++)
        {
            sem_wait(sem);
            if (read(fd, &value, 4) < 0)
            {
                perror("read error");
                exit(1);
            }
            lseek(fd, 0, SEEK_SET);
            value++;
            if (write(fd, &value, 4) < 0)
            {
                perror("write error");
                exit(1);
            }
            lseek(fd, 0, SEEK_SET);
            printf("parent write %d \n", value);
            sem_post(sem);

            sleep(1);
        }
        int status;
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid error");
            exit(0);
        }
    }
    else
    {
        int value;

        for (int i = 0; i < loop; i++)
        {
            sem_wait(sem);
            if (read(fd, &value, 4) < 0)
            {
                perror("read error");
                exit(1);
            }
            lseek(fd, 0, SEEK_SET);
            value++;
            if (write(fd, &value, 4) < 0)
            {
                perror("write error");
                exit(1);
            }
            lseek(fd, 0, SEEK_SET);
            printf("child write %d \n", value);
            sem_post(sem);

            sleep(1);
        }
    }

    sem_close(sem);
    return 0;
}