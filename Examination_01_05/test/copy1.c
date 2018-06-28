#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

void printbuf(char *buf, int size) {
    for (int i = 0; i < size; i++) 
        printf("%c", buf[i] == 0 ? '@' : buf[i] == '\n' ? '~' : buf[i]);
    printf("\n");
}


int main(int argc, char *argv[]) {

    unsigned char str[] = { 'a', 'b', 'c', 0, 'e', 0, 'g', '\n', 'i', 'j', 'k', 0};
    int fd = open("test_file", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, str, sizeof(str));
    struct stat st;
    fstat(fd, &st);
    printf("file size = %d\n", (int)st.st_size);
    close(fd);

    FILE *fp = fopen("test_file", "r");
    char fp_buf[9] = { 0 };
    setvbuf(fp, fp_buf, _IOFBF, sizeof(fp_buf));
    char buf[4] = { 0 };
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        printbuf((char *)fp->_bf._base, fp->_bf._size);
        printbuf(buf, sizeof(buf));
        printf("\n");
        bzero(buf, sizeof(buf));
    }


    return 0;    
}