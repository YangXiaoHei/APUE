#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t rw_mutex;
    pthread_cond_t rw_condreaders;
    pthread_cond_t rw_condwriters;
    int     rw_magic;
    int     rw_nwaitreaders;
    int     rw_nwaitwriters;
    int     rw_refcount;
} yh_pthread_rwlock_t;

#define YH_RW_MAGIC    0x19283746

#define YH_PTHREAD_RWLOCK_INITIALIZER { \
    PTHREAD_MUTEX_INITIALIZER,          \
    PTHREAD_COND_INITIALIZER,           \
    PTHREAD_COND_INITIALIZER,           \
    YH_RW_MAGIC,                        \
    0, 0, 0                             \
}

int yh_pthread_rwlock_destory(yh_pthread_rwlock_t *);
int yh_pthread_rwlock_init(yh_pthread_rwlock_t *, pthread_rwlockattr_t *);
int yh_phtread_rwlock_rdlock(yh_pthread_rwlock_t *);
int yh_phtread_rwlock_wrlock(yh_pthread_rwlock_t *);
int yh_pthread_rwlock_tryrdlock(yh_pthread_rwlock_t *);
int yh_pthread_rwlock_trywrlock(yh_pthread_rwlock_t *);
int yh_pthread_rwlock_unlock(yh_pthread_rwlock_t *);
int yh_pthread_wrlock(yh_pthread_rwlock_t *);

int yh_pthread_rwlock_init(yh_pthread_rwlock_t *rwlock, pthread_rwlockattr_t *attr) {

    if (attr != NULL ) 
        return EINVAL;

    int result;

    if ((result = pthread_mutex_init(&rwlock->rw_mutex, NULL)) != 0) 
        goto err1;
    if ((result = pthread_cond_init(&rwlock->rw_condwriters, NULL)) != 0) 
        goto err2;
    if ((result = pthread_cond_init(&rwlock->rw_condreaders, NULL)) != 0) 
        goto err3;

    rwlock->rw_nwaitwriters = 0;
    rwlock->rw_nwaitreaders = 0;
    rwlock->rw_magic = YH_RW_MAGIC;
    rwlock->rw_refcount = 0;

    return 0;

err3:
    pthread_cond_destroy(&rwlock->rw_condwriters);
err2:
    pthread_mutex_destroy(&rwlock->rw_mutex);
err1:
    return result;
}

int yh_pthread_rwlock_destory(yh_pthread_rwlock_t *rwlock) {

    if (rwlock->rw_magic != YH_RW_MAGIC) 
        return EINVAL;

    if (rwlock->rw_nwaitreaders != 0 ||
        rwlock->rw_nwaitwriters != 0 ||
        rwlock->rw_refcount != 0) 
        return EBUSY;

    pthread_mutex_destroy(&rwlock->rw_mutex);
    pthread_cond_destroy(&rwlock->rw_condreaders);
    pthread_cond_destroy(&rwlock->rw_condwriters);
    rwlock->rw_magic = 0;
    return 0;
}

static void yh_rwlock_cancelrdwait(void *arg) {
    yh_pthread_rwlock_t *rwlock = (yh_pthread_rwlock_t *)arg;
    rwlock->rw_nwaitreaders--;
    pthread_mutex_unlock(&rwlock->rw_mutex);
}

static void yh_rwlock_cancelwrwait(void *arg) {
    yh_pthread_rwlock_t *rwlock = (yh_pthread_rwlock_t *)arg;
    rwlock->rw_nwaitwriters--;
    pthread_mutex_unlock(&rwlock->rw_mutex);
}

int yh_phtread_rwlock_rdlock(yh_pthread_rwlock_t *rwlock) {
    int result;

    if (rwlock->rw_magic != YH_RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    while (rwlock->rw_refcount < 0 || rwlock->rw_nwaitwriters > 0) {
        rwlock->rw_nwaitreaders++;
        pthread_cleanup_push(yh_rwlock_cancelrdwait, rwlock);
        result = pthread_cond_wait(&rwlock->rw_condreaders, &rwlock->rw_mutex);
        pthread_cleanup_pop(0);
        rwlock->rw_nwaitreaders--;
        if (result != 0) 
            return result;
    }

    if (result == 0) 
        rwlock->rw_refcount++;

    pthread_mutex_unlock(&rwlock->rw_mutex);
    return result;
}

int yh_pthread_rwlock_wrlock(yh_pthread_rwlock_t *rwlock) {
    int result;

    if (rwlock->rw_magic != YH_RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    while (rwlock->rw_refcount != 0) {
        rwlock->rw_nwaitwriters++;
        pthread_cleanup_push(yh_rwlock_cancelwrwait, rwlock);
        result = pthread_cond_wait(&rwlock->rw_condwriters, &rwlock->rw_mutex);
        pthread_cleanup_pop(0);
        rwlock->rw_nwaitwriters--;
        if (result != 0) 
            return result;
    }

    if (result == 0) 
        rwlock->rw_refcount = -1;

    pthread_mutex_unlock(&rwlock->rw_mutex);
    return result;
}

int yh_pthread_rwlock_unlock(yh_pthread_rwlock_t *rwlock) {

    int result;

    if (rwlock->rw_magic != YH_RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    if (rwlock->rw_refcount > 0) 
        rwlock->rw_refcount--;
    else if (rwlock->rw_refcount == -1)
        rwlock->rw_refcount = 0;
    else {
        printf("rw_refcount = %d", rwlock->rw_refcount);
        exit(1);
    }

    if (rwlock->rw_nwaitwriters > 0) {
        if (rwlock->rw_refcount == 0) 
            result = pthread_cond_signal(&rwlock->rw_condwriters);
    } else if (rwlock->rw_nwaitreaders > 0) 
        result = pthread_cond_broadcast(&rwlock->rw_condreaders);

    pthread_mutex_unlock(&rwlock->rw_mutex);
    return result;
}

int yh_pthread_rwlock_tryrdlock(yh_pthread_rwlock_t *rwlock) {

    int result;

    if (rwlock->rw_magic != YH_RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    if (rwlock->rw_refcount < 0 || rwlock->rw_refcount > 0) 
        result = EBUSY;
    else
        rwlock->rw_refcount++;

    pthread_mutex_unlock(&rwlock->rw_mutex);
    return result;
}

int yh_pthread_rwlock_trywrlock(yh_pthread_rwlock_t *rwlock) {

    int result;

    if (rwlock->rw_magic != YH_RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    if (rwlock->rw_refcount != 0) 
        result = EBUSY;
    else
        rwlock->rw_refcount = -1;

    pthread_mutex_lock(&rwlock->rw_mutex);
    return result;
}



yh_pthread_rwlock_t lock = YH_PTHREAD_RWLOCK_INITIALIZER;
pthread_t t1, t2;

void *thread1(void *arg) {
    yh_phtread_rwlock_rdlock(&lock);
    printf("thread1() got read lock\n");
    sleep(3);
    printf("thread1() after 3s sleep, begin cancel\n");
    pthread_cancel(t2);
    printf("thread1() after cancel\n");
    sleep(3);
    printf("thread1() after 3s sleep, begin unlock\n");
    printf("rw_refcount = %d, "
           "rw_nwaitreaders = %d"
           "rw_nwaitwriters = %d\n",
           lock.rw_refcount, 
           lock.rw_nwaitreaders,
           lock.rw_nwaitwriters);
    yh_pthread_rwlock_unlock(&lock);
    return NULL;
}

void *thread2(void *arg) {
    printf("thread2() trying to obtain a write lock\n");
    yh_pthread_rwlock_wrlock(&lock);
    printf("thread2() got a write lock");
    sleep(1);
    yh_pthread_rwlock_unlock(&lock);
    return NULL;
}

int main(int argc, char *argv[]) {

    void *status;

    pthread_create(&t1, NULL, thread1, NULL);
    sleep(1);
    pthread_create(&t2, NULL, thread2, NULL);

    pthread_join(t2, &status);
    if (status != PTHREAD_CANCELED) 
        printf("thread2 status = %p\n", status);

    pthread_join(t1, &status);
    if (status != NULL) 
        printf("thread1 status = %p\n", status);
    
    printf("rw_refcount = %d, "
           "rw_nwaitreaders = %d"
           "rw_nwaitwriters = %d\n",
           lock.rw_refcount, 
           lock.rw_nwaitreaders,
           lock.rw_nwaitwriters);

    yh_pthread_rwlock_destory(&lock);
    return 0;    
}