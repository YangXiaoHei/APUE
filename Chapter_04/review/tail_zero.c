#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    
    char buf[64];
    memset(buf, 'a', sizeof(buf));
    char *p = "/YangHan/is/pig";
    char *q = "I Love You";
    strncpy(buf, p, strlen(p));
    buf[strlen(p)] = 0;
    strncat(buf + strlen(p), q, strlen(q));
    printf("%s\n", buf);
    for (int i = 0; i < sizeof(buf); i++) {
        printf("%c", buf[i]);
    }
    
    
    return 0;
}
