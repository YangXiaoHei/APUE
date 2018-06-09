

typedef struct {
    pthread_mutex_t rw_mutex;
    pthread_cond_t rw_condreaders;
    pthread_cond_t rw_condwriters;
    int     rw_magic;
    int     rw_nwaitreaders;
    int     rw_nwaitwriters;
    int     rw_refcount;
} yh_pthread_rwlock_t;

#define RW_MAGIC    0x19283746

#define PTHREAD_RWLOCK_INITIALIZER {    \
    PTHREAD_MUTEX_INITIALIZER,          \
    PTHREAD_COND_INITIALIZER,           \
    PTHREAD_COND_INITIALIZER,           \
    RW_MAGIC,                           \
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

    if ((result = pthread_mutex_init(&rwlock->rw_mutex)) != 0) 
        goto err1;
    if ((result = pthread_cond_init(&rwlock->rw_condwriters)) != 0) 
        goto err2;
    if ((result = pthread_cond_init(&rwlock->rw_condreaders)) != 0) 
        goto err3;

    rwlock->rw_nwaitwriters = 0;
    rwlock->rw_nwaitreaders = 0;
    rwlock->rw_magic = RW_MAGIC;
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

    if (rwlock->rw_magic != RW_MAGIC) 
        return EINVAL;

    if (rwlock->rw_nwaitreaders != 0 ||
        rwlock->rw_nwaitwriters != 0 ||
        rwlock->rw_refcount != 0) 
        return EBUSY;

    pthread_mutex_destroy(rwlock->rw_mutex);
    pthread_cond_destroy(rwlock->rw_condreaders);
    pthread_cond_destroy(rwlock->rw_condwriters);
    rwlock->rw_magic = 0;
    return 0;
}

int yh_phtread_rwlock_rdlock(yh_pthread_rwlock_t *rwlock) {
    int result;

    if (rwlock->rw_magic != RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    while (rwlock->rw_refcount < 0 || rwlock->rw_nwaitwriters > 0) {
        rwlock->rw_condreaders++;
        result = pthread_cond_wait(&rwlock->rw_condreaders, &rwlock->rw_mutex);
        rwlock->rw_condreaders--;
        if (result != 0) 
            return result;
    }

    if (result == 0) 
        rw->rw_refcount++;

    pthread_mutex_unlock(&rwlock->rw_mutex);
    return result;
}

int yh_pthread_rwlock_wrlock(yh_pthread_rwlock_t *rwlock) {
    int result;

    if (rwlock->rw_magic != RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    while (rwlock->rw_refcount != 0) {
        rwlock->rw_nwaitwriters++;
        result = pthread_cond_wait(&rwlock->rw_condwriters, &rwlock->rw_mutex]);
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

    if (rwlock->rw_magic != RW_MAGIC) 
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

    if (rwlock->rw_magic != RW_MAGIC) 
        return EINVAL;

    if ((result = pthread_mutex_lock(&rwlock->rw_mutex)) != 0) 
        return result;

    if (rwlock->rw_refcount < 0 || rwlock->rw_refcount > 0) 
        result = EBUSY;
    else
        rw_refcount++;

    pthread_mutex_unlock(&rwlock->rw_mutex);
    return result;
}

int yh_pthread_rwlock_trywrlock(yh_pthread_rwlock_t *rwlock) {

    int result;

    if (rwlock->rw_magic != RW_MAGIC) 
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

int main(int argc, char *argv[]) {
    return 0;    
}