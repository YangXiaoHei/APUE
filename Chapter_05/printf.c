#include <stdio.h>

int main(int argc, char *argv[]) {
    
    long a = -12333245234234;
    
    printf("%-20ld\n",a);
    printf("% ld\n",a);
    printf("%020ld\n",a);
    
    char b = 1;
    short c = 1;
    long d = 1;
    long long e = 1;
    intmax_t f = 1;
    size_t g = 1;
    ptrdirr_t h = 1;
    long double i = 1.0;
    
    printf("%hh\n", b);
    printf("%h\n", c);
    printf("%l\n", d);
    printf("%ll\n", e);
    printf("%j\n", f);
    printf("%z\n", g);
    printf("%t\n", h);
    printf("%L\n", i);
    
    return 0;
}
