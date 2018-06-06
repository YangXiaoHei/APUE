#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * 用两个管道实现的 UNPv2 4.2 描述的客户端和服务器通信程序
 */
int main(int argc, char *argv[]) {
    
    int p_cw_pr[2], p_cr_pw[2];
    if (pipe(p_cw_pr) < 0 || pipe(p_cr_pw) < 0) {
        perror("pipe error");
        exit(1);
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    } else if (pid == 0) {  /* 子进程 */
        close(p_cr_pw[1]);
        close(p_cw_pr[0]);

        char file_name[1024] = { 0 };
        if (read(p_cr_pw[0], file_name, sizeof(file_name)) < 0) {
            perror("child proc read error");
            exit(1);
        }
        printf("child file_name = %s\n", file_name);

        int fd = open(file_name, O_RDONLY);
        if (fd < 0) {
            perror("child proc open error");
            exit(1);
        }
        struct stat st;
        if (fstat(fd, &st) < 0) {
            perror("child proc fstat error");
            exit(1);
        }

        char *file_buf = malloc(st.st_blksize);
        ssize_t rd_len = 0;
        while ((rd_len = read(fd, file_buf, st.st_blksize)) > 0) {
            if (write(p_cw_pr[1], file_buf, rd_len) != rd_len) {
                perror("child proc write error");
                exit(1);
            }
        }
        if (rd_len < 0) {
            perror("child proc final read error");
            exit(1);
        }
        exit(0);

    } else {  /* 父进程 */
        close(p_cw_pr[1]);
        close(p_cr_pw[0]);

        char file_name[1024] = { 0 };
        if (fgets(file_name, sizeof(file_name), stdin) != NULL) {
            ssize_t len = strlen(file_name);
            if (file_name[len - 1] == '\n') {
                file_name[len - 1] = 0;
                len--;
            }
            if (write(p_cr_pw[1], file_name, len) != len) {
                perror("parent write error");
                exit(1);
            }
        }

        char buf[1024] = { 0 };
        ssize_t rd_len = 0;
        while ((rd_len = read(p_cw_pr[0], buf, sizeof(buf))) > 0) {
            if (fputs(buf, stdout) == EOF) {
                if (ferror(stdout)) {
                    printf("parent fputs error\n");
                    exit(1);
                }
            }
        }

        int status;
        if (waitpid(pid, &status, 0) < 0) {
            perror("parent waitpid error");
            exit(1);
        }
    }

    
    return 0;    
}