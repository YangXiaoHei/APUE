#include <stdlib.h>
#include <limits.h>
#include <sys/resource.h>

#define OPEN_MAX_GUESS 256

long open_max(void) {
    long openmax;
    struct rlimit rl;
    if ((openmax = sysconf(_SC_OPEN_MAX)) < 0 || openmax == LONG_MAX) {
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
            printf("get file limit error\n");
            exit(1);
        }
        if (rl.rlim_max == RLIM_INFINITY) {
            openmax = OPEN_MAX_GUESS;
        } else {
            openmax = rl.rlim_max;
        }
    }
    return openmax;
}
