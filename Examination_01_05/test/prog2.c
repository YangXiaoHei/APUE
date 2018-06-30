#include "exam_prog2.h"

int main(int argc, char *argv[]) {
    
    close(2);
    if (my_dup2(2, 5) < 0) 
        printf("my_dup2 fail 1: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 1\n");

    if (my_dup2(1, 1000) < 0) 
        printf("my_dup2 fail 2: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 2\n");

    int fd = open("test_file", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (my_dup2(1, fd) < 0)
        printf("my_dup2 fail 3: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 3\n");

    const char *ptr = "hello world to stdout\n";
    write(fd, ptr, strlen(ptr));

    if (my_dup2(1, 1) < 0)
        printf("my_dup2 fail 4: %s\n", strerror(errno));
    else
        printf("my_dup2 succ 4\n");
    
    return 0;    
}


