#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/resource.h>
#include <signal.h>

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
        default:
            return -6;
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
    if (max_path_buf[cur_len - 1] != '/')
        max_path_buf[cur_len++] = '/';
    max_path_buf[cur_len] = 0;
    
    DIR *dp = NULL;
    struct dirent *dir_ren = NULL;
    if ((dp = opendir(max_path_buf)) == NULL) {
        __handle_one_file(pathname, &st, YH_OPEN_DIR_FAIL);
    } else {
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
    
    if (dp != NULL && closedir(dp) < 0)
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

void sig_int(int signo) {
    printf("\n【操作被打断！】\n"
           "目录 : %zd\n"
           "普通文件 : %zd\n"
           "fifo : %zd\n"
           "socket : %zd\n"
           "块特殊设备 : %zd\n"
           "字符特殊设备 : %zd\n"
           "符号链接 : %zd\n", ndir, nreg, nfifo, nsock, nblk, nchr, nlnk);
    exit(2);
}

void (*yh_signal(int signo, void (*sig_handler)(int))) (int) {
    struct sigaction new_act, old_act;
    new_act.sa_mask = 0;
    new_act.sa_handler = sig_handler;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
    new_act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        new_act.sa_flags |= SA_RESTART;
    }
    if (sigaction(signo, &new_act, &old_act) < 0)
        return SIG_ERR;
    return old_act.sa_handler;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("usage : %s <beg_file_path>\n", argv[0]);
        exit(1);
    }
    
    setbuf(stdout, NULL);
    
    if (yh_signal(SIGINT, sig_int) == SIG_ERR) {
        perror("signal error");
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

/* 下面是杨晗的电脑的统计信息~ */
/*
 
 /usr/sbin/authserver/ open dir fail : Permission denied
 /usr/local/openfire/ open dir fail : Permission denied
 /.Spotlight-V100/ open dir fail : Permission denied
 /Library/Application Support/Apple/ParentalControls/Users/ open dir fail : Permission denied
 /Library/Application Support/Apple/AssetCache/Data/ open dir fail : Permission denied
 /Library/Application Support/ApplePushService/ open dir fail : Permission denied
 /Library/Application Support/com.apple.TCC/ open dir fail : Permission denied
 /Library/SystemMigration/History/Migration-DFC589ED-9335-45CC-89A8-C0BFC4C1C0B9/QuarantineRoot/private/var/db/datadetectors/ open dir fail : Permission denied
 /Library/Caches/com.apple.iconservices.store/ open dir fail : Permission denied
 /.Trashes/ open dir fail : Permission denied
 /System/Library/DirectoryServices/DefaultLocalDB/Default/ open dir fail : Permission denied
 /System/Library/User Template/ open dir fail : Permission denied
 /System/Library/Caches/com.apple.receiptInstaller/ open dir fail : Permission denied
 /System/Library/Caches/com.apple.coresymbolicationd/ open dir fail : Permission denied
 /.fseventsd/ open dir fail : Permission denied
 /private/etc/cups/certs/ open dir fail : Permission denied
 /private/var/networkd/Library/ open dir fail : Permission denied
 /private/var/install/ open dir fail : Permission denied
 /private/var/ma/ open dir fail : Permission denied
 /private/var/spool/mqueue/ open dir fail : Permission denied
 /private/var/spool/postfix/saved/ open dir fail : Permission denied
 /private/var/spool/postfix/trace/ open dir fail : Permission denied
 /private/var/spool/postfix/defer/ open dir fail : Permission denied
 /private/var/spool/postfix/flush/ open dir fail : Permission denied
 /private/var/spool/postfix/deferred/ open dir fail : Permission denied
 /private/var/spool/postfix/corrupt/ open dir fail : Permission denied
 /private/var/spool/postfix/public/ open dir fail : Permission denied
 /private/var/spool/postfix/private/ open dir fail : Permission denied
 /private/var/spool/postfix/incoming/ open dir fail : Permission denied
 /private/var/spool/postfix/active/ open dir fail : Permission denied
 /private/var/spool/postfix/maildrop/ open dir fail : Permission denied
 /private/var/spool/postfix/hold/ open dir fail : Permission denied
 /private/var/spool/postfix/bounce/ open dir fail : Permission denied
 /private/var/spool/cups/ open dir fail : Permission denied
 /private/var/jabberd/ open dir fail : Permission denied
 /private/var/vm/.Trashes/ open dir fail : Permission denied
 /private/var/audit/ open dir fail : Permission denied
 /private/var/root/ open dir fail : Permission denied
 /private/var/lib/postfix/ open dir fail : Permission denied
 /private/var/db/Spotlight/ open dir fail : Permission denied
 /private/var/db/locationd/ open dir fail : Permission denied
 /private/var/db/net-snmp/ open dir fail : Permission denied
 /private/var/db/analyticsd/ open dir fail : Permission denied
 /private/var/db/nsurlsessiond/ open dir fail : Permission denied
 /private/var/db/fpsd/ open dir fail : Permission denied
 /private/var/db/hidd/ open dir fail : Permission denied
 /private/var/db/geod/ open dir fail : Permission denied
 /private/var/db/lockdown/ open dir fail : Permission denied
 /private/var/db/datadetectors/ open dir fail : Permission denied
 /private/var/db/cmiodalassistants/ open dir fail : Permission denied
 /private/var/db/com.apple.xpc.roleaccountd.staging/ open dir fail : Permission denied
 /private/var/db/dslocal/nodes/Default/ open dir fail : Permission denied
 /private/var/db/sudo/ open dir fail : Permission denied
 /private/var/db/dhcpclient/ open dir fail : Permission denied
 /private/var/db/timed/ open dir fail : Permission denied
 /private/var/db/applepay/ open dir fail : Permission denied
 /private/var/db/securityagent/ open dir fail : Permission denied
 /private/var/db/TokenCache/ open dir fail : Permission denied
 /private/var/db/ConfigurationProfiles/Setup/ open dir fail : Permission denied
 /private/var/db/ConfigurationProfiles/Store/ open dir fail : Permission denied
 /private/var/db/caches/opendirectory/ open dir fail : Permission denied
 /private/var/at/tabs/ open dir fail : Permission denied
 /private/var/at/tmp/ open dir fail : Permission denied
 /private/var/folders/59/7x7_pv9j5k94zs4mgcxx24d4000089/T/ open dir fail : Permission denied
 /private/var/folders/59/7x7_pv9j5k94zs4mgcxx24d4000089/C/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/0/com.apple.LaunchServices.dv/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/0/com.apple.notificationcenter/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/0/com.apple.nsurlsessiond/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/0/com.apple.routined/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/0/com.apple.pluginkit/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/0/com.apple.dock.launchpad/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/T/ open dir fail : Permission denied
 /private/var/folders/bj/lx41_tl569z5cgnqgjnn197w0000gp/C/ open dir fail : Permission denied
 /private/var/folders/kw/ydx08gbn0c528zc4gg11zfp8000086/T/ open dir fail : Permission denied
 /private/var/folders/kw/ydx08gbn0c528zc4gg11zfp8000086/C/ open dir fail : Permission denied
 /private/var/folders/q9/v2y_dc0j6x72ft33ft21bnym0000gp/T/ open dir fail : Permission denied
 /private/var/folders/q9/v2y_dc0j6x72ft33ft21bnym0000gp/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000b000002r/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000b000002r/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000000h000004/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000000h000004/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000104000081/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000104000081/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010r000086/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010r000086/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000sm00006d/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000sm00006d/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000t800006k/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000b400002s/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000b400002s/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000009800002_/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000y000007h/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000y000007h/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000008r000026/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000008r000026/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000c4000031/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000c4000031/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000yc00007l/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000003000000r/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000s800006_/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000s800006_/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010c000083/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010c000083/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000084000021/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000084000021/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000tm00006n/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000011800008_/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000011800008_/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010w000087/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010w000087/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000y800007k/0/com.apple.nsurlsessiond/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000y800007k/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000y800007k/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000108000082/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000108000082/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000114000089/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000114000089/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000z000007r/0/com.apple.notificationcenter/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000z000007r/0/com.apple.pluginkit/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000z000007r/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000z000007r/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000006w00001q/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000006w00001q/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000s0000068/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000s0000068/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000y400007j/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010m000085/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010m000085/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000th00006m/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000th00006m/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000vr00006y/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000vr00006y/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000xc00007b/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000xc00007b/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010h000084/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n000010h000084/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000yw00007q/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000bh00002w/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n00000bh00002w/C/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/0/com.apple.revisiond/com.apple.revisiond.temp/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/0/com.apple.revisiond.temp/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/0/com.apple.Spotlight/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/0/com.apple.pluginkit/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/0/com.apple.Spotlight.mds/com.apple.Spotlight/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/T/ open dir fail : Permission denied
 /private/var/folders/zz/zyxvpxvq6csfxvn_n0000000000000/C/ open dir fail : Permission denied
 /private/var/run/mds/ open dir fail : Permission denied
 /private/var/backups/ open dir fail : Permission denied
 /private/var/agentx/ open dir fail : Permission denied
 /.DocumentRevisions-V100/ open dir fail : Permission denied
 /Users/test_yanghan/Music/ open dir fail : Permission denied
 /Users/test_yanghan/Pictures/ open dir fail : Permission denied
 /Users/test_yanghan/Desktop/ open dir fail : Permission denied
 /Users/test_yanghan/Library/ open dir fail : Permission denied
 /Users/test_yanghan/Public/Drop Box/ open dir fail : Permission denied
 /Users/test_yanghan/Movies/ open dir fail : Permission denied
 /Users/test_yanghan/Documents/ open dir fail : Permission denied
 /Users/test_yanghan/Downloads/ open dir fail : Permission denied
 /Users/Guest/Music/ open dir fail : Permission denied
 /Users/Guest/Pictures/ open dir fail : Permission denied
 /Users/Guest/Desktop/ open dir fail : Permission denied
 /Users/Guest/Library/ open dir fail : Permission denied
 /Users/Guest/Public/Drop Box/ open dir fail : Permission denied
 /Users/Guest/Movies/ open dir fail : Permission denied
 /Users/Guest/Documents/ open dir fail : Permission denied
 /Users/Guest/Downloads/ open dir fail : Permission denied
 /dev/fd/3/ open dir fail : Not a directory
 /dev/fd/4/ open dir fail : Not a directory
 /dev/fd/5/ open dir fail : Not a directory
 目录 : 1029963
 普通文件 : 3281292
 fifo : 111
 socket : 28
 块特殊设备 : 9
 字符特殊设备 : 312
 符号链接 : 50849
 
 */





