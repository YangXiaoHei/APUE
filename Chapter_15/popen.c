
static pid_t *child_pid = NULL;
static int max_fd;

FILE *popen(const char *cmdstring, const char *type) {
    pid_t pid;
    FILE *fp;
    int i;
    int pfd[2];

    if((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
        errno = EINVAL;
        return NULL;
    }

    if (child_pid == NULL) {
        max_fd = 256;
        if ((child_pid = calloc(max_fd, sizeof(pid_t))) == NULL) 
            return NULL;
    }

    if (pipe(pfd) < 0) {
        perror("pipe error");
        exit(1);
    }

    if (pfd[0] >= max_fd || pfd[1] >= max_fd) {
        close(pfd[0]);
        close(pfd[1]);
        errno = EMFILE;
        return NULL;
    }

    if ((pid = fork()) < 0) {
        return NULL;
    } else if (pid == 0) {
        if (*type == 'r') {
            /*
             *  父进程要读子进程的输出
             *  
             *  让子进程把标准输出重定向到管道，这样
             *  子进程写入到标准输出中的数据，都被写入了管道
             *  于是父进程可以从管道的另一头读到这些数据
             */
            close(pfd[0]); 
            if (pfd[1] != STDOUT_FILENO) {
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]); 
            }
        } else { 

            /*
             *  父进程想要将数据传到子进程的输入端
             *  
             *  那么只需让子进程将标准输入重定向到管道，
             *  那么当父进程将数据写入到管道，子进程就可以从
             *  标准输入读到这些数据
             *
             */
            close(pfd[1]); 
            if (pfd[0] != STDIN_FILENO) {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]); 
            }
        }

        for (i = 0; i < max_fd; i++) 
            if (child_pid[i] > 0) 
                close(i);

        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }

    /*
     * 父进程把管道用不到的一端关掉
     */
    if (*type == 'r') {
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL) {
            return NULL;
        }
    } else {
        close(pfd[0]);
        if ((fp = fdopen(pfd[1], type)) == NULL) {
            return NULL;
        }
    }

    child_pid[fileno(fp)] = pid;
    return fp;
}

int pclose(FILE *fp) {

    int fd, stat;
    pid_t pid;

    /*
     * 从来没有调用过 popen
     */
    if (child_pid == NULL) {
        errno = EINVAL;
        return -1;
    }

    /*
     * 无效的文件描述符
     */
    fd = fileno(fp);
    if (fd >= max_fd) {
        errno = EINVAL;
        return -1;
    }

    /*
     * 这个流对象不是被 popen 打开的
     */
    if ((pid = child_pid[fd]) == 0) {
        errno = EINVAL;
        return -1;
    }

    child_pid[fd] = 0;
    if (fclose(fp) == EOF) {
        return -1;
    }

    /*
     *  waitpid 的阻塞如果被信号所打断，那么 errno 被置为 EINTR
     *  在这种情况下，重新调用 waitpid
     *  如果子进程已经不存在（比如在父进程信号捕获函数中被 wait 掉），
     *  那么 errno 被置为 ECHLD，此时就返回 -1
     */
    while (waitpid(pid, &stat, 0) < 0) {
        if (errno != EINTR) {
            return -1;
        }
    }
    return stat;
}


int main(int argc, char *argv[]) {
    
    
    
    return 0;    
}