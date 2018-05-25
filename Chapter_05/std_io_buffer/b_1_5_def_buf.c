#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

int main() {
    
    char buf[4096];
    memset(buf, 'a', sizeof(buf));
    
    printf("%s", buf);
    
    _exit(0);
}

