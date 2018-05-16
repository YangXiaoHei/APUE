#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

int main() {
//    printf("getenv = %s\n", getenv("BigPig"));
    if (setenv("BigPig", "a short string", 1) < 0) {
        printf("setenv fail\n");
    } else {
        printf("setenv succ\n");
    }
    printf("now BigPig is %s\n", getenv("BigPig"));
    return 0;
}
