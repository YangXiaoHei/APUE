#include <stdio.h>
#include <stdlib.h>

#define YH_GET_COUNT(COUNT, ...) COUNT

#define YH_ARG_30_PLACEHOLDER(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, \
                  _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
                  _22, _23, _24, _25, _26, _27, _28, _29, ...) \
            YH_GET_COUNT( __VA_ARGS__)

#define YH_ARG_COUNT(...) \
            YH_ARG_30_PLACEHOLDER(__VA_ARGS__,30, 29, 28, 27, 26, 25, 24, 23, \
                                  22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, \
                                  11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

int main(int argc, char *argv[]) {
    printf("argc count = %d\n", YH_ARG_COUNT("hello", 
                                             123, 3.14, 
                                             'a', 
                                             "world", 
                                             0x987654321));
    return 0;    
}