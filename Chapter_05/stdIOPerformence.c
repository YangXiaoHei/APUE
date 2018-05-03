#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

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
    
    fclose(src_fp);
    fclose(dst_fp);
}

void fgetsfputs(const char *src, const char *dst) {
    
    FILE *src_fp = fopen(src, "r");
    FILE *dst_fp = fopen(dst, "a+");
    if (src_fp == NULL || dst_fp == NULL) {
        printf("fopen fail\n");
        exit(1);
    }
    char buf[4096] = { 0 };
    while (fgets(buf, sizeof(buf), src_fp) != NULL) {
        printf("fgets break\n");
        if (fputs(buf, dst_fp) == EOF && ferror(dst_fp)) {
            printf("fputs break\n");
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

void freadfwrite(const char *src, const char *dst) {
    
    /*
     fread(void *restrict ptr, size_t size, size_t nitems,
     FILE *restrict stream);
     
     size_t
     fwrite(const void *restrict ptr, size_t size, size_t nitems,
     FILE *restrict stream);
     
     */
    struct stat sa;
    if (lstat(src, &sa) < 0) {
        perror("lstat");
        exit(1);
    }
    
    long best_io = sa.st_blksize;
    long src_size = sa.st_size;
    
    FILE *src_fp = fopen(src, "r");
    FILE *dst_fp = fopen(dst, "a+");
    if (src_fp == NULL || dst_fp == NULL) {
        printf("fopen fail\n");
        exit(1);
    }
    
    long nitems = src_size / best_io;
    long left = src_size - nitems * best_io;
    
    printf("src size = %ld\n", (long)src_size);
    printf("best io = %ld\n", (long)best_io);
    printf("total %ld items = %ld\n", (long)best_io, (long)nitems);
    printf("left = %ld\n", (long)left);
    
    char *buf = NULL;
    
    if (nitems > 0) {
        
        buf = malloc(best_io);
        bzero(buf, best_io);
        
        while (fread(buf, sizeof(buf), 1, src_fp) == 1) {
            if (fwrite(buf, sizeof(buf),1, dst_fp) != 1) {
                break;
            }
        }
        
        if (fread(buf, left, 1, src_fp) == 1) {
            if (fwrite(buf, left, 1, dst_fp) != 1) {
                printf("write fail\n");
            }
        }
        
    } else {
        
        buf = malloc(left);
        bzero(buf, left);
        
        if (fread(buf, left, 1, src_fp) == 1) {
            if (fwrite(buf, left, 1, dst_fp) != 1) {
                printf("write fail");
            }
        }
    }
    
    free(buf);
    
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
    } else if (strcmp(argv[1], "fread") == 0) {
        freadfwrite(argv[2], argv[3]);
    }
    
    return 0;
}
