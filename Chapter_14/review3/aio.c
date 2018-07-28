#include <stdio.h>
#include <unistd.h>
#include <aio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>

#define BSZ 4096
#define NBUF 8

enum rwop
{
    UNUSED = 0,
    READ_PENDING = 1,
    WRITE_PENDING = 2
};

struct aio_info
{
    enum rwop op;
    int last;
    struct aiocb aiocb;
    unsigned char data[BSZ];
};

struct aio_info bufs[NBUF];

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage : %s <infile><outfile>\n", argv[0]);
        exit(1);
    }

    int ifd, ofd;
    if ((ifd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("open error");
        exit(1);
    }
    if ((ofd = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0644)) < 0)
    {
        perror("open error");
        exit(1);
    }
    struct stat st;
    if (fstat(ifd, &st) < 0)
    {
        perror("fstat error");
        exit(1);
    }

    const struct aiocb *aiolist[NBUF];

    for (int i = 0; i < NBUF; i++)
    {
        bufs[i].op = UNUSED;
        bufs[i].aiocb.aio_buf = bufs[i].data;
        bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
        aiolist[i] = NULL;
    }

    int off = 0;
    int err = 0;
    int numop = 0;
    while (1)
    {
        for (int i = 0; i < NBUF; i++)
        {
            switch (bufs[i].op)
            {
                case UNUSED :
                {
                    if (off < st.st_size)
                    {
                        printf("UNUSED %d\n", i);

                        bufs[i].op = READ_PENDING;
                        bufs[i].aiocb.aio_fildes = ifd;
                        bufs[i].aiocb.aio_offset = off;
                        off += BSZ;
                        if (off > st.st_size)
                            bufs[i].last = 1;
                        bufs[i].aiocb.aio_nbytes = BSZ;
                        if (aio_read(&bufs[i].aiocb) < 0)
                        {
                            perror("aio_read error");
                            exit(1);
                        }
                        aiolist[i] = &bufs[i].aiocb;
                        numop++;
                    }
                } break;
                case READ_PENDING:
                {

                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                        continue;

                    if (err != 0)
                    {
                        if (err == -1)
                        {
                            perror("aio_error error");
                            exit(1);
                        }
                        else
                        {
                            perror("aio_read error");
                            exit(1);
                        }
                    }

                    printf("read finished %d\n", i);

                    int n = 0;
                    if ((n = aio_return(&bufs[i].aiocb)) < 0)
                    {
                        perror("aio_return error");
                        exit(1);
                    }
                    if (n != BSZ && bufs[i].last != 1)
                    {
                        perror("short read\n");
                        exit(1);
                    }
                    numop--;

                    bufs[i].aiocb.aio_fildes = ofd;
                    bufs[i].aiocb.aio_nbytes = n;
                    bufs[i].op = WRITE_PENDING;
                    if (aio_write(&bufs[i].aiocb) < 0)
                    {
                        perror("aio_write error");
                        exit(1);
                    }
                } break;
                case WRITE_PENDING:
                {
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                        continue;

                    if (err != 0)
                    {
                        perror("aio_error error");
                        exit(1);
                    }

                    printf("write finished %d\n", i);

                    int n = 0;
                    if ((n = aio_return(&bufs[i].aiocb)) < 0)
                    {
                        perror("aio_return error");
                        exit(1);
                    }
                    if (n != bufs[i].aiocb.aio_nbytes)
                    {
                        perror("short write");
                        exit(1);
                    }
                    // aiolist[i] = NULL;
                    bufs[i].op = UNUSED;
                    numop--;
                } break;
            }
        }
        if (numop == 0)
        {
            if (off > st.st_size)
                break;
        }
        else
        {
            if (aio_suspend(aiolist, NBUF, NULL) < 0)
            {
                perror("aio_suspend error");
                exit(1);
            }
        }
    }
    bufs[0].aiocb.aio_fildes = ofd;
    if (aio_fsync(O_SYNC, &bufs[0].aiocb) < 0)
    {
        perror("aio_fsync error");
        exit(1);
    }
    return 0;
}