#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>

extern char **environ;

static void get_store_kv() {
	int argc = 0;
	char buf[20] = { 0 };
	for (int i = 0; environ[i] != NULL; i++) {
		if (strncmp(environ[i], "YH_KEY_", 7) == 0) {
			bzero(buf, sizeof(buf));
			char *eq_p = strstr(environ[i], "=");
			if (eq_p == NULL) {
				printf("envrion get equal fail : %s\n", environ[i]);
				exit(1);
			}
			strncpy(buf, environ[i], eq_p - environ[i]);
			printf("argv[%d] = %s\n", argc++, getenv(buf));
		}
	}
}

int main(int argc, char *argv[]) {
	
	for (int i = 0; i < argc; i++) {
		char key[20] = { 0 };
		char value[100] = { 0 };
		snprintf(key, sizeof(key), "YH_KEY_%d__", i);
		snprintf(value, sizeof(value), "%s", argv[i]);
		if (setenv(key, value, 0) != 0) {
			printf("setenv fail\n");
			exit(1);
		}
	}

	get_store_kv();
	
	return 0;	
}