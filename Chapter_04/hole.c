#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("useage : ./hole [path_prefix] [total_bytes]\n");
        exit(1);
    }
    
    const char *path_name = argv[1];
    int nbytes = atoi(argv[2]);
    
    char hole[64] = { 0 };
    char nohole[64] = { 0 };
    
    sprintf(hole, "hole_%s", path_name);
    sprintf(nohole, "no_hole_%s", path_name);
    
    umask(0);
    
    int hfd = 0, nohfd = 0;
    if ((hfd = creat(hole, 0740)) < 0) {
        perror("creat ");
        exit(1);
    }
    
    if ((nohfd = creat(nohole, 0740)) < 0) {
        perror("creat ");
        exit(1);
    }
    
    char * alphabet = "abcdefghijklmnopqrstuvwxyz";
    char * p = alphabet;
    int n = nbytes, i = 0;
    while (n--) {
        if (write(nohfd, p + (i++ % strlen(alphabet)), 1) < 0) {
            perror("write ");
            exit(1);
        }
    }
    
    if (write(hfd, alphabet, strlen(alphabet)) < 0) {
        perror("write ");
        exit(1);
    }
    lseek(hfd, nbytes - strlen(alphabet), SEEK_SET);
    if (write(hfd, alphabet, strlen(alphabet)) < 0) {
        perror("write ");
        exit(1);
    }
    
    return 0;
}
