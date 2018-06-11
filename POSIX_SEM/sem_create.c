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

int main(int argc, char *argv[]) {
    
    int c, flags;
    sem_t *sem;
    unsigned int value;

    flags = O_RDWR | O_CREAT;

    value = 1;

    while ((c = getopt(argc, argv, "ei:")) != -1) {
        switch(c) {
            case 'e' :
                flags |= O_EXCL;
                break;
            case 'i' :
                value = atoi(optarg);
                break;
        }
    }
    if (optind != argc - 1) {
        printf("usage : sem_create [ -e ] [ -i initalvalue ] <name>\n");
        exit(1);
    }

    sem = sem_open(argv[optind], flags, 0644, value);

    sem_close(sem);
    
    return 0;    
}