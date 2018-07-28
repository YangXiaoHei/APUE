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
   struct stat oldst;
   if (lstat(argv[1], &oldst) < 0)
   {
        perror("lstat error");
        exit(1);
   }
   printf("access time :  %ld\n", oldst.st_atimespec.tv_sec * 1000000000 + oldst.st_atimespec.tv_nsec);

    return 0;
}