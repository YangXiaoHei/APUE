#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    for (int i = 1; i < argc; ++i) {
        printf("%s : ", argv[i]);
        
        struct stat st;
        if (lstat(argv[i], &st) < 0) {
            perror("lstat");
            exit(1);
        }
        
        if (S_ISREG(st.st_mode)) {
            printf("regular\n");
        } else if (S_ISSOCK(st.st_mode)) {
            printf("socket\n");
        } else if (S_ISDIR(st.st_mode)) {
            printf("directory\n");
        } else if (S_ISBLK(st.st_mode)) {
            printf("block special\n");
        } else if (S_ISFIFO(st.st_mode)) {
            printf("fifo\n");
        } else if (S_ISCHR(st.st_mode)) {
            printf("character special\n");
        } else if (S_ISLNK(st.st_mode)) {
            printf("symbolic link\n");
        } else {
            printf("unknown mode\n");
        }
    }
    
    
    
    return 0;
}
