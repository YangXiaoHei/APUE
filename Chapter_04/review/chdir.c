#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>


int main() {
    
    chdir("/Users/bot/Desktop");
    
    char buf[1024] = { 0 };
    printf("%s\n", getcwd(buf, sizeof(buf)));
    
//    DIR *dp;
//    if ((dp = opendir("1")) == NULL) {
//        printf("打开目录失败\n");
//    }
    
    if (chdir("1") < 0) {
        perror("chdir error");
        exit(1);
    }
    
    return 0;
}
