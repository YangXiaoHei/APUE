#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    char buf[1024] = { 0 };
    int nread = 0;
    if ((nread = read(STDIN_FILENO, buf, sizeof(buf))) < 0) {
        perror("read error");
        exit(1);
    }
    int nwritten = 0;
    if ((nwritten = write(STDOUT_FILENO, buf, nread)) != nread) {
        perror("write error");
        exit(1);
    }
    
    return 0;    
}