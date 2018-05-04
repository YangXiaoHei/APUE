#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>


int main() {
    
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    printf("tv_sec = %zd   tv_nsec = %zd\n", tv.tv_sec, tv.tv_usec);
    
    return 0;
}
