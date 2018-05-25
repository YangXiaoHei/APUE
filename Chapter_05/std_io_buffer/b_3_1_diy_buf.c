#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    
    setbuf(stdout, NULL);
    
    printf("hello world");
    
    _exit(0);
}
