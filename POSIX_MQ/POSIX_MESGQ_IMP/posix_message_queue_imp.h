
typedef struct yh_mq_info *yh_mqd_t;

struct yh_mq_attr {
    long mq_flags;
    long mq_maxmsg;
    long mq_msgsize;
    long mq_curmsgs;
};

struct yh_mq_hdr {
    
    /*
     *  每个消息队列头部都有一个结构体 mqh_attr，
     *  用于描述消息队列的属性
     */
    struct yh_mq_attr mqh_attr;
    /*
     * 消息队列中第一条消息的地址（不是内存地址，而是字节数组的索引）
     */
    long    mqh_head;
    long    mqh_free;

    /*
     * 有多少个进程阻塞在 mq_receive 调用中
     */
    long    mqh_nwait;

    /*
     *  如果有进程注册了需要接受队列由空变为非空的通知，
     *  那么记录一下需要接受通知信号的进程 ID
     */
    pid_t   mqh_pid;
    struct  sigevent mqh_event;

    /*
     * 进程间锁和条件变量
     */
    phtread_mutex_t mqh_lock;
    pthread_cond_t  mqh_wait;
};

struct yh_msg_hdr {
    /*
     * 下一条消息的地址（不是内存地址，而是在字节数组中的索引）
     */
    long msg_next;
    /*
     * 该条消息真正的长度
     */
    ssize_t msg_len;
    /*
     * 该条消息的优先级
     */
    unsigned int msg_prio;
};

struct yh_mq_info {
    /*
     * 消息队列头的地址
     */
    struct yh_mq_hdr *mqi_hdr;
    long    mqi_magic;
    int     mqi_flags;
};

#define MQI_MAGIC   0x98765432

#define MSGSIZE_ALIGN(_sz_to_align_) (((_sz_to_align_ + sizeof(long) - 1) / sizeof(long)) * sizeof(long))

#ifdef __bsdi__
#define va_mode_t int
#else
#define va_mode_t mode_t
#endif

extern int yh_mq_open(const char *pathname, int oflag, ...);
extern int yh_mq_unlink(const char *pathname);
extern int yh_mq_close(yh_mqd_t mqd);
extern int yh_mq_getattr(yh_mqd_t mqd, struct yh_mq_attr *mqattr);
extern int yh_mq_setattr(yh_mqd_t mqd, struct yh_mq_attr *new_mqattr, yh_mq_attr *old_mqattr);
extern int yh_mq_notify(yh_mqd_t mqd, const struct sigevent *notification);
extern int yh_mq_send(yh_mqd_t mqd, const char *ptr, size_t len, unsigned int prio);
extern int yh_mq_receive(yh_mqd_t mqd, char *ptr, size_t maxlen, unsigned int *prio);











