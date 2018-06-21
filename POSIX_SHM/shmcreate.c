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

void test(int argc, char *argv[]) {
    int c;
    while ((c = getopt(argc, argv, "a:b::cd")) != -1) {
        switch (c) {
            case 'a' : {
                printf("\n--------------------\n"
                       "optarg : %s\n"
                       "optind : %d\n"
                       "optopt : %c\n"
                       "--------------------\n", optarg, optind, optopt);
            } break;
            case 'b' : {
                printf("\n--------------------\n"
                       "optarg : %s\n"
                       "optind : %d\n"
                       "optopt : %c\n"
                       "--------------------\n", optarg, optind, optopt);
            } break;
            case 'c' : {
                printf("\n--------------------\n"
                       "optarg : %s\n"
                       "optind : %d\n"
                       "optopt : %c\n"
                       "--------------------\n", optarg, optind, optopt);
            } break;
            case 'd' : {
                printf("\n--------------------\n"
                       "optarg : %s\n"
                       "optind : %d\n"
                       "optopt : %c\n"
                       "--------------------\n", optarg, optind, optopt);
            } break;
            default : {
                printf("\n------- default -------\n"
                       "optarg : %s\n"
                       "optind : %d\n"
                       "optopt : %c\n"
                       "--------------------\n", optarg, optind, optopt);
            } break;
        }
    }
    printf("\n------- outter -------\n"
           "optarg : %s\n"
           "optind : %d\n"
           "optopt : %c\n"
           "--------------------\n", optarg, optind, optopt);
} 

int main(int argc, char *argv[]) {

    // test(argc, argv);

    int flags = O_CREAT | O_RDWR;
    
    int c;
    while ((c = getopt(argc, argv, "e")) != -1) {
        switch (c) {
            case 'e' : {
                flags |= O_EXCL;
                break;
            }
        }
    }
    if (optind != argc - 2) {
        printf("usage : %s [ -e ] <name> <length>\n", argv[0]);
        exit(1);
    }
    int length = atoi(argv[optind + 1]);
    int fd = shm_open(argv[optind], flags, 0644);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }

    printf("length = %d\n", length);
    if (ftruncate(fd, length) < 0) {
        perror("ftruncate error");
        exit(1);
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
      perror("fstat error");
      exit(1);
    }
    printf("newsize = %d\n", st.st_size);

    char *ptr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    
    return 0;    
}