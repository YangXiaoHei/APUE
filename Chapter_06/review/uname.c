#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	
	struct utsname name_buf;
	if (uname(&name_buf) != 0) {
		printf("uname fail\n");
		exit(1);
	}

	printf( "sysname = %s\n"
			"nodename = %s\n"
			"release = %s\n"
			"version = %s\n"
			"machine = %s\n",
			name_buf.sysname,
			name_buf.nodename,
			name_buf.release,
			name_buf.version,
			name_buf.machine);
	
	return 0;	
}