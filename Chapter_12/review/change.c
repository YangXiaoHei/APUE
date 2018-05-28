#include <stdio.h>
#include <stdlib.h>
/********************** old_version ******************/
//int x = 5;
//int max(int a) { return a > x ? a : x; }
//
//#define call(_clourse_) \
//do {            \
//    printf("max number is %d\n", _clourse_); \
//} while (0);
//
//int main() {
//
//    call(max(6));
//
//    return 0;
//}
/****************************************************/

/*************** modified_version *******************/

int x = 5;
int max(int a) {  return a > x ? a : x; }

typedef int  (*function)(int);
struct wrapper {
    function fun;
    int arg;
};

static struct wrapper *init(function fun, int arg) {
    struct wrapper *w = malloc(sizeof(struct wrapper));
    if (w == NULL) return NULL;
    w->fun = fun;
    w->arg = arg;
    return w;
}

int call(struct wrapper *w) {
    int rel = w->fun(w->arg);
    free(w);
    printf("max number is %d\n", rel);
    return rel;
}

int main() {
    
    call(init(max, 6));
    
    return 0;
}

/****************************************************/
