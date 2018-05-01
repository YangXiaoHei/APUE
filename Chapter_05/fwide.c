#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 
 typedef    struct __sFILE {
 
 unsigned char *_p;         // current position in (some) buffer
 int      _r;               // read space left for getc()
 int      _w;               // write space left for putc()
 short    _flags;           // flags, below; this FILE is free if 0
 short    _file;            // fileno, if Unix descriptor, else -1
 struct   __sbuf _bf;       // the buffer (at least 1 byte, if !NULL)
 int      _lbfsize;         // 0 or -_bf._size, for inline putc
 
 // operations
 void    *_cookie;                       // cookie passed to io functions
 int    (* _Nullable _close)(void *);
 int    (* _Nullable _read) (void *, char *, int);
 fpos_t (* _Nullable _seek) (void *, fpos_t, int);
 int    (* _Nullable _write)(void *, const char *, int);
 
 // separate buffer for long sequences of ungetc()
 struct    __sbuf _ub;       // ungetc buffer
 struct __sFILEX *_extra;    // additions to FILE to not break ABI
 int    _ur;                 // saved _r when _r is counting ungetc data
 
 // tricks to meet minimum requirements even when malloc() fails
 unsigned char _ubuf[3];    // guarantee an ungetc() buffer
 unsigned char _nbuf[1];    // guarantee a getc() buffer
 
 // separate buffer for fgetln() when line crosses buffer boundary
 struct    __sbuf _lb;    // buffer for fgetln()
 
 // Unix stdio files get aligned to block boundaries on fseek()
 int    _blksize;      // stat.st_blksize (may be != _bf._size)
 fpos_t    _offset;    // current lseek offset (see WARNING)
 } FILE;
 
 
 // stdio buffers
 struct __sbuf {
 unsigned char    *_base;
 int               _size;
 };
 
 */

void FILE_print(FILE *fp) {
    if (fp == NULL) {
        return;
    }
    
    if (fp->_p != NULL) {
        printf("当前位置 : %hhd\n", *(char *)(fp->_p));
    }
    
    printf("剩余可以写的空间 : %ld\n", (long)fp->_w);
    printf("剩余可以读的空间 : %ld\n", (long)fp->_r);
    printf("fd : %ld\n", (long)fp->_file);
    
    struct __sbuf *buf = &fp->_bf;
    for (int i = 0; i < buf->_size; i++) {
        printf("%d", buf->_base[i]);
    }
    
    printf("best I/O block : %ld\n", (long)fp->_blksize);
    printf("offset : %ld\n", (long)fp->_offset);
}

int main(int argc, char *argv[]) {
    
    FILE *fp = fopen(argv[1], "r+");
    
    printf("fwide : %d\n", fwide(fp, 0));
    printf("fwide : %d\n", fwide(fp, -1));
    
    
    if (fp != NULL) {
        FILE_print(fp);
    }
    
    
    return 0;
}
