#include <stdio.h>
#include <sys/acct.h>
#include <stdlib.h>

static unsigned long compt2ulong(comp_t comptime) {
    unsigned long var;
    int           exp;
    var = comptime & 0x1FFF;
    exp = comptime >> 13 & 7;
    while (exp--)
        var <<= 3;
    return var;
}

int main(int argc, const char * argv[]) {
    
    struct acct acdata;
    
    FILE *fp;
    
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("fopen fail\n");
        exit(1);
    }
    
    while (fread(&acdata, sizeof(acdata), 1, fp) == 1) {
        printf("user_time = %zd"
               " system time = %zd"
               " elapsed time = %zd"
               " uid = %ld"
               " gid = %ld"
               " average memory usage = %ld"
               " count of IO blocks = %ld"
               " controlling tty = %ld"
               " flag = %c %c %c %c\n",
               compt2ulong(acdata.ac_utime),
               compt2ulong(acdata.ac_stime),
               compt2ulong(acdata.ac_etime),
               (long)acdata.ac_uid,
               (long)acdata.ac_gid,
               (long)acdata.ac_mem,
               compt2ulong(acdata.ac_io),
               (long)acdata.ac_tty,
               
               acdata.ac_flag & ACORE ? 'D' : ' ',
               acdata.ac_flag & AXSIG ? 'X' : ' ',
               acdata.ac_flag & AFORK ? 'F' : ' ',
               acdata.ac_flag & ASU ? 'S' : ' ');
    }
    
    return 0;
}
