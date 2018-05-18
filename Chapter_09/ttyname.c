#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
    printf("%s\n", ttyname(0));
    printf("%s\n", ttyname(1));
    printf("%s\n", ttyname(2));
    return 0;    
}
