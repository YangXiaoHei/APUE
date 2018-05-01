#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    
    struct stat st;
    
    if (lstat(argv[1], &st) < 0) {
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
    
    printf("\n[file_name = %s]\n", argv[1]);
    if (S_ISREG(st.st_mode)) {
        printf("[file_type = regular]\n");
    } else if (S_ISSOCK(st.st_mode)) {
        printf("[file_type = socket]\n");
    } else if (S_ISDIR(st.st_mode)) {
        printf("[file_type = directory]\n");
    } else if (S_ISBLK(st.st_mode)) {
        printf("[file_type = block special]\n");
    } else if (S_ISFIFO(st.st_mode)) {
        printf("[file_type = fifo]\n");
    } else if (S_ISCHR(st.st_mode)) {
        printf("[file_type = character special]\n");
    } else if (S_ISLNK(st.st_mode)) {
        printf("[file_type = symbolic link]\n");
    } else {
        printf("[file_type = unknown mode]\n");
    }
    
    printf("st_mode = %ld\n", (long)st.st_mode);
    printf("st_mode = %s\n", mode_str);
    printf("st_ino = %ld\n", (long)st.st_ino);
    printf("st_dev = %ld\n", (long)st.st_dev);
    printf("st_rdev = %ld\n", (long)st.st_rdev);
    printf("st_nlink = %ld\n", (long)st.st_nlink);
    printf("st_uid = %ld\n", (long)st.st_uid);
    printf("st_gid = %ld\n", (long)st.st_gid);
    printf("st_size = %ld\n", (long)st.st_size);
    
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
    
    
    printf("atime = %ld  %ld\n", (long)st.st_atimespec.tv_sec, (long)st.st_atimespec.tv_nsec);
    printf("mtime = %ld  %ld\n", (long)st.st_mtimespec.tv_sec, (long)st.st_atimespec.tv_nsec);
    printf("ctime = %ld  %ld\n", (long)st.st_ctimespec.tv_sec, (long)st.st_atimespec.tv_nsec);
    
    printf("st_atime = %s", atime);
    printf("st_mtime = %s", mtime);
    printf("st_ctime = %s", ctime);
    
    printf("st_blksize = %ld\n", (long)st.st_blksize);
    printf("st_blocks = %ld\n", (long)st.st_blocks);
    printf("\n");
    
    return 0;
}
