
// typedef struct yh_mq_info *yh_mqd_t;

// struct yh_mq_attr {
//     long mq_flags;
//     long mq_maxmsg;
//     long mq_msgsize;
//     long mq_curmsgs;
// };

// struct yh_mq_hdr {
    
//     /*
//      *  每个消息队列头部都有一个结构体 mqh_attr，
//      *  用于描述消息队列的属性
//      */
//     struct yh_mq_attr mqh_attr;
//     /*
//      * 消息队列中第一条消息的地址（不是内存地址，而是字节数组的索引）
//      */
//     long    mqh_head;
//     long    mqh_free;

//     /*
//      * 有多少个进程阻塞在 mq_receive 调用中
//      */
//     long    mqh_nwait;

//     /*
//      *  如果有进程注册了需要接受队列由空变为非空的通知，
//      *  那么记录一下需要接受通知信号的进程 ID
//      */
//     pid_t   mqh_pid;
//     struct  sigevent mqh_event;

//     /*
//      * 进程间锁和条件变量
//      */
//     phtread_mutex_t mqh_lock;
//     pthread_cond_t  mqh_wait;
// };

// struct yh_msg_hdr {
//     /*
//      * 下一条消息的地址（不是内存地址，而是在字节数组中的索引）
//      */
//     long msg_next;
//     /*
//      * 该条消息真正的长度
//      */
//     ssize_t msg_len;
//     /*
//      * 该条消息的优先级
//      */
//     unsigned int msg_prio;
// };

// struct yh_mq_info {
//     /*
//      * 消息队列头的地址
//      */
//     struct yh_mq_hdr *mqi_hdr;
//     long    mqi_magic;
//     int     mqi_flags;
// };

// #define MQI_MAGIC   0x98765432

// #define MSGSIZE_ALIGN(_sz_to_align_) (((_sz_to_align_ + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

// #ifdef __bsdi__
// #define va_mode_t int
// #else
// #define va_mode_t mode_t
// #endif

// extern int yh_mq_open(const char *pathname, int oflag, ...);
// extern int yh_mq_unlink(const char *pathname);
// extern int yh_mq_close(yh_mqd_t mqd);
// extern int yh_mq_getattr(yh_mqd_t mqd, struct yh_mq_attr *mqattr);
// extern int yh_mq_setattr(yh_mqd_t mqd, struct yh_mq_attr *new_mqattr, yh_mq_attr *old_mqattr);
// extern int yh_mq_notify(yh_mqd_t mqd, const struct sigevent *notification);
// extern int yh_mq_send(yh_mqd_t mqd, const char *ptr, size_t len, unsigned int prio);
// extern int yh_mq_receive(yh_mqd_t mqd, char *ptr, size_t maxlen, unsigned int *prio);

#include "posix_message_queue_imp.h"

#define MAX_TRIES 10

struct yh_mq_attr defattr = { 0, 128, 1024, 0 };

yh_mqd_t yh_mq_open(const char *pathname, int oflag, ...) {

    int     i, fd, nonblock,  created, save_errno;
    long    msgsize, filesize, index;
    va_list ap;
    mode_t  mode;
    char    *ptr;
    struct stat     statbuff;
    struct yh_mq_hdr    *mqhdr;
    struct yh_msg_hdr   *msghdr;
    struct yh_mq_attr   *attr;
    struct yh_mq_info   *mqinfo;
    pthread_mutexattr_t mattr;
    pthread_condattr_t  cattr;

    created = 0;
    nonblock = oflag & O_NONBLOCK;
    oflag &= ~O_NONBLOCK;
    mptr = (char *)MAP_FAILED;
    mapinfo = NULL;

again:
    if (oflag & O_CREAT) {
        va_start(ap, oflag);
        mode = va_arg(ap, va_mode_t) & ~S_IXUSR;
        attr = va_arg(ap, struct yh_mq_attr *);
        va_end(ap);

        fd = open(pathname, oflag | O_EXCL | O_RDWR, mode | S_IXUSR);
        if (fd < 0) {
            if (errno == EEXIST && (oflag & O_EXCL) == 0) 
                goto exists;
            else
                return ((yh_mqd_t) -1);
        }

        created = 1;

        if (attr == NULL) 
            attr = &defattr;
        else {
            if (attr->mq_maxmsg <= 0 || attr->mq_msgsize <= 0) {
                errno = EINVAL;
                goto err;
            }
        }

        msgsize = MSGSIZE_ALIGN(attr->mq_msgsize);
        filesize = sizeof(struct yh_mq_hdr) + attr->mq_maxmsg * (sizeof(struct yh_msg_hdr) + msgsize);
        
        if (lseek(fd, filesize - 1, SEEK_SET) == -1) 
            goto err;
        if (write(fd, "", 1) == -1) 
            goto err;
        
        mptr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (mptr == MAP_FAILED) 
            goto err;

        if ( (mqinfo = malloc(sizeof(yh_mq_info))) == NULL) 
            goto err;
        
        mqinfo->mqi_hdr = mqhdr = (struct yh_mq_hdr *)mptr;
        mqinfo->mqi_magic = MQI_MAGIC;
        mqinfo->mqi_flags = nonblock;

        mqhdr->mqh_attr.mq_flags = 0;
        mqhdr->mqh_attr.mq_maxmsg = attr->mq_maxmsg;
        mqhdr->mqh_attr.mq_msgsize = attr->mq_msgsize;
        mqhdr->mqh_attr.mq_curmsgs = 0;
        mqhdr->mqh_nwait = 0;
        mqhdr->mqh_pid = 0;
        mqhdr->mqh_head = 0;

        index = sizeof(struct yh_mq_hdr);
        mqhdr->mqh_free = index;
        for (i = 0; i < attr->mq_maxmsg - 1; i++) {
            msghdr = (struct yh_msg_hdr *)&mptr[index];
            index += sizeof(struct yh_msg_hdr) + msghdr->msg_len;
            msghdr->msg_next = index;
        }
        msghdr = (struct yh_msg_hdr *)&mptr[index];
        msghdr->msg_next = 0;

        if ((i = pthread_mutexattr_init(&mattr)) != 0) 
            goto pthreaderr;

        pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
        i = pthread_mutex_init(&mqhdr->mqh_lock, &mattr);
        pthread_mutexattr_destroy(&mattr);
        if (i != 0) 
            goto pthreaderr;
        
        if ( (i = pthread_condattr_init(&cattr)) != 0) 
            goto pthreaderr;

        pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
        i = pthread_cond_init(&mqhdr->mqh_wait, &cattr);
        if (i != 0) 
            goto pthreaderr;
        
        if (fchmod(fd, mode) == -1) 
            goto err;
        
        close(fd);
        return (yh_mqd_t) mqinfo;
    }
exists:
    
    if ( (fd = open(pathname, O_RDWR)) < 0) {
        if (errno == ENOENT && (oflag & O_CREAT)) 
            goto again;
        goto err;
    }

    for (i = 0; i < MAX_TRIES; i++) {
        if (stat(pathname, &statbuff) == -1) {
            if (errno == ENOENT && (oflag & O_CREAT)) {
                close(fd);
                goto again;
            }
            goto err;
        }

        if ( (statbuff.st_mode & S_IXUSR) == 0) 
            break;
        
        sleep(1);
    }

    if (i == MAX_TRIES) {
        errno = ETIMEOUT;
        goto err;
    }

    filesize = statbuff.st_size;
    mptr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mptr == MAP_FAILED) 
        goto err;
    close(fd);

    if ( (mqinfo = malloc(sizeof(struct yh_mq_info))) == NULL) 
        goto err;

    mqinfo->mqi_hdr = (struct yh_mq_hdr *)mptr;
    mqinfo->mqi_magic = MQI_MAGIC;
    mqinfo->mqi_flags = nonblock;
    return (yh_mqd_t) mqinfo;
pthreaderr:
    errno = i;
err:
    save_errno = errno;
    if (created) 
        unlink(pathname);
    if (mptr == MAP_FAILED) 
        munmap(mptr, filesize);
    if (mqinfo != NULL) 
        free(mqinfo);
    close(fd);
    errno = save_errno;
    return (yh_mqd_t) -1;
}

int yh_mq_close(yh_mqd_t mqd) {
    long msgsize, filesize;
    struct yh_mq_hdr *mqhdr;
    struct yh_mq_attr *attr;
    struct yh_mq_info *mqinfo;
    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return -1;
    }
    mqhdr = mqinfo->mqi_hdr;
    attr = &mqhdr->mqh_attr;

    if (yh_mq_notify(mqd, NULL) != 0) 
        return -1;

    msgsize = MSGSIZE_ALIGN(attr->mq_msgsize);
    filesize = sizeof(struct yh_mq_hdr) + mqhdr->mq_maxmsg * (sizeof(struct yh_msg_hdr) + msgsize);
    
    if (munmap(mqinfo->mqi_hdr, filesize) == -1) 
        return -1;

    mqinfo->mqi_magic = 0;
    free(mqinfo);
    return 0;
}

int yh_mq_unlink(const char *pathname) {
    if (unlink(pathname) == -1) 
        return -1;
    return 0;
}

int yh_mq_getattr(yh_mqd_t mqd, struct yh_mq_attr *mqattr) {
    int     n;
    struct yh_mq_hdr     *mqhdr;
    struct yh_mq_attr    *attr;
    struct yh_mq_info    *mqinfo;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
         errno = EBADF;
         return -1;
    }
    mqhdr = mqinfo->mqi_hdr;
    attr = mqinfo->mqh_attr;
    if ( (n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return -1;
    }
    if (mqattr != NULL) {
        mqattr->mq_flags = mqinfo->mqi_flags;
        mqattr->mq_maxmsg = attr->mq_maxmsg;
        mqattr->mq_msgsize = attr->mq_msgsize;
        mqattr->mq_curmsgs = attr->mq_curmsgs;
    } 
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return 0;
}
int yh_mq_setattr(yh_mqd_t mqd, struct yh_mq_attr *new_mqattr, yh_mq_attr *old_mqattr) {

    struct yh_mq_hdr *mqhdr;
    struct yh_mq_attr *attr;
    struct yh_mq_info *mqinfo;
    int n;

    mqinfo = mqd;
    if (mqinfo->mqi_magic != MQI_MAGIC) {
        errno = EBADF;
        return -1;
    }
    mqhdr = mqinfo->mqi_hdr;
    attr = mqhdr->mqh_attr;
    if ( (n = pthread_mutex_lock(&mqhdr->mqh_lock)) != 0) {
        errno = n;
        return -1;
    }
    if (old_mqattr != NULL) {
        old_mqattr->mq_flags = mqinfo->mq_flags;
        old_mqattr->mq_curmsgs = attr->mq_curmsgs;
        old_mqattr->mq_maxmsg = attr->mq_maxmsg;
        old_mqattr->mq_msgsize = attr->mq_msgsize;
    }
    if (new_mqattr != NULL) {
        
    }
    pthread_mutex_unlock(&mqhdr->mqh_lock);
    return 0;
}
int yh_mq_notify(yh_mqd_t mqd, const struct sigevent *notification);
int yh_mq_send(yh_mqd_t mqd, const char *ptr, size_t len, unsigned int prio);
int yh_mq_receive(yh_mqd_t mqd, char *ptr, size_t maxlen, unsigned int *prio);















