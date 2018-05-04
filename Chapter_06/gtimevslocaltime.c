#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

 /*
 *  struct tm {
 int    tm_sec;         // seconds after the minute [0-60]  //
int    tm_min;         // minutes after the hour [0-59]  //
int    tm_hour;     // hours since midnight [0-23]  //
int    tm_mday;     // day of the month [1-31]  //
int    tm_mon;         // months since January [0-11]  //
int    tm_year;     // years since 1900  //
int    tm_wday;     // days since Sunday [0-6]  //
int    tm_yday;     // days since January 1 [0-365]  //
int    tm_isdst;     // Daylight Savings Time flag  //
long    tm_gmtoff;     // offset from UTC in seconds  //
char    *tm_zone;     // timezone abbreviation  //
};

 */

void printftm(struct tm *t) {
    printf("---------------------\n");
    printf("tm_sec = %d \n"
           "tm_min = %d \n"
           "tm_hour = %d \n"
           "tm_mday = %d \n"
           "tm_mon = %d \n"
           "tm_year = %d \n"
           "tm_wday = %d \n"
           "tm_yday = %d \n",
           t->tm_sec,
           t->tm_min,
           t->tm_hour,
           t->tm_mday,
           t->tm_mon + 1,
           t->tm_year + 1900,
           t->tm_wday != 0 ? t->tm_wday : 7,
           t->tm_yday);
    printf("---------------------\n");
}


int main() {
    
    struct tm *gt;
    struct tm *lt;
    
    time_t cur = time(NULL);
    
    if ((gt = gmtime(&cur)) == NULL) {
        printf("gtime fail\n");
        return -1;
    }
    
    if ((lt = localtime(&cur)) == NULL) {
        printf("localtime fail\n");
        return -1;
    }
    
    printftm(gt);
    printftm(lt);
    
    return 0;
}
