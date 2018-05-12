#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *env_init[] = { "USER=unknow", "PATH=/tmp", NULL };

int main() {
    
    pid_t pid;
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        if (execle("./echoall",
                   "echoall",
                   "myarg1",
                   "MY ARG2",
                   (char *)0,
                   env_init) < 0) {
            perror("execle error");
            exit(1);
        }
    }

    if (waitpid(pid, NULL, 0) < 0) {
        perror("waitpid");
        exit(1);
    }
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        
        /* 增加这一段，增加环境变量 PATH 中的路径前缀*/
        char buf[256] = { 0 };
        char *old = getenv("PATH");
        strncpy(buf, old, strlen(old));
        strncat(buf, ":.", 2);
        
        if (setenv("PATH", buf, 1) < 0) {
            printf("setenv fail");
            exit(1);
        }
        /**************************************/

        if (execlp("echoall", "echoall", "only 1 arg", (char *)0) < 0) {
            perror("execlp error");
            exit(1);
        }
    }
    return 0;
}
