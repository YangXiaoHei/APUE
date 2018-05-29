#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	
	time_t now = time(NULL);
	struct tm *now_tm = localtime(&now);
	if (now_tm == NULL) {
		printf("localtime fail\n");
		exit(1);
	}
	char buf[1024] = { 0 };
	if (strftime(buf, sizeof(buf), "%Y %b %d %a %H:%M:%S %Z", now_tm) == 0) {
		printf("strftime fail\n");
		exit(1);
	}
	printf("%s\n", buf);
	
	return 0;	
}