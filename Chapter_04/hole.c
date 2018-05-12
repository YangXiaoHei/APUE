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
    
    char alphabet[] = "abc";
    size_t l = strlen(alphabet);
    if (atoi(argv[2]) < 2 * l) {
        printf("[ERROR] 创建的文件太小...\n");
        exit(1);
    }
    
    const char *prefix = argv[1];
    int nbytes = atoi(argv[2]);
    
    char hole[64] = { 0 };
    char nohole[64] = { 0 };
    
    snprintf(hole, sizeof(hole), "%s_hole", prefix);
    snprintf(nohole, sizeof(nohole), "%s_no_hole", prefix);
    
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
    
    
    int n = nbytes, i = 0;
    
    /* 无空洞 */
    while (n--) {
        size_t cur = i++ % l;
        if (write(nohfd, alphabet + cur, 1) != 1) {
            perror("write ");
            exit(1);
        }
    }
    
    
    /* 有空洞 */
    if (write(hfd, alphabet, l) < 0) {
        perror("write ");
        exit(1);
    }
    lseek(hfd, nbytes - 2 * l, SEEK_CUR);
    if (write(hfd, alphabet, l) < 0) {
        perror("write ");
        exit(1);
    }
    
    return 0;
}
