#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char c = 0;
    while ((c = getchar()) != EOF) {
        printf("%c", c);
    }
    
    return 0;
}
