#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char buf[20] = { 0 };
    memset(buf, 'A', sizeof(buf));

    sprintf(buf, "hello");
    printf("%s\n", buf);

    for (int i = 0; i < sizeof(buf); i++) {
        printf("%c", buf[i] == 0 ? '@' : buf[i]);
    }
    
    return 0;    
}