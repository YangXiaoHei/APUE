#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>


int main(int argc, char *argv[]) {
	int err;
	pthread_t t1, t2;

	if ((err = phtread_create(&t1, NULL, run1, NULL)) != 0)
	{
		printf("pthread_create fail : %s\n");
		exit(1);
	}
	if ((err = phtread_create(&t1, NULL, run1, NULL)) != 0)
	{
		printf("pthread_create fail : %s\n");
		exit(1);
	}

	return 0;
}