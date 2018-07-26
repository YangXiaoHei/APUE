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

extern char **environ;

void display_environ(char **env)
{
    printf("--------------------\n");
    int i;
    for (i = 0; env[i] != NULL; i++)
        printf("%s\n", env[i]);
    printf("--------------------\n");
}

pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void thread_init()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock, &attr);
    pthread_mutexattr_destroy(&attr);
}

int yh_putenv_r(char *str, char *buf, int buflen, char **new_env, int new_env_len) 
{
    char * eqloc;
    int exist, i, oval_len, nval_len, nname_len, oname_len, osize;
    char **new_environ;
    char *testp;

    eqloc = strstr(str, "=");
    if (eqloc == NULL) 
        return EINVAL;

    nname_len = eqloc - str;
    eqloc++;
    nval_len = strlen(eqloc);

    pthread_once(&init_done, thread_init);
    pthread_mutex_lock(&lock);

    for (i = 0; environ[i] != NULL; i++)
    {
        if (strncmp(str, environ[i], nname_len) == 0 &&
            environ[i][nname_len] == '=')
        {
            oval_len = strlen(&environ[i][nname_len + 1]);
            if (nval_len <= oval_len) 
            {
                strncpy(&environ[i][nname_len + 1], eqloc, nval_len);
                environ[i][nname_len + 1 + nval_len] = 0;
            }
            else
            {
                if (strlen(str) > buflen)
                    goto err;

                strcpy(buf, str);
                environ[i] = buf;
            }
            pthread_mutex_unlock(&lock);
            return 0;
        }
    }

    osize = ++i;
    
    if (osize > new_env_len || strlen(str) > buflen)
        goto err;

    for (i = 0; i < osize; i++)
        new_environ[i] = environ[i];

    strcpy(buf, str);
    new_environ[i - 1] = buf;
    new_environ[i] = NULL;
    environ = new_environ;

    pthread_mutex_unlock(&lock);
    return 0;

err:
    pthread_mutex_unlock(&lock);
    return ENOSPC;
}

int main(int argc, char const *argv[])
{
    char **new_env = malloc(4096);
    char *buf = malloc(4096);

    yh_putenv_r("heihei=hello world", buf, 4096, new_env, 4096);

    display_environ(environ);

    return 0;
}