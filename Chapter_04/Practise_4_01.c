#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    struct stat st;
    if (stat(argv[1], &st) < 0) {
        perror("stat ");
        exit(1);
    }
    
    printf("st_ino = %ld\n", (long)st.st_ino);
    printf("st_size = %ld\n", (long)st.st_size);
    
    return 0;
}
