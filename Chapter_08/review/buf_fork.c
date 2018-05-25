#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    
    pid_t pid;
    int status;
    
    unsigned long buf_size_before = stdout->_bf._size;
    unsigned char *buf_before = stdout->_bf._base;
    
    printf("unknow buffer");
    
    unsigned long buf_size_after = stdout->_bf._size;
    unsigned char *buf_after = stdout->_bf._base;
    
    printf("before : %zd  %c\n"
           "after : %zd  %c\n",
           buf_size_before, buf_before != NULL ? (*buf_before) : 'x',
           buf_size_after,  buf_after  != NULL ? (*buf_after)  : 'x');
    
   
    _exit(0);
}
