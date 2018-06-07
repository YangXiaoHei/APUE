#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    char buf[1024] = { 0 };
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        printf("fgets error");
        exit(1);
    }
    char cmd[1024] = { 0 };
    snprintf(cmd, sizeof(cmd), "cat : %s", buf);

    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen error");
        exit(1);
    }
    char out[4096] = { 0 };
    while (fgets(out, sizeof(out), fp) != NULL) {
        if (fputs(out, stdout) == EOF) {
            if (ferror(stdout)) {
                printf("fputs error");
                exit(1);
            }
        }
    }

    if (pclose(fp) < 0) {
        perror("pclose error");
        exit(1);
    }
    
    return 0;    
}