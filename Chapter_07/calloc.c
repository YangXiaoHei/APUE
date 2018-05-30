#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	
	long *a = calloc(10, sizeof(long));
	for (int i = 0; i < 10; i++) {
		printf("%ld\n", a[i]);
	}

	printf("\n-----------------\n");

	char **b = calloc(10, sizeof(char *));
	for (int i = 0; i < 10; i++) {
		printf("%ld\n", (long)b[i]);
	}

	printf("\n-----------------\n");

	float *c = calloc(10, sizeof(float));
	for (int i = 0; i < 10; i++) {
		printf("%.16f\n", c[i]);
	}
	
	return 0;	
}