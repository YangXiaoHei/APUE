#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char path_name[100] = { 0 };
    
    if (chdir(argv[1]) < 0) {
        perror("chdir ");
        exit(1);
    }
    
    if (getcwd(path_name, sizeof(path_name)) < 0) {
        perror("getcwd");
        exit(1);
    }
    
    printf("cwd = %s\n", path_name);
    return 0;
}
