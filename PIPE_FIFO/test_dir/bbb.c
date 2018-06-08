#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    
    char buf[1024];

    fgets(buf, sizeof(buf), stdin);

    int write_fd = open("./yanghan", O_WRONLY);

    int n = strlen(buf);
    write(write_fd, buf,  n);
    
    return 0;    
}