#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int make_thread(void *(*run)(void *), void *arg, pthread_t *tid_p) {
    
    pthread_attr_t attr;
    pthread_t tid;
    int err;
    
    if ((err = pthread_attr_init(&attr)) != 0)
        return err;
    
    if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) == 0)
        if ((err = pthread_create(&tid, &attr, run, arg)) == 0)
            if (tid_p != NULL)
                *tid_p = tid;
    
    pthread_attr_destroy(&attr);
    return err;
}

void *thr_run(void *arg) {
    printf("哈哈哈\n");
    pthread_exit((void *)1);
}


int main() {
    
    int err = 0;
    pthread_t tid = 0;
    
    if ((err = make_thread(thr_run, (void *)(1L), &tid)) != 0)
        printf("thread create fail : %s\n", strerror(err));
    
    if (err == 0) {
        void *ret = NULL;
        if ((err = pthread_join(tid, &ret)) != 0)
            printf("thread join fail : %s\n", strerror(err));
    }
    

    return 0;
}
