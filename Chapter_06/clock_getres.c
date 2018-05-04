#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main() {
    
    struct timespec ts;
    //CLOCK_MONOTONIC
    // CLOCK_PROCESS_CPUTIME_ID  都他妈一样???
    // CLOCK_REALTIME
    if (clock_getres(CLOCK_REALTIME, &ts) != -1) {
        printf("tv_sec = %ld tv_nsec = %ld\n", ts.tv_sec, ts.tv_nsec);
    }
    
    return 0;
}
