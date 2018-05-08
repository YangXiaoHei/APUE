#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>


static void pr_limit(const char *name, int);

#define doit(_x_) pr_limit(#_x_, _x_);

int main() {
    
#ifdef RLIMIT_AS
    doit(RLIMIT_AS)
#endif
#ifdef RLIMIT_CORE
    doit(RLIMIT_CORE)
#endif
#ifdef RLIMIT_CPU
    doit(RLIMIT_CPU)
#endif
#ifdef RLIMIT_DATA
    doit(RLIMIT_DATA)
#endif
#ifdef RLIMIT_STACK
    doit(RLIMIT_STACK)
#endif
#ifdef RLIMIT_NPROC
    doit(RLIMIT_NPROC)
#endif
#ifdef RLIMIT_FSIZE
    doit(RLIMIT_FSIZE)
#endif
#ifdef RLIMIT_SBSIZE
    doit(RLIMIT_SBSIZE)
#endif
#ifdef RLIMIT_SIGPENDING
    doit(RLIMIT_SIGPENDING)
#endif
#ifdef RLIMIT_NOFILE
    doit(RLIMIT_NOFILE)
#endif
#ifdef RLIMIT_MEMLOCK
    doit(RLIMIT_MEMLOCK)
#endif
#ifdef RLIMIT_MSGQUEUE
    doit(RLIMIT_MSGQUEUE)
#endif
#ifdef RLIMIT_NICE
    doit(RLIMIT_NICE)
#endif
#ifdef RLIMIT_SWAP
    doit(RLIMIT_SWAP)
#endif
#ifdef RLIMIT_VMEM
    doit(RLIMIT_VMEM)
#endif
    
    return 0;
}

static void
pr_limit(const char *name, int resource) {
    struct rlimit rl;
    unsigned long long lim;
    if (getrlimit(resource, &rl) < 0) {
        printf("getrlimit fail\n");
        exit(1);
    }
    printf("%-14s ", name);
    if (rl.rlim_cur == RLIM_INFINITY) {
        printf("(infinite)");
    } else {
        printf("%10lld ", rl.rlim_cur);
    }
    
    if (rl.rlim_max == RLIM_INFINITY) {
        printf("(infinite)");
    } else {
        printf("%10lld ", rl.rlim_max);
    }
    
    putchar('\n');
    
}
