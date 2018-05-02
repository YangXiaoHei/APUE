#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void fgetcfputc(const char *src, const char *dst) {
    
    FILE *src_fp = fopen(src, "r");
    FILE *dst_fp = fopen(dst, "a+");
    if (src_fp == NULL || dst_fp == NULL) {
        printf("fopen fail\n");
        exit(1);
    }
    
    int c;
    while ((c = fgetc(src_fp)) != EOF) {
        if (fputc(c, dst_fp) == EOF) {
            break;
        }
    }
    
    if (ferror(dst_fp)) {
        printf("fputc fail\n");
    }
    if (ferror(src_fp)) {
        printf("fgetc fail\n");
    }
}

void fgetsfputs(const char *src, const char *dst) {
    
    FILE *src_fp = fopen(src, "r");
    FILE *dst_fp = fopen(dst, "a+");
    if (src_fp == NULL || dst_fp == NULL) {
        printf("fopen fail\n");
        exit(1);
    }
    char buf[5] = { 0 };
    while (fgets(buf, sizeof(buf), src_fp) != NULL) {
        if (fputs(buf, dst_fp) == EOF) {
            break;
        }
    }
    
    if (ferror(dst_fp)) {
        printf("fputs fail\n");
    }
    if (ferror(src_fp)) {
        printf("fgets fail\n");
    }
    
    fclose(dst_fp);
    fclose(src_fp);
}

int main(int argc, char *argv[]) {
    
    if (strcmp(argv[1], "fgetc") == 0) {
        fgetcfputc(argv[2], argv[3]);
    } else if (strcmp(argv[1], "fgets") == 0) {
        fgetsfputs(argv[2], argv[3]);
    }
    
    return 0;
}
