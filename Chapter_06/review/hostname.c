#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	
	char name[1024] = { 0 };
	if (gethostname(name, sizeof(name)) != 0) {
		printf("gethostname fail\n");
		exit(1);
	}
	printf("gethostname = %s\n", name);
	
	return 0;	
}