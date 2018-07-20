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

struct A {
    int a;
    int b;
    int c;
    int d;
};

void pr_struct(const char *msg, struct A *a) {
    printf("%s \n", msg);
    printf("a->a = %d\n", a->a);
    printf("a->b = %d\n", a->b);
    printf("a->c = %d\n", a->c);
    printf("a->d = %d\n", a->d);
}

void *thread_1(void *arg) {
    pr_struct("thread_1: ", arg);
       pthread_exit((void *)arg);
}

void *thread_2(void *arg) {
    pr_struct("thread_2: ",arg);
    pthread_exit((void *)0);
}


int main(int argc, char const *argv[])
{
   struct A *a = malloc(sizeof(struct A));
   a->a = 4;
   a->b = 3;
   a->c = 2;
   a->d = 1;
   int err = 0;
   pthread_t tid1, tid2;
   if ((err = pthread_create(&tid1, NULL, thread_1, a)) != 0) {
       printf("pthread_create fail : %s\n", strerror(err));
       exit(1);
   }
   void *ret;
   if ((err = pthread_join(tid1, &ret)) != 0) {
       printf("pthread_join fail : %s\n", strerror(err));
       exit(1);
   }
   if ((err = pthread_create(&tid2, NULL, thread_2, ret)) != 0) {
       printf("pthread_create fail : %s\n", strerror(err));
       exit(1);
   }
   if ((err = pthread_join(tid2, NULL)) != 0) {
       printf("pthread_join fail : %s\n", strerror(err));
       exit(1);
   }
   pr_struct("main thread:", (struct A*)ret);
   free(a);
    return 0;
}