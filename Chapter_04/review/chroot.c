#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

void yh_ls(DIR *dp) {
    if (dp == NULL)
        return;
    struct dirent *dir_ent;
    while ((dir_ent = readdir(dp)) != NULL) {
        if (strcmp(dir_ent->d_name, "..") == 0 ||
            strcmp(dir_ent->d_name, ".") == 0 ||
            dir_ent->d_name[0] == '.')
            continue;
        printf("%s\n", dir_ent->d_name);
    }
}

int main() {
    
    mkdir("chroot_test", 0777);
    chdir("chroot_test");  // 进入 chroot_test
    
    mkdir("1", 0777);
    
    chdir("1"); // 进入 1
    
    mkdir("1.1", 0777);
    mkdir("1.2", 0777);
    
    chdir("1.1"); // 进入 1.1
    
    mkdir("1.1.1", 0777);
    mkdir("1.1.2", 0777);
    
    chdir(".."); // 回到 1.1
    chdir(".."); // 回到 1
    
    mkdir("2", 0777);
    
    chdir("2"); // 进入 2
    
    mkdir("2.1", 0777);
    mkdir("2.2", 0777);
    mkdir("2.3", 0777);
    
    chdir(".."); // 回到 2

    char buf[1024] = { 0 };
    printf("---------------------------------------------\n"
           "current process directory : %s\n"
           "---------------------------------------------\n", getcwd(buf, sizeof(buf)));
    
    DIR *dp = opendir("/");
    if (dp != NULL) {
        yh_ls(dp);
        closedir(dp);
    }
    
    if (chroot(buf) < 0) {
        perror("chroot error");
        printf("real uid = %d effective uid = %d\n", getuid(), geteuid());
        exit(1);
    }
    
    bzero(buf, sizeof(buf));
    printf("---------------------------------------------\n"
           "current process directory : %s\n"
           "---------------------------------------------\n", getcwd(buf, sizeof(buf)));
    
    dp = opendir("/");
    if (dp != NULL) {
        yh_ls(dp);
        closedir(dp);
    }
    
    return 0;
}
