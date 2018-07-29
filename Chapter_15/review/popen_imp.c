

static pid_t *childpid = NULL;
static int  maxfd;

FILE *popen(const char *cmd, const char *type)
{
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0)
    {
        errno = EINVAL;
        return NULL;
    } 
    if (childpid == NULL)
    {
        maxfd = 256;
        if ((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
            return NULL;
    }

    int fds[2];
    if (pipe(fds) < 0)
        return NULL;

    if (fds[0] >= maxfd || fds[1] >= maxfd)
    {
        close(fds[0]);
        close(fds[1]);
        errno = EMFILE;
        return NULL;
    }

    if ((pid = fork()) < 0)
        return NULL;
    else if (pid == 0)
    {
        if (*type == 'r')
        {
            close(fds[0]);
            if (fds[1] != STDOUT_FILENO)
            {
                dup2(fds[1], STDOUT_FILENO);
                close(fds[1]);
            }
        }
        else
        {
            close(fds[1]);
            if (fds[0] != STDIN_FILENO)
            {
                dup2(fds[0], STDIN_FILENO);
                close(fds[0]);
            }
        }

        for (int i = 0; i < maxfd; i++)
            if (childpid[i] > 0)
                close(i);

        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127);
    }

    if (*type == 'r')
    {
        close(fds[1]);
        if ((fp = fdopen(fds[0], type)) == NULL)
            return NULL;
    }
    else
    {
        close(fds[0]);
        if ((fp = fdopen(fds[1], type)) == NULL)
            return NULL;
    }
    childpid[fileno(fp)] = pid;
    return fp;
}

int pclose(FILE *fp)
{
    if (childpid == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    int fd = fileno(fp);
    if (fd >= maxfd)
    {
        errno = EINVAL;
        return -1;
    }
    if ((pid = childpid[fd]) == 0)
    {
        errno = EINVAL;
        return -1;
    }
    childpid[fd] = 0;
    if (fclose(fp) == EOF) 
        return -1;
    
    int stat;
    while (waitpid(pid, &stat, 0) < 0)
        if (errno != EINTR)
            RETURN -1;

    return stat;
}

int main(int argc, char const *argv[])
{
    
    return 0;
}