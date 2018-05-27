#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    
    printf("before lock \n");
    pthread_mutex_lock(&lock);
    printf("after lock \n");
    
    
    printf("before lock \n");
    pthread_mutex_lock(&lock);
    printf("after lock \n");
    
    
    return 0;
}
