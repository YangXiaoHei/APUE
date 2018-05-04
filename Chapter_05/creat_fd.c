#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *Argv[]) {
    
    for (int i = 0; i < 100; i++) {
        FILE *fp = tmpfile();
        if (fp == NULL) {
            printf("临时文件创建失败\n");
            break;
        } else {
            printf("fd = %d\n", fp->_file);
        }
    }
    
    
    return 0;
}
