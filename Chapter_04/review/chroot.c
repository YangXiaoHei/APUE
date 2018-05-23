#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

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
    printf("current process directory : %s\n", getcwd(buf, sizeof(buf)));
    
    if (chroot(buf) < 0) {
        perror("chroot error");
        printf("real uid = %d effective uid = %d\n", getuid(), geteuid());
        exit(1);
    }
    
    bzero(buf, sizeof(buf));
    printf("current process directory : %s\n", getcwd(buf, sizeof(buf)));
    
    return 0;
}
