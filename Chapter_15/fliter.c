#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    int c;
    while ((c = getchar()) != EOF) {
        if (isupper(c)) {
            c = tolower(c);
        }
        if (putchar(c) == EOF) {
            printf("putchar error");
            exit(1);
        }
        if (c == '\n') {
            fflush(stdout);
        }
    }
    return 0;    
}