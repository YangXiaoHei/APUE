#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    printf("fd0 %s\n", ttyname(0));
    printf("fd1 %s\n", ttyname(1));
    printf("fd2 %s\n", ttyname(2));

    return 0;
}
