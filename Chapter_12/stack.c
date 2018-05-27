#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *thr_run(void *arg) {

    unsigned long stack_lowest = (unsigned long)arg;
    
    char a[99UL << 10];
    
    printf("a 的地址为 %lu\n", (unsigned long)a);
    printf("剩余空间 : %lu\n", (unsigned long)a - stack_lowest);
    
    char b[700];
    
    printf("b 的地址为 %lu\n", (unsigned long)b);
    printf("剩余空间 : %lu\n", (unsigned long)b - stack_lowest);
    
    long l;
    
    printf("l 的地址为 %lu\n", (unsigned long)&l);
    printf("剩余空间 : %lu\n", (unsigned long)&l - stack_lowest);
    
    long ll;
    
    printf("l 的地址为 %lu\n", (unsigned long)&ll);
    printf("剩余空间 : %lu\n", (unsigned long)&ll - stack_lowest);
    
    pthread_exit((void *)1);
}

int main() {
    
    int err = 0;
    pthread_attr_t attr;
    if ((err = pthread_attr_init(&attr)) != 0) {
        printf("thread attr init fail : %s\n", strerror(err));
        exit(1);
    }
    
    size_t stack_size = 100UL << 10;
    void *stack = malloc(stack_size);
    if ((err = pthread_attr_setstack(&attr, stack, stack_size)) != 0) {
        printf("thread attr init fail : %s\n", strerror(err));
        exit(1);
    }
    
    void *get_stack = NULL;
    size_t get_stack_size = 0;
    if ((err = pthread_attr_getstack(&attr, &get_stack, &get_stack_size)) != 0) {
        printf("thread attr init fail : %s\n", strerror(err));
        exit(1);
    }
    
    size_t gurad_size = 0;
    if ((err = pthread_attr_getguardsize(&attr, &gurad_size)) != 0) {
        printf("thread attr init fail : %s\n", strerror(err));
        exit(1);
    }
    
    printf("线程栈结束地址 : %lu 线程栈尺寸 : %lu 保护区大小 : %lu\n", (unsigned long)get_stack, get_stack_size, gurad_size);
    
    
    pthread_t tid;
    if ((err = pthread_create(&tid, &attr, thr_run, get_stack)) != 0) {
        printf("thread create fail : %s\n", strerror(err));
        exit(1);
    }
    
    void *ret = NULL;
    if ((err = pthread_join(tid, &ret)) != 0) {
        printf("thread create fail : %s\n", strerror(err));
        free(stack);
        exit(1);
    }
    
    free(stack);
    
    return 0;
}
