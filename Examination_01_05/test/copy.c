#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

/*
 * write read blksize
 */
void copy_1(const char *src, const char *dst) {
    int srcfd = open(src, O_RDONLY);
    int dstfd = open(dst, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (srcfd <  0 || dstfd < 0) {
        perror("open error");
        exit(1);
    }
    struct stat st;
    if (fstat(srcfd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }
    int buffer_size = st.st_blksize;
    char *buffer = malloc(buffer_size);
    if (buffer == NULL) {
        printf("malloc error\n");
        exit(1);
    }
    int nread = 0, nwritten = 0;
    while ((nread = read(srcfd, buffer, buffer_size)) > 0) {
        if ((nwritten = write(dstfd, buffer, nread)) != nread) {
            perror("write error");
            break;
        }
    }
    printf("copy_1 succ\n");
    free(buffer);
}

/*
 * write read other size
 */
void copy_2(const char *src, const char *dst) {

}

/*
 * fgets fputs
 */
void print_buf(unsigned char *buf, int size, const char *msg) {
    printf("%s : ", msg);
    for (int i = 0; i < size; i++) 
        printf("%c", buf[i] == 0 ? '@' : buf[i] == '\n' ? '~' : buf[i]);
    printf("\n");
}
void copy_3(const char *src, const char *dst) {
    unlink(dst);
    FILE *srcfp = fopen(src, "r");
    FILE *dstfp = fopen(dst, "w");
    if (srcfp == NULL || dstfp == NULL) {
        printf("fopen error\n");
        exit(1);
    }
    char buf[1024];
    while (fgets(buf, sizeof(buf), srcfp) != NULL) {
        int nwritten = 0;
        if ((nwritten = fputs(buf, dstfp)) == EOF) {
            printf("fputs error");
            exit(1);
        }
    }
    if (ferror(srcfp)) {
        printf("fgets error\n");
        exit(1);
    }
    if (feof(srcfp)) {
        printf("copy_3 succ\n");
    }
}


/*
 * fread fwrite
 */
void copy_4(const char *src, const char *dst) {
    unlink(dst);
    FILE *srcfp = fopen(src, "r");
    FILE *dstfp = fopen(dst, "w");
    if (srcfp == NULL || dstfp == NULL) {
        printf("fopen error\n");
        exit(1);
    }
    char buf[1024] = { 0 };
    int nread = fread(buf, sizeof(buf), 1, srcfp);
    printf("nread = %d\n", nread);
    if (ferror(srcfp)) {
        printf("出错\n");
        exit(1);
    } 
    if (feof(srcfp)) {
        printf("末尾\n");
    }
    print_buf(buf, sizeof(buf), "fread");
    nread = fread(buf, sizeof(buf), 1, srcfp);
    printf("nread = %d\n", nread);
    if (ferror(srcfp)) {
        printf("出错\n");
        exit(1);
    } 
    if (feof(srcfp)) {
        printf("末尾\n");
    }
    // while (fread(buf, sizeof(buf), 1, srcfp) ) {
    //     content
    // }
}

/*
 * fputc fgetc
 */
void copy_5(const char *src, const char *dst) {
    
}




int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("usage : %s <src><dst>\n", argv[0]);
        exit(1);
    }

    copy_4(argv[1], argv[2]);
    
    
    return 0;    
}