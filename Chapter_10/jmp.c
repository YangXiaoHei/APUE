#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>

jmp_buf jmp;


void bar() {
    longjmp(jmp, 2);
}

void foo() {
    bar();
}

int main() {
    
    int ret = 0;
    if ((ret = setjmp(jmp)) != 0) {
        printf("return value is %d\n", ret);
        exit(0);
    }
    
    foo();
    
    return 0;
}
