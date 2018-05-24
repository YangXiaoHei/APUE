#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("[ERROR] usage : ./date [days] \n");
        exit(1);
    }
    
    setbuf(stdout, NULL);
    
    time_t cur_time = time(NULL);
    
    long days = atol(argv[1]);
    if (days < 0 || days >= cur_time / (24 * 60 * 60)) {
        printf("[ERROR] days args invalid, please retry\n");
        exit(1);
    }
    
    /* localtime 返回的是自己内部维护的静态变量地址，
       不是堆上分配的，无需释放。但这同时意味着，
       struct tm 的数据会被覆盖.... */
    
    time_t days_ago = cur_time - (days * 24 * 60 * 60);
    struct tm *tm_now = localtime(&cur_time);
    char buf1[256] = { 0 };
    snprintf(buf1, sizeof(buf1), "%-d/%-d/%-d %02d:%02d:%02d",
             tm_now->tm_year + 1900,
             tm_now->tm_mon + 1,
             tm_now->tm_mday,
             tm_now->tm_hour,
             tm_now->tm_min,
             tm_now->tm_sec);
    
    char buf2[256] = { 0 };
    struct tm *tm_ago = localtime(&days_ago);
    snprintf(buf2, sizeof(buf2), "%-d/%-d/%-d %02d:%02d:%02d",
             tm_ago->tm_year + 1900,
             tm_ago->tm_mon + 1,
             tm_ago->tm_mday,
             tm_ago->tm_hour,
             tm_ago->tm_min,
             tm_ago->tm_sec);
    
    printf("current date : %s\n"
           "%ld days ago : %s\n", buf1, days, buf2);
    return 0;
}
