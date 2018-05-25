#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

/*
 该输出函数不会改变标准 I/O 库的缓冲区，
 并且每次调用都对应一次系统调用 write
 */
size_t __not_use_stdio_printf(const char *format, ...) {
    if (format == NULL)  return 0;
    va_list vl;
    va_start(vl, format);
    char buf[1024] = { 0 };
    vsnprintf(buf, sizeof(buf), format, vl);
    size_t len = strlen(buf), written = 0;
    if ((written = write(STDOUT_FILENO, buf, len)) != len)
        return written;
    va_end(vl);
    return len;
}

#ifdef printf_nb
#error what a fuck ??
#else
#define printf_nb(_format_, ...) \
__not_use_stdio_printf(_format_, ##__VA_ARGS__)
#endif

/*
 打印标准 I/O 缓冲区信息
 */
void print_stdio_buffer_info_nb() {
    
    printf_nb("---------------------------\n");
    printf_nb("stdio buffer type : ");
    if      (stdout->_flags & _IONBF)   printf_nb("no buffer");
    else if (stdout->_flags & _IOLBF)   printf_nb("line buffer");
    else if (stdout->_flags & _IOFBF)   printf_nb("full buffer");
    else                                printf_nb("unknow buffer");
    printf_nb("\n");
    printf_nb("stdio buffer size : %ld B\n", stdout->_bf._size);
    printf_nb("stdio buffer first bytes : %c\n",
              stdout->_bf._base != NULL ?
              (*stdout->_bf._base == 0 ? '?' : *stdout->_bf._base )
              : ' ');
    printf_nb("---------------------------\n");
}

#ifdef print_stdio_buffer_info_nb
#error what a fuck ??
#else
#define info_nb \
print_stdio_buffer_info_nb();
#endif

int main() {
    
    char buf[12] = { 0 };
    setbuf(stdout, buf);
    
    printf("hello worl\n");
    
    _exit(0);
}
