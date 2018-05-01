#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    if (chdir("/tmp") < 0) {
        perror("chdir");
        exit(1);
    }
    
    printf("chdir to /tmp succeeded\n");
    return 0;
}
