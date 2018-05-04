#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int main(int argc, char *argv[]) {
    
    struct timespec ts;
    
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != -1) {
        printf("tv_sec = %zd  tv_nsec = %zd\n",
               (unsigned long)ts.tv_sec,
               (unsigned long)ts.tv_nsec);
        int tv = ts.tv_sec;
        int day = tv / (24 * 60 * 60);
        tv %= (24 * 60 * 60);
        int hour = tv / (60 * 60);
        tv %= (60 * 60);
        int min = tv / 60;
        tv %= 60;
        printf("朋友！！%zd 天 %zd 小时 %zd 分 %zd 秒 没有关机了！\n",
               (unsigned long)day,
               (unsigned long)hour,
               (unsigned long)min,
               (unsigned long)tv);
    }
    
    
    return 0;
}
