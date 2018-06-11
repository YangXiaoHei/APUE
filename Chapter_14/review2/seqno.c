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

void lock(int fd) {

}

void unlock(int fd) {

}

#define SEQ_FILE "./file"


int main(int argc, char *argv[]) {
    
    int fd;
    if (access(SEQ_FILE, F_OK) == 0) {
        if ((fd = open(SEQ_FILE, O_RDWR)) < 0) {
            perror("open error");
            exit(1);
        }
    } else {
        if ((fd = open(SEQ_FILE, O_CREAT | O_RDWR, 0644)) < 0) {
            perror("open error");
            exit(1);
        }
        if (write(fd, "1", 1) != 1) {
            perror("write error");
            exit(1);
        }
        if (lseek(fd, 0, SEEK_SET) < 0) {
            perror("lseek error");
            exit(1);
        }
    }
    

    char buf[1024];
    ssize_t n;
    int seqno = 0;
    for (int i = 0; i < 100000; i++) {
        lock(fd);

        if (lseek(fd, 0, SEEK_SET) < 0) {
            perror("lseek error");
            exit(1);
        }
        n = read(fd, buf, sizeof(buf));
        buf[n] = 0;

        n = sscanf(buf, "%d\n", &seqno);
        if (n != 1) {
            printf("sscanf error");
            exit(1);
        }
        printf("pid = %ld #seq = %d\n", (long)getpid(), seqno);

        seqno++;

        snprintf(buf, sizeof(buf), "%d\n", seqno);
        n = strlen(buf);

        if (lseek(fd, 0, SEEK_SET) < 0) {
            perror("lseek error");
            exit(1);
        }
        if (write(fd, buf, n) != n) {
            perror("write error");
            exit(1);
        }


        unlock(fd);
    }




    
    return 0;    
}