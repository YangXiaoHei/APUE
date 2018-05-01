#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    FILE *fp = fopen(argv[1], "r+");
    if (fp == NULL) {
        printf("file not exits\n");
        exit(1);
    }
    
    char c = '\0';
    while ((c = fgetc(fp)) != EOF) {
        printf("%c", c);
    }
    
    fclose(fp);
    
    return 0;
}
