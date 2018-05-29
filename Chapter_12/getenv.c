#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>

#define MAX_STRING_SZ 4096

static char env_buf[MAX_STRING_SZ];

extern char **environ;

char *yh_getenv(const char *name) {
    int i, len;
    len = strlen(name);
    for (i = 0; environ[i] != NULL; ++i) {
        if (strncmp(name, environ[i], len) == 0 &&
            (environ[i][len] == '=')) {
            strncpy(env_buf, &environ[i][len + 1], MAX_STRING_SZ - 1);
            return env_buf;
        }
    }
    return NULL;
}

void *thr_run(void *arg) {
    while (1) {
        printf("thread【%ld】 USER = %s\n",(long)arg, yh_getenv("USER"));
        usleep(rand() % 100 + 1);
    }
    return (void *)1;
}

int main(int argc, char *argv[]) {

    pthread_t tid[20];
    int err;
    for (int i = 0; i < sizeof(tid) / sizeof(tid[0]); i++) {
        if ((err = pthread_create(tid + i, NULL, thr_run, (void *)(long)i)) != 0) {
            printf("pthread_create fail : %s\n", strerror(err));
            exit(1);
        }
    }
    
    pause();
    
    return 0;
}
