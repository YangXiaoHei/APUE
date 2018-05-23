#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/resource.h>

#ifndef MAX_NAME
#define MAX_NAME 256
#endif

#define YH_FILE 1
#define YH_ISDIR 2
#define YH_STAT_FAIL 3
#define YH_OPEN_DIR_FAIL 4
#define YH_MEMORY_FAIL 5
#define YH_CLOSE_DIR_FAIL 6

static size_t max_path_len;
static char *max_path_buf;
static size_t ndir,  // directory
              nchr,  // special character
              nsock, // socket
              nreg,  // regular
              nlnk,  // symbolic link
              nfifo, // fifo
              nblk;  // special block

static int __handle_one_file(const char *pathname, struct stat *st, int flag) {
    switch (flag) {
        case YH_FILE:
            if (S_ISREG(st->st_mode))       nreg++;
            else if (S_ISCHR(st->st_mode))  nchr++;
            else if (S_ISBLK(st->st_mode))  nblk++;
            else if (S_ISLNK(st->st_mode))  nlnk++;
            else if (S_ISSOCK(st->st_mode)) nsock++;
            else if (S_ISFIFO(st->st_mode)) nfifo++;
            else {
                printf("%s unkonw fail\n", pathname);
                return -1;
            }
            break;
        case YH_ISDIR:
            ndir++;
            break;
        case YH_STAT_FAIL:
            printf("%s get stat fail : %s\n", pathname, strerror(errno));
            return -2;
        case YH_OPEN_DIR_FAIL:
            printf("%s open dir fail : %s\n", pathname, strerror(errno));
            return -3;
        case YH_MEMORY_FAIL:
            printf("realloc fail, current level : %s\n", pathname);
            exit(1);
        case YH_CLOSE_DIR_FAIL:
            printf("close dir fail, current level : %s\n", pathname);
            return -5;
        default: return -6;
    }
    return 0;
}

static int recursivly_go_deep_from(const char *pathname) {
    
    struct stat st;
    if (lstat(pathname, &st) < 0)
        return __handle_one_file(pathname, &st, YH_STAT_FAIL);
    
    if (S_ISDIR(st.st_mode) == 0)
        return __handle_one_file(pathname, &st, YH_FILE);
    
    int ret = 0;
    if ((ret = __handle_one_file(pathname, &st, YH_ISDIR)) < 0)
        return ret;
     
    size_t cur_len = strlen(max_path_buf);
    if (cur_len + MAX_NAME + 2 > max_path_len) {
        max_path_len *= 2;
        max_path_buf = realloc(max_path_buf, max_path_len);
        if (max_path_buf == NULL)
            return __handle_one_file(pathname, &st, YH_MEMORY_FAIL);
    }
    max_path_buf[cur_len++] = '/';
    max_path_buf[cur_len] = 0;
    
    DIR *dp = NULL;
    struct dirent *dir_ren = NULL;
    if ((dp = opendir(max_path_buf)) != NULL) {
        while ((dir_ren = readdir(dp)) != NULL) {
            if (strcmp(dir_ren->d_name, ".")  == 0 ||
                strcmp(dir_ren->d_name, "..") == 0)
                continue;
           
            strncpy(max_path_buf + cur_len, dir_ren->d_name, strlen(dir_ren->d_name));
            max_path_buf[cur_len + strlen(dir_ren->d_name)] = 0;
            
            if (recursivly_go_deep_from(max_path_buf) != 0)
                break;
        }
    }
    
    if (dp == NULL)
        return __handle_one_file(pathname, &st, YH_OPEN_DIR_FAIL);
    
    if (closedir(dp) < 0)
        return __handle_one_file(pathname, &st, YH_CLOSE_DIR_FAIL);
    
    return 0;
}

int myftw(const char *pathname) {
    if (pathname == NULL) {
        printf("pathname is NULL!\n");
        return 1;
    }
    max_path_buf = malloc(1024);
    max_path_len = 1024;
    strncpy(max_path_buf, pathname, strlen(pathname));
    max_path_buf[strlen(pathname)] = 0;
    int ret = recursivly_go_deep_from(pathname);
    free(max_path_buf);
    max_path_len = 0;
    return ret;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("./a.out [beg_file_path]\n");
        exit(1);
    }
    
    if (myftw(argv[1]) < 0) {
        printf("myftw fail\n");
        exit(1);
    }
    
    printf("目录 : %zd\n"
           "普通文件 : %zd\n"
           "fifo : %zd\n"
           "socket : %zd\n"
           "块特殊设备 : %zd\n"
           "字符特殊设备 : %zd\n"
           "符号链接 : %zd\n", ndir, nreg, nfifo, nsock, nblk, nchr, nlnk);
    
    return 0;
}






