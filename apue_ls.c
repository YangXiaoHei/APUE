#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    DIR *dp;
    struct dirent *dirp;
    if (argc != 2) {
        printf("usage : ls name\n");
        exit(1);
    }
    
    if ((dp = opendir(argv[1])) == NULL) {
        perror("opendir ");
        exit(1);
    }
    while ((dirp = readdir(dp)) != NULL) {
        printf("%s\n", dirp->d_name);
    }
    
    return 0;
}
