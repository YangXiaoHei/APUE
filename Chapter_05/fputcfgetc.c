#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    int c = 0;
    while ((c = fgetc(stdin)) != EOF) {
        if (fputc(c, stdout) == EOF) {
            printf("fputc fail\n");
            break;
        }
    }
    
    return 0;
}
