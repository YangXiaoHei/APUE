#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <sys/stat.h>

/* 中断系统调用不自动重启的版本 */
void (*yh_signal(int signo, void (*sig_handler)(int)))(int) {
    struct sigaction act, oact;
    
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT   // Mac OS X 上居然没定义这个宏？！
    act.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &act, &oact) < 0) {
        return SIG_ERR;
    }
    return oact.sa_handler;
}

void sig_user1(int signo) {
    static unsigned long cnt = 0;  // 这个静态变量没有加锁保护，肯定是不准的
    printf("[%d] capture SIGUSER1 [seqno = %lu]\n",getpid(), cnt++);
}

int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        printf("[COMMAND_LINE_ARGS_ERR] /.restart [src_file] [dst_file]");
        exit(1);
    }
    
    umask(0);
    
    /* 创建一个日志文件用来记录打印 */
    int log_fd = open("./restart.log", O_CREAT | O_TRUNC | O_WRONLY | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("[CREAT_LOG_ERR]");
        exit(1);
    } else {
        dup2(log_fd, STDOUT_FILENO);
        dup2(log_fd, STDERR_FILENO);
    }
    
    /* 将下列标准 I/O 设置为无缓冲 */
    setbuf(stdout, NULL);
    
    /* 用于记录进程时间 */
    struct tms start, end;
    clock_t s, e;
    long clk;
    if ((clk = sysconf(_SC_CLK_TCK)) < 0) {  /* 获取嘀嗒数 */
        printf("[GET_CLK_ERR]");
        exit(1);
    }
    
    s = times(&start);
    
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("[FORK_ERR]");
        exit(1);
    } else if (pid > 0) {
        
        if (signal(SIGUSR1, sig_user1) == SIG_ERR) {
            printf("[SIG_REG_FAIL] signal handler register fail\n");
            exit(1);
        }
        
        /* 复制粘贴文件操作 */
        int src_fd = open(argv[1], O_RDONLY);
        int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
        if (src_fd < 0 || dst_fd < 0) {
            perror("[OPEN_SRC_DST_ERR]");
            exit(1);
        }
        
        char buf[1024];
        ssize_t coppied = 0, rl = 0, wl = 0;
        long i = 0;
        while ((rl = read(src_fd, buf, sizeof(buf))) > 0) {
            if (write(dst_fd, buf, rl) != rl) {
                printf("[WRITE_DST_ERR] coppied %zd bytes reason : %s", coppied, strerror(errno));
                exit(1);
            }
            coppied += rl;
        }
    
        if (rl == 0)
            printf("[COPY_SUCC] coppied %zd bytes\n", coppied);
         else
            printf("[READ_SRC_ERR] coppied %zd bytes reason : %s\n", coppied, strerror(errno));
    
    } else {
        
        /* 子进程先睡 1 秒，保证让父进程先执行... */
        sleep(1);
        printf("child process %d begin to send SIGUSR1\n", getpid());
        
        /* 不停的向父进程发 SIGUSR1 信号 */
        for (int i = 0; i < 100000; i++)
            kill(getppid(), SIGUSR1);
        
        exit(0);
    }
    
    int status;
    if (wait(&status) < 0) {
        perror("wait");
        exit(1);
    }
    
    e = times(&end);
    
    printf("wall time elapsed : %.2f\n", (e - s) * 1.0 / clk);
    printf("copy_proc user time elapsed : %.2f\n", (end.tms_utime - start.tms_utime) * 1.0 / clk);
    printf("copy_proc sys time elapsed : %.2f\n", (end.tms_stime - start.tms_stime) * 1.0 / clk);
    printf("chld_proc user time elapsed : %.2f\n", (end.tms_cutime - start.tms_cutime) * 1.0 / clk);
    printf("chld_proc sys time elapsed : %.2f\n", (end.tms_cstime - start.tms_cstime) * 1.0 / clk);
    return 0;
}
