#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <unistd.h>

static void pr_times(clock_t, struct tms *, struct tms *);
static void do_cmd(char *);

int main(int argc, char *argv[]) {
    
    int i;
    setbuf(stdout, NULL);
    
    for (i = 1; i < argc; ++i) {
        do_cmd(argv[i]);
    }
    
    return 0;
}

static void pr_times(clock_t real, struct tms *start, struct tms *end) {
    static long clk = 0;
    if (clk == 0) {
        if ((clk = sysconf(_SC_CLK_TCK)) < 0) {
            printf("sysconf error");
            exit(1);
        }
    }
    
    printf(" real : %7.2f\n", real * 1.0 / clk);
    printf(" user : %7.2f\n", (end->tms_utime - start->tms_utime) * 1.0 / clk);
    printf(" sys : %7.2f\n", (end->tms_stime - start->tms_stime) * 1.0 / clk);
    printf(" child user : %7.2f\n", (end->tms_cutime - start->tms_cutime) * 1.0 / clk);
    printf(" child sys : %7.2f\n", (end->tms_cstime - start->tms_cstime) * 1.0 / clk);
    
}

static void do_cmd(char *cmd) {
    
    struct tms tms_start, tms_end;
    clock_t start, end;
    int     status;
    
    printf("\n command : %s\n", cmd);
    
    if ((start = times(&tms_start)) == -1) {
        printf("times error");
        exit(1);
    }
    
    if ((status = system(cmd)) < 0) {
        printf("system() error");
        exit(1);
    }
    
    if ((end = times(&tms_end)) == -1) {
        printf("times error");
        exit(1);
    }
    
    pr_times(end - start, &tms_start, &tms_end);
}
