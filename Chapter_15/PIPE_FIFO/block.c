#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    int fds[2];

    pipe(fds);

    char buf[1024] = { 0 };
    if (read(fds[0], buf, sizeof(buf)) < 0) {
        perror("read error");
        exit(1);
    }
    printf("read content : %s\n", buf);    
    return 0;    
}