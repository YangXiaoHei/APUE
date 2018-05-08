#include <stdio.h>
#include <string.h>

int main() {
    
    char buf[4096] = { 0 };
    
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n') {
            buf[strlen(buf) - 1] = '\0';
        }
        strncat(buf, "_prog3\n", 7);
        fputs(buf, stdout);
    }
    
    return 0;
}
