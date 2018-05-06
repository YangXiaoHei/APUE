#include <stdio.h>
#include <stdlib.h>

static void after_exit1(void);
static void after_exit2(void);

int main(int argc, char *argv[]) {
    
    if (atexit(after_exit1) != 0) {
        printf("can't register after_exit1\n");
        exit(1);
    }
    
    if (atexit(after_exit2) != 0) {
        printf("cant't register after_exit2\n");
        exit(1);
    }
    
    if (atexit(after_exit2) != 0) {
        printf("cant't register after_exit2\n");
        exit(1);
    }
    printf("main done!\n");
    
    return 0;
}

void after_exit1(void) {
    printf("after_exit1\n");
}

void after_exit2(void) {
    printf("after_exit2\n");
}
