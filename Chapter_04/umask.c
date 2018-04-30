#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)


int main(int argc, char *argv[]) {
    
    umask(0);
    
    if (creat("foo", RWRWRW) < 0) {
        perror("creat ");
        exit(1);
    }
    
//    umask(0066);
//    umask(S_IWGRP | S_IXGRP | S_IRWXO);
    umask(S_IRWXO);
    
    if (creat("bar", RWRWRW) < 0) {
        perror("creat");
        exit(1);
    }
    
    return 0;
}
