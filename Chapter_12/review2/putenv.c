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

int yh_putenv(char *str) 
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
                if ((environ[i] = malloc(4096)) == NULL)
                    goto malloc_err_1;
                if (environ[i] == NULL)
                    return ENOSPC;
                strcpy(environ[i], str);
            }
            return 0;
        }
    }

    osize = ++i;

    if ((testp = malloc(4096)) == NULL) 
        goto malloc_err_1;

    /* 已经在堆上 */
    if ((unsigned long)testp > (unsigned long)environ)
    {
        environ = realloc(environ, ++osize);

        if ((environ[osize - 2] = malloc(4096)) == NULL)
            goto malloc_err_2;

        strcpy(environ[osize - 2], str);
        environ[osize - 1] = NULL;
    }
    /* 第一次 putenv  */
    else
    {
        if ((new_environ = malloc(++osize)) == NULL) 
            goto malloc_err_2;

        for (i = 0; i < osize - 1; i++)
            new_environ[i] = environ[i];

        if ((new_environ[osize - 2] = malloc(4096)) == NULL)
            goto malloc_err_3;

        strcpy(new_environ[osize - 2], str);
        new_environ[osize - 1] = NULL;

        environ = new_environ;
    }
    return 0;

malloc_err_3:
    free(new_environ);
malloc_err_2:
    free(testp);
malloc_err_1:
    return ENOSPC;
}

int main(int argc, char const *argv[])
{

    return 0;
}