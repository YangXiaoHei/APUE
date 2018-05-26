#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

struct foo {
    int a;
    double d;
    long l;
    char *p;
    char msg[10];
};

void pr_struct(const char *msg, const struct foo *f) {
    printf("%s : "
           "f.a = %d\n"
           "f.d = %.3f\n"
           "f.l = %ld\n"
           "f.p = %s\n"
           "f.msg = %s\n",
           msg, f->a, f->d, f->l, f->p, f->msg);
}

void *th1(void *arg) {
    
    struct foo f;
    f.a = 1;
    f.d = 3.14;
    f.l = 10000000001;
    f.p = "hello world";
    f.msg[9] = 0;
    strncpy(f.msg, "123456789", 9);
    
    pthread_exit((void *)&f);
}

void *th2(void *arg) {
    struct foo *f = (struct foo *)arg;
    pr_struct("thread 2", f);
    pthread_exit((void *)1);
}

int main() {
    
    int err;
    pthread_t t1, t2;
    if ((err = pthread_create(&t1, NULL, th1, NULL)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    
    void *ret = NULL;
    if ((err = pthread_join(t1, &ret)) != 0) {
        printf("pthread join fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("thread 1 exit\n");
    pr_struct("main thread 1", ret);
    
    if ((err = pthread_create(&t2, NULL, th2, ret)) != 0) {
        printf("pthread create fail : %s\n", strerror(err));
        exit(1);
    }
    
    void *ret2 = NULL;
    if ((err = pthread_join(t2, &ret2)) != 0) {
        printf("pthread join fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("thread 2 exit with code : %d\n", (int)ret2);
    
    
    pr_struct("main thread 2", ret);
    
    
    
    
    
    
    
    return 0;
}
