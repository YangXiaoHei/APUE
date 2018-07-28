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
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

void sleep_us(unsigned int nusecs)
{
    struct timeval tv;
    tv.tv_sec = nusecs / 1000000;
    tv.tv_usec = nusecs % 1000000;
    select(0, NULL, NULL, NULL, &tv);
}

void sleep_us_v2(unsigned int nusecs)
{
    struct pollfd dummy;
    int timeout;
    if ((timeout = nusecs / 1000) <= 0) 
        timeout = 1;
    poll(&dummy, 0, timeout);
}

int main(int argc, char const *argv[])
{
    
    printf("begin to sleep 2s\n");
    sleep_us(2000 * 1000);
    printf("end sleep\n");


    return 0;
}