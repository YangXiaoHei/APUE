#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char buf[1024] = { 0 };
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (fputs(buf, stdout) == EOF) {
            break;
        }
    }
    
    return 0;
}
