#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>

void print_pwd(struct passwd *pwd) {
	printf( "pw_name = %s\n"
			"pw_passwd = %s\n"
			"pw_uid = %ld\n"
			"pw_gid = %ld\n"
			"pw_gecos = %s\n"
			"pw_dir = %s\n"
			"pw_shell = %s\n"
			"pw_class = %s\n",
			pwd->pw_name,
			pwd->pw_passwd,
			(long)pwd->pw_uid,
			(long)pwd->pw_gid,
			pwd->pw_gecos,
			pwd->pw_dir,
			pwd->pw_shell,
			pwd->pw_class
		);
}

struct passwd *yh_getpwuid(const char *name) {
	struct passwd *pwd = NULL;
	setpwent();
	while ((pwd = getpwent()) != NULL) 
		if (strcmp(name, pwd->pw_name) == 0) 
			break;
	endpwent();
	return pwd;
}

int main(int argc, char *argv[]) {
		
	if (argc != 2) {
		printf("./a.out name\n");
		exit(1);
	}	
	print_pwd(yh_getpwuid(argv[1]));
	
	return 0;	
}