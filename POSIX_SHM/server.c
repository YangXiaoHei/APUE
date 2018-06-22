#include "comm.h"

void init_sem(shared *sh) 
{
    sem_unlink(SHARED_MUTEX_SEM);
    if ((sh->mutex = sem_open(SHARED_MUTEX_SEM, O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    sem_unlink(SHARED_NOVERFLOW_SEM);
    if ((sh->noverflow_mutex = sem_open(SHARED_NOVERFLOW_SEM, O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    sem_unlink(SHARED_NEMPTY_SEM);
    if ((sh->nempty = sem_open(SHARED_NEMPTY_SEM, O_CREAT | O_EXCL, 0644, MSG_NUM)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    sem_unlink(SHARED_NSTROED_SEM);
    if ((sh->nstored = sem_open(SHARED_NSTROED_SEM, O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }
}

void init_shared_mem(shared **vptr) 
{
    shm_unlink(SHARED_MEM);
    shared *ptr = NULL;
    int fd;
    if ((fd = shm_open(SHARED_MEM, O_CREAT | O_RDWR | O_EXCL, 0644)) < 0) {
        perror("shm_open error");
        exit(1);
    }
    if (ftruncate(fd, sizeof(shared)) < 0) {
        perror("ftruncate error");
        exit(1);
    }
    if ((ptr = mmap(NULL, sizeof(shared), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    close(fd);

    for (int i = 0; i < MSG_NUM; i++) 
        ptr->offset[i] = i * MSG_SIZE;

    init_sem(ptr);

    if (vptr != NULL) 
        *vptr = ptr;
}

int main(int argc, char *argv[]) 
{
    shared *ptr = NULL;
    init_shared_mem(&ptr);

    int i = 0, tmp = 0;
    int last_noverflow = 0;
    while (1) {
        sem_wait(ptr->nstored);

        sem_wait(ptr->mutex);
        int off = ptr->offset[i];
        sem_post(ptr->mutex);

        printf("index = %d : %s\n", i, &ptr->data[off]);
        i = (i + 1) % MSG_NUM;

        sem_post(ptr->nempty);

        sem_wait(ptr->noverflow_mutex);
        tmp = ptr->noverflow;
        sem_post(ptr->noverflow_mutex);

        if (tmp != last_noverflow) {
            printf("noverflow = %d\n", tmp);
            last_noverflow = tmp;
        }
    }
    
    return 0;    
}