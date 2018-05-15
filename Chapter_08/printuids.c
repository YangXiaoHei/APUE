#include <stdio.h>
#include <unistd.h>

int main() {
    printf("real uid = %d, effective uid = %d\n", getuid(), geteuid());
}
