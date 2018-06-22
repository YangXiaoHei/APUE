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
#include <semaphore.h>
#include <sys/mman.h>

#define MSG_SIZE 256
#define MSG_NUM 16

#define SHARED_MEM              "shared_mem_name"
#define SHARED_MUTEX_SEM            "shared_mutex_sem_name"
#define SHARED_NSTROED_SEM      "shared_nstored_sem_name"
#define SHARED_NEMPTY_SEM       "shared_nempty_sem_name"
#define SHARED_NOVERFLOW_SEM    "shared_noverflow_sem_name"

typedef struct shared {
    sem_t *mutex;
    sem_t *nstored;
    sem_t *nempty;
    sem_t *noverflow_mutex;
    
    long noverflow;
    long offset[MSG_NUM];
    char data[MSG_NUM * MSG_SIZE];
} shared;