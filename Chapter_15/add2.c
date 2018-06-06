#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char buf[1024] = { 0 };
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        ssize_t len = strlen(buf);
        int a, b;
        if (sscanf(buf, "%d%d", &a, &b) != 2) {
            printf("sscanf error\n");
            exit(1);
        }
        printf("a + b = %d\n", a + b);
        fflush(stdout);
    }
    
    return 0;    
}