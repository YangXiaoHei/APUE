#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
 #include <sys/select.h>
#if defined(__gnu_linux__)
#include <mqueue.h>
#endif

static char envbuf[4096];

extern char **environ;

char *getenv(const char *name)
{
    int i, len;
    len = strlen(name);
    for (i = 0; environ[i] != NULL; i++)
    {
        if (strncmp(name, environ[i], len) == 0 && 
            environ[i][len] == '=')
        {
            strncpy(envbuf, &environ[i][len + 1], 4096 - 1);
            return envbuf;
        }
    }
    return NULL;
}

pthread_mutex_t evn_mutex;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void thread_init(void)
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&env_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

int getenv_r(const char *name, char *buf, int buflen) 
{
    int i, len, olen;
    pthread_once(&init_done, thread_init);
    len = strlen(name);
    pthread_mutex_lock(&env_mutex);
    for (i = 0; environ[i] != NULL; i++)
    {
        if (strncmp(environ[i], name, len) == 0 &&
            environ[i][len] == '=')
        {
            olen = strlen(&environ[i][len + 1]);
            if (olen > buflen) {
                pthread_mutex_unlock(&env_mutex);
                return ENOSPC;
            }
            strcpy(buf, &environ[i][len + 1]);
            pthread_mutex_unlock(&env_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&env_mutex);
    return ENOENT;
}


int main(int argc, char const *argv[])
{
    
    return 0;
}