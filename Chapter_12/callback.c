#include <sys/times.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int make_thread(void *(*thread_fun)(void *), void *arg) {
    pthread_attr_t attr;
    int err = 0;
    if ((err = pthread_attr_init(&attr)) != 0)
        return err;
    if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) != 0)
        return err;
    pthread_t tid;
    err = pthread_create(&tid, &attr, thread_fun, arg);
    pthread_attr_destroy(&attr);
    return err;
}

struct to_info {
    void (*to_fun)(void *);
    void *to_arg;
    struct timespec to_wait;
};

#define SECTONSEC 1000000000

void *timeout_helper(void *arg) {
    struct to_info *tip = (struct to_info *)arg;
    nanosleep(&tip->to_wait, NULL);
    (*tip->to_fun)(tip->to_arg);
    free(arg);
    return 0;
}

void timeout(const struct timespec *when, void (*fun)(void *), void *arg) {
    
    struct timespec now;
    struct to_info *tip;
    int err;
    
    clock_gettime(CLOCK_REALTIME, &now);
    
    if (when->tv_sec < now.tv_sec ||
        (when->tv_sec == now.tv_sec &&
         when->tv_nsec < now.tv_nsec))
        goto run_now;
    
    if ((tip = malloc(sizeof(struct to_info))) == NULL)
        goto run_now;
    
    tip->to_fun = fun;
    tip->to_arg = arg;
    
    tip->to_wait.tv_sec = when->tv_sec - now.tv_sec;
    if (when->tv_nsec >= now.tv_nsec) {
        tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec;
    } else {
        tip->to_wait.tv_sec--;
        tip->to_wait.tv_nsec = SECTONSEC - now.tv_nsec + when->tv_nsec;
    }
    
    if (make_thread(timeout_helper, (void *)tip) == 0)
        return;
    
run_now:
    (*fun)(arg);
}

pthread_mutexattr_t attr;
pthread_mutex_t lock;

void retry(void *arg) {
    pthread_mutex_lock(&lock);
    
    /* do something special */
    printf("啦啦啦~\n");
    
    pthread_mutex_unlock(&lock);
}


int main(int argc, char *argv[]) {
    
    
    int err, condition = 1, arg;
    struct timespec when;
    
    if ((err = pthread_mutexattr_init(&attr)) != 0) {
        printf("pthread_mutexattr_init fail : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) != 0) {
        printf("pthread_mutexattr_settype fail : %s\n", strerror(err));
        exit(1);
    }
    if ((err = pthread_mutex_init(&lock, &attr)) != 0) {
        printf("pthread_mutex_init fail : %s\n", strerror(err));
        exit(1);
    }
    
    pthread_mutex_lock(&lock);
    
    if (condition) {
        clock_gettime(CLOCK_REALTIME, &when);
        when.tv_sec += 3;
        timeout(&when, retry, (void *)(unsigned long)arg);
    }
    pthread_mutex_unlock(&lock);
    
    while (1);
    
    return 0;
}
