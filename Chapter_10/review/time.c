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
       // int tm_sec;     /* seconds (0 - 60) */
       //     int tm_min;     /* minutes (0 - 59) */
       //     int tm_hour;    /* hours (0 - 23) */
       //     int tm_mday;    /* day of month (1 - 31) */
       //     int tm_mon;     /* month of year (0 - 11) */
       //     int tm_year;     year - 1900 
       //     int tm_wday;    /* day of week (Sunday = 0) */
       //     int tm_yday;    /* day of year (0 - 365) */
       //     int tm_isdst;   /* is summer time in effect? */
       //     char *tm_zone;  /* abbreviation of timezone name */
       //     long tm_gmtoff; /* offset from UTC in seconds */
    int fd = open("time_test", O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }
    char buf[100] = { 0 };
    char *line = "------------------\n";
    while (1) {
        time_t c = time(NULL);
        struct tm *t = localtime(&c);
        if (t->tm_sec != 59)
          snprintf(buf, sizeof(buf), "%d\n", t->tm_sec);
        else
          snprintf(buf, sizeof(buf), "%d\n----------------------\n", t->tm_sec);

        int len = strlen(buf);
        if (write(fd, buf, len) != len) {
            perror("write error");
            exit(1);
        }
        sleep(1);
    }

    return 0;
}