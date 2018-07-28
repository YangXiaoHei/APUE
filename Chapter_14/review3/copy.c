#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <poll.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

#define COPY_SIZE (1L << 30) // 1G

int main(int argc, char const *argv[])
{
   if (argc != 3)
   {
        printf("usage : %s <infile> <outfile>\n", argv[0]);
        exit(1);
   }     

   int ifd, ofd;
   if ((ifd = open(argv[1], O_RDONLY)) < 0) 
   {
        perror("open error");
        exit(1);
   }

   if ((ofd = open(argv[2], O_RDWR | O_TRUNC | O_CREAT, 0644)) < 0)
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

   lseek(ofd, st.st_size, SEEK_SET);
   write(ofd, "", 1);

   char *src, *dst;

   int copysz = 0;
   int fsz = 0;
   while (fsz < st.st_size)
   {
        if (st.st_size - fsz > COPY_SIZE)
            copysz = COPY_SIZE;
        else
            copysz = st.st_size - fsz;

        if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, ifd, fsz)) == MAP_FAILED)
        {
            perror("mmap error");
            exit(1);
        }
        if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, ofd, fsz)) == MAP_FAILED)
        {
            perror("mmap error");
            exit(1);
        }
        close(ifd);
        close(ofd);
        memcpy(dst, src, copysz);
        munmap(src, copysz);
        munmap(dst, copysz);
        fsz += copysz;
   }
    return 0;
}