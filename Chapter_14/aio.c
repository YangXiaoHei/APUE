#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <aio.h>
#include <sys/stat.h>

#define BSZ 4096
#define NBUF 8

enum rwop {
    UNUSED = 0,
    READ_PENDING = 1,
    WRITE_PENDING = 2
};

struct buf {
    enum rwop op;
    int     last;
    struct aiocb aiocb;
    unsigned char data[BSZ];
};

struct buf bufs[NBUF];
unsigned char translate(unsigned char c) {
    if (isalpha(c)) {
        if (c >= 'n') {
            c -= 13;
        } else if (c >= 'a') {
            c += 13;
        } else if (c >= 'N') {
            c -= 13;
        } else {
            c += 13;
        }
    }
    return c;
}

int main(int argc, char *argv[]) {
    
    int in_fd, out_fd, i, j, n, err, numop;
    struct stat st;
    const struct aiocb  *aiolist[NBUF];
    off_t off = 0;

    if (argc != 3) {
        printf("usage : %s infile outfile\n", argv[0]);
        exit(1);
    }

    if ((in_fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open error");
        exit(1);
    }
    if ((out_fd = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0644)) < 0) {
        perror("open error");
        exit(1);
    }
    if (fstat(in_fd, &st) < 0) {
        perror("fstat error");
        exit(1);
    }

    for (i = 0; i < NBUF; i++) {
        bufs[i].op = UNUSED;
        bufs[i].aiocb.aio_buf = bufs[i].data;
        bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
        aiolist[i] = NULL;
    }

    numop = 0;
    while (1) {
        for (i = 0; i < NBUF; i++) {
            switch(bufs[i].op) {
                case UNUSED: {
                    if (off < st.st_size) {
                        bufs[i].op = READ_PENDING;
                        bufs[i].aiocb.aio_fildes = in_fd;
                        bufs[i].aiocb.aio_offset = off;
                        off += BSZ;
                        if (off >= st.st_size) {
                            bufs[i].last = 1;
                        }
                        bufs[i].aiocb.aio_nbytes = BSZ;
                        if (aio_read(&bufs[i].aiocb) < 0) {
                            perror("aio_read failed");
                            exit(1);
                        }
                        aiolist[i] = &bufs[i].aiocb;
                        numop++;
                    }
                } break;

                case READ_PENDING: {
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS) {
                        continue;
                    }
                    if (err != 0) {
                        if (err == -1) {
                            perror("aio_err failed");
                            exit(1);
                        } else {
                            perror("aio_read other error");
                            exit(1);
                        }
                    }

                    if ((n = aio_return(&bufs[i].aiocb)) < 0) {
                        perror("aio_return error");
                        exit(1);
                    }
                    if (n != BSZ && !bufs[i].last) {
                        perror("aio short read error");
                        exit(1);
                    }
                    // for (j = 0; j < n; j++) {
                    //     bufs[i].data[j] = translate(bufs[i].data[j]);
                    // }
                    bufs[i].op = WRITE_PENDING;
                    bufs[i].aiocb.aio_fildes = out_fd;
                    bufs[i].aiocb.aio_nbytes = n;
                    if (aio_write(&bufs[i].aiocb) < 0) {
                        perror("aio_write error");
                        exit(1);
                    }

                } break;
                case WRITE_PENDING : { 
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS) {
                        continue;
                    }
                    if (err != 0) {
                        if (err == -1) {
                            perror("aio_error failed");
                            exit(1);
                        } else {
                            perror("aio_err other error");
                            exit(1);
                        }
                    }
                    if ((n = aio_return(&bufs[i].aiocb)) < 0) {
                        perror("aio_return error");
                        exit(1);
                    }
                    if (n != bufs[i].aiocb.aio_nbytes) {
                        perror("aio short write");
                        exit(1);
                    }
                    aiolist[i] = NULL;
                    bufs[i].op = UNUSED;
                    numop--;
                } break;
            }
        }
        if (numop == 0) {
            if (off >= st.st_size) break;
        } else {
            if (aio_suspend(aiolist, NBUF, NULL) < 0) {
                perror("aio_suspend failed");
                exit(1);
            }
        }
    }

    bufs[0].aiocb.aio_fildes = out_fd;
    if (aio_fsync(O_SYNC, &bufs[0].aiocb) < 0) {
        perror("aio_fsync error");
        exit(1);
    }
    
    return 0;    
}