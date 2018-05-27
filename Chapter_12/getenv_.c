#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>


#define MAX_STRING_SZ 4096
static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_lock = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void thread_init(void) {
    pthread_key_create(&key, free);
}

char *getenv(const char *name) {
    
    int len;
    char *env_buf;
    
    pthread_once(&init_done, thread_init);
    pthread_mutex_lock(&env_lock);
    env_buf = (char *)pthread_getspecific(key);
    if (env_buf == NULL) {
        printf("thread %lu enter\n", (unsigned long)pthread_self());
        env_buf = malloc(MAX_STRING_SZ);
        if (env_buf == NULL) {
            pthread_mutex_unlock(&env_lock);
            return NULL;
        }
        pthread_setspecific(key, env_buf);
    }
    len = strlen(name);
    for (int i = 0; environ[i] != NULL; ++i) {
        if (strncmp(name, environ[i], len) == 0 &&
            environ[i][len] == '=') {
            strncpy(env_buf, &environ[i][len + 1], MAX_STRING_SZ - 1);
            pthread_mutex_unlock(&env_lock);
            return env_buf;
        }
    }
    pthread_mutex_unlock(&env_lock);
    return NULL;
}

void *thread_start(void *arg) {
    while (1) {
        printf("thread %lu USER = %s\n",
               (unsigned long)pthread_self, getenv("USER"));
        sleep(1);
    }
    return (void *)1;
}

int main(int argc, char *argv[]) {
    
    pthread_t tid[2];
    int err = 0;
    for (int i = 0; i < sizeof(tid) / sizeof(tid[0]); i++) {
        if ((err = pthread_create(tid + i, NULL, thread_start, NULL)) != 0) {
            printf("pthread_create fail : %s\n", strerror(err));
            exit(1);
        }
    }
    
    while (1);
    
    return 0;
}
