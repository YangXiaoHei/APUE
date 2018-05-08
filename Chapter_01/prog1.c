#include <stdio.h>
#include <stdlib.h>

int main() {
    
    char buf[4096] = { 0 };
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        fputs(buf, stdout);
    }
    
    if (ferror(stdin) || ferror(stdout)) {
        printf("stdin or stdout fail\n");
    }
    
    return 0;
}
