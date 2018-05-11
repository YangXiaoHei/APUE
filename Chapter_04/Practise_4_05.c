#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    struct stat st;
    for (int i = 1; i < argc; ++i) {
        if (lstat(argv[i], &st) < 0) {
            perror("lstat ");
            exit(1);
        }
        printf("%s -> st_size = %zd\n",argv[i], (unsigned long)st.st_size);
    }
    
    return 0;
}
