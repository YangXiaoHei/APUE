#include "comm.h"

// #define MSG_SIZE 256
// #define MSG_NUM 16

// #define SHARED_MEM              "shared_mem_name"
// #define SHARED_MUTEX_SEM            "shared_mutex_sem_name"
// #define SHARED_NSTROED_SEM      "shared_nstored_sem_name"
// #define SHARED_NEMPTY_SEM       "shared_nempty_sem_name"
// #define SHARED_NOVERFLOW_SEM    "shared_noverflow_sem_name"

// typedef struct shared {
//     sem_t *mutex;
//     sem_t *nstored;
//     sem_t *nempty;
//     sem_t *noverflow_mutex;
//
//     long noverflow;
//     long offset[MSG_NUM];
//     char data[MSG_NUM * MSG_SIZE];
// } shared;

void open_sem(shared *sh) 
{
    if ((sh->mutex = sem_open(SHARED_MUTEX_SEM, O_RDWR)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
    if ((sh->noverflow_mutex = sem_open(SHARED_NOVERFLOW_SEM, O_RDWR)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
    if ((sh->nempty = sem_open(SHARED_NEMPTY_SEM, O_RDWR)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
    if ((sh->nstored = sem_open(SHARED_NSTROED_SEM, O_RDWR)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage : %s <nloop>\n", argv[0]);
        exit(1);
    }
    
    int fd = shm_open(SHARED_MEM, O_RDWR);
    if (fd < 0) {
        perror("shm_open error");
        exit(1);
    }

    shared *ptr = mmap(NULL, sizeof(shared), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    open_sem(ptr);

    int nloop = atoi(argv[1]);
    char buf[MSG_SIZE];
    int nput = 0;
    for (int i = 0; i < nloop; i++) {
        
        // usleep(1000 * 100);
        snprintf(buf, sizeof(buf), "pid = %d : message : %d", getpid(), i);

        if (sem_trywait(ptr->nempty) == -1) {
            if (errno == EAGAIN) {
                sem_wait(ptr->noverflow_mutex);
                ptr->noverflow++;
                sem_post(ptr->noverflow_mutex);
                continue;
            } else {
                perror("sem_trywait error");
                exit(1);
            }
        }
        sem_wait(ptr->mutex);
        int off = ptr->offset[nput];
        sem_post(ptr->mutex);

        nput = (nput + 1) % MSG_NUM;
        strcpy(&ptr->data[off], buf);

        sem_post(ptr->nstored);
    }
    
    return 0;    
}