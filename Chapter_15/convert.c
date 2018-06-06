#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    FILE *fp = popen("fliter", "r");
    if (fp == NULL) {
        perror("popen error");
        exit(1);
    }

    char buf[1024] = { 0 };
    while (1) {
        fputs("input> ", stdout);
        // fflush(stdout);
        if (fgets(buf, sizeof(buf), fp) == NULL) 
            break;
        if (fputs(buf, stdout) == EOF) {
            printf("fputs error\n");
            exit(1);
        }
    }
    
    if (ferror(fp)) {
        printf("fgets error\n");
        exit(1);
    }
    putchar('\n');
    return 0;    
}