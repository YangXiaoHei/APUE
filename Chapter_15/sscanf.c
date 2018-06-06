#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char buf[10] = { 0 };
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        printf("fgets error");
        exit(1);
    }
    ssize_t len = strlen(buf);
    int a, b;
    ssize_t rel = 0;
    if ((rel = sscanf(buf, "%d%d", &a, &b)) != 2) {
        printf("sscanf error with ret_cdoe %zd\n", rel);
        exit(1);
    }
    printf("a = %d b = %d\n", a, b);
    
    return 0;    
}