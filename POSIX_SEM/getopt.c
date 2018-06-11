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
    
    int c;
    while ((c = getopt(argc, argv, "a:b::c")) != -1) {
        printf("------------------------\n");
        printf("return value : %c\n", c);
        printf("optarg : %s\n", optarg);
        printf("optind : %d\n", optind);
        printf("optopt : %c\n", optopt);
        printf("------------------------\n");
    }

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    
    return 0;    
}