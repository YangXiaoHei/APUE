#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    
    char name[1024], line[4096];
    FILE *fp;
    
    printf("%s\n", tmpnam(NULL));
    
    tmpnam(name);
    printf("%s\n", name);
    
    if ((fp = tmpfile()) == NULL) {
        printf("tmpfile error\n");
        exit(1);
    }
    
    printf("临时文件文件描述符为 : %d\n", fp->_file);
    
    fputs("one line of output\n", fp);
    printf("ftell = %d\n", ftell(fp));
    
//    fseek(fp, 0, SEEK_SET);
    /*
     fseek(fp, 0, SEEK_SET) 的作用等同于 rewind()
     */
     
    rewind(fp);
    printf("ftell = %d\n", ftell(fp));
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("fgets fail\n");
        exit(1);
    }
    fputs(line, stdout);
   
    return 0;
}
