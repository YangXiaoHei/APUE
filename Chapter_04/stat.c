#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    struct stat st;
    
    if (stat(argv[1], &st) < 0) {
        perror("stat ");
        exit(1);
    }
    
    char mode_str[64] = { 0 };
    long mode = st.st_mode;
    sprintf(mode_str,
            "%c%c%c %c%c%c %c%c%c",
            
            /* user */
            (mode & S_IRUSR) ? 'r' : '-',
            (mode & S_IWUSR) ? 'w' : '-',
            ((mode & S_IXUSR) ? 'x' : ((mode & S_ISUID) ? 's' : ((mode & S_ISGID) ? 'S' : '-'))),
            
            /* group */
            (mode & S_IRGRP) ? 'r' : '-',
            (mode & S_IRGRP) ? 'w' : '-',
            ((mode & S_IXGRP) ? 'x' : ((mode & S_ISUID) ? 's' : ((mode & S_ISGID) ? 'S' : '-'))),
            
            /* other */
            (mode & S_IROTH) ? 'r' : '-',
            (mode & S_IROTH) ? 'w' : '-',
            ((mode & S_IXOTH) ? 'x' : ((mode & S_ISUID) ? 's' : ((mode & S_ISGID) ? 'S' : '-')))
            
            );
    
    printf("st_mode = %d\n", st.st_mode);
    printf("st_mode = %s\n", mode_str);
    printf("st_ino = %ld\n", st.st_ino);
    printf("st_dev = %ld\n", st.st_dev);
    printf("st_rdev = %ld\n", st.st_rdev);
    printf("st_nlink = %ld\n", st.st_nlink);
    printf("st_uid = %ld\n", st.st_uid);
    printf("st_gid = %ld\n", st.st_gid);
    printf("st_size = %ld\n", st.st_size);
    
    char atime[64], mtime[64], ctime[64];
    bzero(atime, sizeof(atime));
    bzero(mtime, sizeof(mtime));
    bzero(ctime, sizeof(ctime));
    
    struct tm *at = NULL;
    struct tm *mt = NULL;
    struct tm *ct = NULL;
    
    at = localtime(&st.st_atimespec.tv_sec);
    mt = localtime(&st.st_mtimespec.tv_sec);
    ct = localtime(&st.st_ctimespec.tv_sec);
    
    sprintf(atime,
            "%-4d %-2d %-2d %2d:%2d:%02d\n",
            at->tm_year + 1900,
            at->tm_mon + 1,
            at->tm_mday,
            at->tm_hour,
            at->tm_min,
            at->tm_sec);
    
    sprintf(mtime,
            "%-4d %-2d %-2d %2d:%2d:%02d\n",
            mt->tm_year + 1900,
            mt->tm_mon + 1,
            mt->tm_mday,
            mt->tm_hour,
            mt->tm_min,
            mt->tm_sec);
    
    sprintf(ctime,
            "%-4d %-2d %-2d %2d:%2d:%02d\n",
            ct->tm_year + 1900,
            ct->tm_mon + 1,
            ct->tm_mday,
            ct->tm_hour,
            ct->tm_min,
            ct->tm_sec);
    
    
    printf("st_atime = %s", atime);
    printf("st_mtime = %s", mtime);
    printf("st_ctime = %s", ctime);
    
    printf("st_blksize = %ld\n", st.st_blksize);
    printf("st_blocks = %ld\n", st.st_blocks);
    
    return 0;
}
