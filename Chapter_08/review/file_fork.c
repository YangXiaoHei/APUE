#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PATH "./yanghan"

int main(int argc, char *argv[]) {
    
    setbuf(stdout, NULL);
    
    int fd = open(PATH, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    int a_l = 32768 + 1, b_l = 32768 + 1;
    
    char *a = malloc(a_l);
    char *b = malloc(b_l);
    
    memset(a, 'a', a_l);
    memset(b, 'b', b_l);
    
    a[a_l - 1] = 0;
    b[b_l - 1] = 0;
    
    printf("total %ld\n", strlen(a) + strlen(b));
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        
        for (int i = 0; i < a_l - 1; i += 64) {
            write(fd, a + i, 64);
        }
        
    } else {
        
        for (int i = 0; i < b_l - 1; i += 64) {
            write(fd, b + i, 64);
        }
    }
    
    return 0;
}
