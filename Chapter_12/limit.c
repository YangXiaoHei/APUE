#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    
#ifdef PTHREAD_DESTRUCTOR_ITERATIONS
    printf("线程退出时操作系统实现试图销毁线程特定数据的最大次数 %d\n", PTHREAD_DESTRUCTOR_ITERATIONS);
#else
    long thread_destructor_iterations = sysconf(_SC_THREAD_DESTRUCTOR_ITERATIONS);
    printf("线程退出时操作系统实现试图销毁线程特定数据的最大次数 %ld\n", thread_destructor_iterations);
#endif
    
#ifdef PTHREAD_KEYS_MAX
    printf("进程可以创建的键的最大数目 %d\n", PTHREAD_KEYS_MAX);
#else
    long keys_max = sysconf(_SC_THREAD_KEYS_MAX);
    printf("进程可以创建的键的最大数目 %ld\n", keys_max);
#endif
    
#ifdef PTHREAD_STACK_MIN
    printf("线程的栈可用的最小字节数 %d\n", PTHREAD_STACK_MIN);
#else
    long stack_min = sysconf(_SC_THREAD_STACK_MIN);
    printf("线程的栈可用的最小字节数 %ld\n", stack_min);
#endif
    
#ifdef PTHREAD_THREADS_MAX
    printf("进程可以创建的最大线程数 %d\n", PTHREAD_THREADS_MAX);
#else
    unsigned long threads_max = sysconf(_SC_THREAD_THREADS_MAX);
    printf("进程可以创建的最大线程数 %lu\n", threads_max);
    if (threads_max == ULONG_MAX) {
        printf("无限制\n");
    }
#endif
    
    
    
    
    
    return 0;
}
