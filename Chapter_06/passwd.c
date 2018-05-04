#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>

/*
 struct passwd {
 char    *pw_name;         // user name  //
 char    *pw_passwd;         // encrypted password  //
 uid_t    pw_uid;             // user uid  //
 gid_t    pw_gid;             // user gid  //
 __darwin_time_t pw_change;         // password change time  //
 char    *pw_class;         // user access class  //
 char    *pw_gecos;         // Honeywell login info  //
 char    *pw_dir;         // home directory  //
 char    *pw_shell;         // default shell  //
 __darwin_time_t pw_expire;         // account expiration  //
 };
 
 */
int main(int argc, const char * argv[]) {
    
    uid_t uid = 501;
    
    if (argc == 2) {
        uid = atoi(argv[1]);
    }
    
    struct passwd *pd;
    pd = getpwuid(uid);
    
    printf("pw_name : %s\n", pd->pw_name);
    printf("pw_passwd : %s\n", pd->pw_name);
    
    printf("pw_uid : %ld\n", (long)pd->pw_uid);
    printf("pw_gid : %ld\n", (long)pd->pw_gid);
    printf("pw_class : %s\n", pd->pw_class);
    printf("pw_gecos : %s\n", pd->pw_gecos);
    printf("pw_dir : %s\n", pd->pw_dir);
    printf("pw_shell : %s\n", pd->pw_shell);
    
    return 0;
}
