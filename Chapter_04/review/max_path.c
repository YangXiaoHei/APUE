#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void creat_dir(const char *beg_path, int num) {
    char buf[1024] = { 0 };
    strncpy(buf, beg_path, strlen(beg_path));
    buf[strlen(beg_path)] = 0;
    for (int i = 0; i < num; i++) {
        if (chdir(buf) < 0) {
            perror("chdir error");
            exit(1);
        }
        bzero(buf, sizeof(buf));
        snprintf(buf, sizeof(buf), "%d", i);
        if (mkdir(buf, 0777) < 0) {
            perror("mkdir error");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("./a.out num\n");
        exit(1);
    }
    
    creat_dir(".", atoi(argv[1]));
    
    char buf[2048] = { 0 };
    if (getcwd(buf, sizeof(buf)) == NULL) {
        perror("getcwd error");
        exit(1);
    }
    printf("%d  %s\n",strlen(buf), buf);
    
    return 0;
}
