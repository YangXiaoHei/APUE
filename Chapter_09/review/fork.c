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
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int main(int argc, char const *argv[])
{
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {
        setsid();
        printf("child gid = %ld  pid = %ld\n", (long)getgid(), (long)getpid());
        printf("%s \n %s \n %s \n", ttyname(0), ttyname(1), ttyname(2));
        while(1);
        exit(0);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid error");
        exit(1);
    }
    return 0;
}