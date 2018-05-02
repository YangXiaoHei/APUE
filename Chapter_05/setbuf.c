#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    FILE *fp = fopen(argv[1], "r+");
    if (fp == NULL) {
        printf("file not exist\n");
        exit(1);
    }
    
    int c = EOF;
    while ((c = fgetc(fp)) != EOF) {
        putchar(c);
    }
    
    printf("ferror %d\n", ferror(fp));
    printf("feof %d\n", feof(fp));
    
    return 0;
}
