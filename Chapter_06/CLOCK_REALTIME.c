#include <stdio.h>
#include <sys/time.h>

int main() {
    
    struct timespec ts;
    
    if (clock_gettime(CLOCK_REALTIME, &ts) != -1) {
        printf("%zd   %zd\n", ts.tv_sec, ts.tv_nsec);
    }

    return 0;
}
