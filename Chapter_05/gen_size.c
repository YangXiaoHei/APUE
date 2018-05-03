#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("./a.out [file_name][file_size]\n");
        exit(1);
    }
    
    int fd = 0;
    if ((fd = creat(argv[1], 0640)) < 0) {
        perror("creat");
        exit(1);
    }
    
    int size = atoi(argv[2]);
    char random[] = { 'a', 'b','\0', 'c', '\0', 'd','\0', 'e','\0', 'f','\0', 'g','\0', 'h','\0', 'i', 'j','\0', 'k','\0', 'l', 'm', 'n','\0', 'o', 'p', 'q', 'r','\0', 's','\0','t','\0','u','v','\0','w','x','\0','y','z'  };
    char *p = random, *q = NULL;
    int i = 0; char c = '0';
    long j = 0;
    while (size--) {
        if (write(fd, q = (p + (i++ % sizeof(random))), 1) < 0) {
            perror("write ");
            exit(1);
        }
        if (q != NULL && *q == '\0') {
            j++;
        }
    }
    
    printf("空字符个数 : %ld\n", j);
    
    close(fd);
    
    return 0;
}
