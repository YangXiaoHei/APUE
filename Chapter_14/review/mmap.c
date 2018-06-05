
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>

#define COPY_MAX (1 << 30) /* 1GB */

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("usage : %s src dst\n", argv[0]);
        exit(1);
    }

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0) {
        perror("src file open error");
        exit(1);
    }
    int dst_fd = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (dst_fd < 0) {
        perror("dst file creat error");
        exit(1);
    }

    struct stat st;
    if (fstat(src_fd, &st) < 0) {
        perror("src file get stat fail");
        exit(1);
    }

    if (lseek(dst_fd, st.st_size, SEEK_SET) < 0) {
        perror("lseek error");
        exit(1);
    }
    write(dst_fd, "a", 1);
    lseek(dst_fd, 0, SEEK_SET);

    size_t map_size = 0, cursor = 0;
    char *read_begin = NULL, *write_begin = NULL;
    while (cursor < st.st_size) {
        if (st.st_size - cursor > COPY_MAX) {
            map_size = COPY_MAX;
        } else {
            map_size = st.st_size - cursor;
        }

        if ((read_begin = mmap(0, 
            map_size, 
            PROT_READ, 
            MAP_SHARED,
            src_fd, 
            cursor)) == MAP_FAILED) {
            perror("mmap error");
            exit(1);
        }
        if ((write_begin = mmap(0, 
            map_size, 
            PROT_WRITE | PROT_READ, 
            MAP_SHARED, 
            dst_fd, 
            cursor)) == MAP_FAILED) {
            perror("mmap error");
            exit(1);
        }
        memcpy(write_begin, read_begin, map_size);
        munmap(read_begin, map_size);
        munmap(write_begin, map_size);

        cursor += map_size;
    }

    
    return 0;    
}