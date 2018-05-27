
extern char **environ;

pthread_mutex_t env_lock;

static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void thread_init(void) {
    
    printf("thread init\n");
    
    pthread_mutexattr_t attr;
    
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&env_lock, &attr);
    pthread_mutexattr_destroy(&attr);
}

int getenv_r(const char *name, char *buf, int buflen) {
    
    pthread_once(&init_done, thread_init);
    len = strlen(name);
    pthread_mutex_lock(&env_lock);
    for (int i = 0; envrion[i] != NULL; i++) {
        if (strncmp(name, environ[i], len) == 0 &&
            environ[i][len] == '=') {
            int olen = strlen(&environ[i][len + 1]);
            if (olen >= buflen) {
                pthread_mutex_unlock(&env_lock);
                return ENOSPC;
            }
            strcpy(buf, &environ[i][len + 1]);
            pthread_mutex_unlock(&env_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&env_lock);
    return ENOENT;
}

int main(int argc, char *argv) {
    
    
    
    
    
    return 0;
}
