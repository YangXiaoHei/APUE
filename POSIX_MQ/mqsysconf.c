#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

int main(int argc, char *argv[]) {

    long mq_open_max, mq_prio_max;
    if ((mq_open_max = sysconf(_SC_MQ_OPEN_MAX)) < 0) {
        perror("sysconf error");
        
    }
    if ((mq_prio_max = sysconf(_SC_MQ_PRIO_MAX)) < 0) {
        perror("sysconf error");
        
    }
    
    printf("MQ_OPEN_MAX = %ld, MQ_PRIO_MAX = %ld\n", mq_open_max, mq_prio_max);
    
    return 0;    
}