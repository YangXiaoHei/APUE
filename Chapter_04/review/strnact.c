#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    
    char buf[1024];
    memset(buf, 'x', sizeof(buf));
    
    char *p1 = "hello world";
    strncpy(buf, p1, strlen(p1));
    for (int i = 0; i < sizeof(buf); ++i) {
        printf("%c", buf[i] ? buf[i] : ' ');
    }
    
    char *p2 = "practise_1_2_3";
    strncat(buf, p2, strlen(p2));

    printf("%s\n", buf);
    for (int i = 0; i < sizeof(buf); ++i) {
        printf("%c", buf[i] ? buf[i] : ' ');
    }
    
    return 0;
}
