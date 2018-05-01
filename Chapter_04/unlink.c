#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    if (open("unlink_test", O_RDWR) < 0) {
        perror("crea ");
        exit(1);
    }
    if (unlink("unlink_test") < 0) {
        perror("unlink");
        exit(1);
    }
    printf("file unlink\n");
    
    sleep(15);
    
    printf("done\n");
    
    return 0;
}
