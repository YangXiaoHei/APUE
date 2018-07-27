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
#include <syslog.h>
#include <sys/resource.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif


void daemonize(const char *cmd)
{
    struct rlimit rl;
    struct sigaction sa;
    pid_t pid;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        printf("%s: can't get file limit\n", cmd);
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid != 0)
        exit(0);

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        printf("%s: can't ignore SIGHUP\n", cmd);
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(1);
    }
    else if (pid != 0)
        exit(0);

    if (chdir("/") < 0)
    {
        perror("chdir error");
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;

    for (int i = 0; i < rl.rlim_max; i++)
        close(i);

    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    FILE *fp;
    char *p;

    daemonize("hahahah");

    p = getlogin();
    fp = fopen("/tmp/getlog.out", "w");
    if (fp != NULL)
    {
        if (p == NULL)
            fprintf(fp, "no login name\n");
        else
            fprintf(fp, "login name: %s\n", p);
    }

    return 0;
}