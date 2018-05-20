#include <stdio.h>
#include <unistd.h>

int main() {
    
    printf("PID = %d\n", getpid());
    printf("PPID = %d\n", getppid());
    printf("real user id = %d\n", getuid());
    printf("effective user id = %d\n", geteuid());
    
    while (1);
    
    return 0;
}
