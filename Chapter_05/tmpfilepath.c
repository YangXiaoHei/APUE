#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    
    char name[PATH_MAX] = { 0 };
    tmpnam(name);
    
    printf("%s\n", name);
    
    FILE *fp = fopen(name, "w+");
    if (fp == NULL) {
        printf("file not exist\n");
        exit(1);
    }
    for (int i = 0; i < 2; i++) {
        printf("%d\n", i);
        sleep(1);
    }
    unlink(name);
    printf("unlink finished\n");
    
    char buf[4096] = { 0 };
    int i = 0;
    int n = 10;
    while (n--) {
        snprintf(buf, sizeof(buf), "hello wolrd = %d\n", i++);
        printf("0 = current offset is %d\n", ftell(fp));
        fseek(fp, 0, SEEK_END);
        fputs(buf, fp);
        printf("1 = current offset is %d\n", ftell(fp));
        int len = strlen(buf);
        fseek(fp, -len, SEEK_CUR);
        printf("2 = current offset is %d\n", ftell(fp));
        fgets(buf, sizeof(buf), fp);
        printf("3 = current offset is %d\n", ftell(fp));
        fputs(buf, stdout); //  在外面重定向到日志输出
        printf("4 = current offset is %d\n", ftell(fp));
        fflush(stdout);
        sleep(1);
    }
    
    return 0;
}
