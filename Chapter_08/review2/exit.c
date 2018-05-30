#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int glob = 3;

int main(int argc, char *argv[]) {
	
	int var = 0;

	pid_t pid;
	if ((pid = vfork()) < 0) {
		perror("vfork error");
		exit(1);
	} else if (pid == 0) {
		var++;
		glob++;
		// exit(0);
		fclose(stdout);
		exit(0);
	}

	sleep(2);

	printf("parent process var = %d glob = %d\n", var, glob);
	
	return 0;	
}