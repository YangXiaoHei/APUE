#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define EXEC_MORE "/usr/bin/more"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage : %s [file_name]\n", argv[0]);
        exit(1);
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen error");
        exit(1);
    }

    FILE *more_fp = popen(EXEC_MORE, "w");
    if (more_fp == NULL) {
        perror("popen error");
        exit(1);
    }

// RETURN VALUES
//      Upon successful completion, fgets() and gets() return a pointer to the
//      string.  If end-of-file occurs before any characters are read, they return
//      NULL and the buffer contents remain unchanged.  If an error occurs, they
//      return NULL and the buffer contents are indeterminate.  The fgets() and
//      gets() functions do not distinguish between end-of-file and error, and
//      callers must use feof(3) and ferror(3) to determine which occurred.
    
    char buf[1024] = { 0 };
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        ssize_t len = strlen(buf);
        if (fputs(buf, more_fp) == EOF) {
            if (ferror(more_fp)) {
                printf("fputs error");
                exit(1);
            }
        }
    }
    if (ferror(fp)) {
        printf("fgets error\n");
        exit(1);
    }

    if (pclose(more_fp) < 0) {
        perror("pclose error");
        exit(1);
    }
    
    return 0;    
}