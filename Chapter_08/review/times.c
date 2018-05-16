#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <fcntl.h>

int main() {
    
    struct tms start, end;
    clock_t s, e;
    
    pid_t pid;
    int status;
    
    int fd = open("./haha", O_CREAT | O_TRUNC | O_APPEND | O_RDWR, 0644);
    
    setbuf(stdout, NULL);
    
    long clk = sysconf(_SC_CLK_TCK);
    
    if ((s = times(&start)) == -1) {
        printf("times\n");
        exit(1);
    }
    
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        for (int i = 0; i < 300000; i++) {
            write(fd, "a", 1);
        }
        exit(0);
    }
    
    int sum = 1;
    for (int i = 0; i < 1000000; i++) {
        write(fd, "a", 1);
    }
    
    if (wait(&status) < 0) {
        perror("wait");
        exit(1);
    }
    
    if ((e = times(&end)) == -1) {
        printf("times\n");
        exit(1);
    }
    
    printf("real time  %.3f\n", (e - s) * 1.0 / clk);
    printf("user CPU time  %.3f\n", (end.tms_utime - start.tms_utime) * 1.0 / clk);
    printf("system CPU time  %.3f\n", (end.tms_stime - start.tms_stime) * 1.0 / clk);
    printf("child user CPU time  %.3f\n", (end.tms_cutime - start.tms_cutime) * 1.0 / clk);
    printf("child system CPU time  %.3f\n", (end.tms_cstime - start.tms_cstime) * 1.0 / clk);
    
    
    return 0;
}
